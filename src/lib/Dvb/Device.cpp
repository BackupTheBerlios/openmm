/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include <fstream>
#include <sstream>
#include <cstring>

#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <Poco/File.h>
#include <Poco/Glob.h>
#include <Poco/StreamCopier.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/DOM/AbstractContainerNode.h>
#include <Poco/DOM/DOMException.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/SAX/InputSource.h>

#include <Poco/StringTokenizer.h>
#include <Poco/Thread.h>
#include <map>

#include "Sys/System.h"
#include "Log.h"
#include "DvbLogger.h"
#include "Descriptor.h"
#include "Section.h"
#include "Stream.h"
#include "Service.h"
#include "Transponder.h"
#include "Frontend.h"
#include "Demux.h"
#include "Mux.h"
#include "Remux.h"
#include "Dvr.h"
#include "Device.h"


namespace Omm {
namespace Dvb {


Adapter::Adapter(int num)
{
    _deviceName = "/dev/dvb/adapter" + Poco::NumberFormatter::format(num);
}


Adapter::~Adapter()
{

}


void
Adapter::addFrontend(Frontend* pFrontend)
{
    _frontends.push_back(pFrontend);
}


void
Adapter::openAdapter()
{
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->openFrontend();
    }

}


void
Adapter::closeAdapter()
{
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->closeFrontend();
    }
}


std::string
Adapter::getId()
{
    return _id;
}


void
Adapter::setId(const std::string& id)
{
    _id = id;
}


void
Adapter::readXml(Poco::XML::Node* pXmlAdapter)
{
    LOG(dvb, debug, "read adapter ...");

    if (pXmlAdapter->hasChildNodes()) {
        Poco::XML::Node* pXmlFrontend = pXmlAdapter->firstChild();
        int numFrontend = 0;
        while (pXmlFrontend && pXmlFrontend->nodeName() == "frontend") {
            std::string frontendType = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("type");
            std::string frontendName = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("name");
            if (numFrontend >= _frontends.size()) {
                LOG(dvb, error, "too many frontends for adapter, rescan recommended");
                break;
            }
            Frontend* pFrontend = _frontends[numFrontend];
            if (frontendType != pFrontend->getType()) {
                LOG(dvb, error, "frontend type mismatch, rescan recommended");
                break;
            }
            if (frontendName != pFrontend->getName()) {
                LOG(dvb, error, "frontend name mismatch, rescan recommended");
                break;
            }
            pFrontend->readXml(pXmlFrontend);
            pXmlFrontend = pXmlFrontend->nextSibling();
            numFrontend++;
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no frontends");
        return;
    }

    LOG(dvb, debug, "read adapter.");
}


void
Adapter::writeXml(Poco::XML::Element* pDvbDevice)
{
    LOG(dvb, debug, "write adapter ...");

    Poco::XML::Document* pDoc = pDvbDevice->ownerDocument();
    Poco::XML::Element* pAdapter = pDoc->createElement("adapter");
    pAdapter->setAttribute("id", _id);
    pDvbDevice->appendChild(pAdapter);
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->writeXml(pAdapter);
    }


    LOG(dvb, debug, "wrote adapter.");
}


Device* Device::_pInstance = 0;

Device::Device()
{
}


Device::~Device()
{
    for(std::map<std::string, Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        delete it->second;
    }
}


Device*
Device::instance()
{
    if (!_pInstance) {
        _pInstance = new Device;
    }
    return _pInstance;
}


Device::ServiceIterator
Device::serviceBegin()
{
    return _serviceMap.begin();
}


Device::ServiceIterator
Device::serviceEnd()
{
    return _serviceMap.end();
}


void
Device::addInitialTransponders(const std::string& frontendType, const std::string& initialTransponders)
{
    _initialTransponders[frontendType].insert(initialTransponders);
}


void
Device::open()
{
    LOG(dvb, debug, "device open ...");
    for (std::map<std::string, Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        it->second->openAdapter();
    }
    LOG(dvb, debug, "device open finished.");
}


void
Device::close()
{
    LOG(dvb, debug, "device close ...");
    for (std::map<std::string, Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        it->second->closeAdapter();
    }
    LOG(dvb, debug, "device close finished.");
}


void
Device::scan()
{
    detectAdapters();

    for (std::map<std::string, Adapter*>::iterator ait = _adapters.begin(); ait != _adapters.end(); ++ait) {
        LOG(dvb, debug, "scan adapter " + ait->second->_deviceName);
        for (std::vector<Frontend*>::iterator fit = ait->second->_frontends.begin(); fit != ait->second->_frontends.end(); ++fit) {
            LOG(dvb, debug, "scan frontend " + (*fit)->_deviceName + " of type: " + (*fit)->getType());
            std::map<std::string, std::set<std::string> >::iterator tsit = _initialTransponders.find((*fit)->getType());
            LOG(dvb, debug, "number of initial transponder lists: " + Poco::NumberFormatter::format(tsit->second.size()));
            if (tsit != _initialTransponders.end()) {
                for (std::set<std::string>::iterator tit = tsit->second.begin(); tit != tsit->second.end(); ++tit) {
                    std::string initialTransponders = (*fit)->getType() + "/" + *tit;
                    LOG(dvb, debug, "scan initial transponders " + initialTransponders);
                    (*fit)->scan(initialTransponders);
                }
            }
        }
    }

    initServiceMap();
}


void
Device::readXml(std::istream& istream)
{
    LOG(dvb, debug, "read device ...");

    Poco::AutoPtr<Poco::XML::Document> pXmlDoc = new Poco::XML::Document;
    Poco::XML::InputSource xmlFile(istream);

    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    try {
        pXmlDoc = parser.parse(&xmlFile);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "parsing dvb description failed: " + e.displayText());
        return;
    }

    Poco::XML::Node* pDvbDevice = pXmlDoc->documentElement();
    if (!pDvbDevice || pDvbDevice->nodeName() != "device") {
        LOG(dvb, error, "xml not a valid dvb description");
        return;
    }
    if (pDvbDevice->hasChildNodes()) {

        detectAdapters();

        Poco::XML::Node* pXmlAdapter = pDvbDevice->firstChild();
        while (pXmlAdapter && pXmlAdapter->nodeName() == "adapter") {
            std::string adapterId = static_cast<Poco::XML::Element*>(pXmlAdapter)->getAttribute("id");
            std::map<std::string, Adapter*>::iterator it = _adapters.find(adapterId);
            if (it != _adapters.end()) {
                it->second->readXml(pXmlAdapter);
            }
            else {
                LOG(dvb, error, "could not find adapter with id: " + adapterId + " on system, skipping (rescan recommended)");
            }
            pXmlAdapter = pXmlAdapter->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no adapters");
        return;
    }
    initServiceMap();

    LOG(dvb, debug, "read device.");
}


void
Device::writeXml(std::ostream& ostream)
{
    LOG(dvb, debug, "write device ...");
    Poco::AutoPtr<Poco::XML::Document> pXmlDoc = new Poco::XML::Document;

    Poco::XML::DOMWriter writer;
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);

    Poco::XML::Element* pDvbDevice = pXmlDoc->createElement("device");
    pXmlDoc->appendChild(pDvbDevice);
    try {
        for (std::map<std::string, Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
            it->second->writeXml(pDvbDevice);
        }
        writer.writeNode(ostream, pXmlDoc);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "writing dvb description failed: " + e.displayText());
    }
    LOG(dvb, debug, "wrote device.");
}


Transponder*
Device::getFirstTransponder(const std::string& serviceName)
{
    ServiceIterator it = _serviceMap.find(serviceName);
    if (it != _serviceMap.end() && it->second.size()) {
        return it->second[0];
    }
    else {
        LOG(dvb, error, "could not find transponder for service: " + serviceName);
        return 0;
    }
}


std::vector<Transponder*>&
Device::getTransponders(const std::string& serviceName)
{
    return _serviceMap.find(serviceName)->second;
}


std::istream*
Device::getStream(const std::string& serviceName)
{
    LOG(dvb, debug, "get stream: " + serviceName);

    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    // scrambled services are not supported, yet
    Transponder* pTransponder = tuneToService(serviceName, true);
    if (!pTransponder) {
        return 0;
    }
    Service* pService = pTransponder->getService(serviceName);
    pService = startService(pService);
    std::istream* pStream = pService->getStream();
    _streamMap[pStream] = pService;
    return pStream;
}


void
Device::freeStream(std::istream* pIstream)
{
    LOG(dvb, debug, "free stream ...");

    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    Service* pService = _streamMap[pIstream];
    if (!pService) {
        return;
    }
    stopService(pService);
    _streamMap.erase(pIstream);
    delete pIstream;

    LOG(dvb, debug, "free stream finished.");
}


void
Device::detectAdapters()
{
    clearAdapters();

    std::vector<Sys::Device*> dvbDevices;
    Sys::System::instance()->getDevicesForType(dvbDevices, Sys::System::DeviceTypeDvb);

    for (std::vector<Sys::Device*>::iterator it = dvbDevices.begin(); it != dvbDevices.end(); ++it) {
        std::string adapterId = (*it)->getId().substr(0, (*it)->getId().length() - std::string(".frontendX").length());
        LOG(dvb, debug, "found dvb device node " + (*it)->getNode() + " with id: " + adapterId);

        std::string deviceNode = (*it)->getNode();
        Poco::StringTokenizer deviceNodePath(deviceNode, "/");
        if (deviceNodePath.count() < 2) {
            LOG(dvb, error, "failed to detect dvb adapters, device path too short: " + deviceNode);
            return;
        }
        try {
            if (deviceNodePath[deviceNodePath.count() - 1].substr(0, std::string("frontend").length()) == "frontend") {
                int adapterNum = Poco::NumberParser::parse(deviceNodePath[deviceNodePath.count() - 2].substr(std::string("adapter").length()));
                int frontendNum = Poco::NumberParser::parse(deviceNodePath[deviceNodePath.count() - 1].substr(std::string("frontend").length()));
                Adapter* pAdapter = addAdapter(adapterId, adapterNum);
                Frontend* pFrontend = Frontend::detectFrontend(pAdapter, frontendNum);
                if (pFrontend) {
                    pAdapter->addFrontend(pFrontend);
                }
                else {
                    LOG(dvb, error, "failed to detect frontend " + (*it)->getNode());
                }
            }
        }
        catch (Poco::Exception& e) {
            LOG(dvb, error, "failed to detect dvb device numbers: " + e.message());
        }
    }
}


Adapter*
Device::addAdapter(const std::string& id, int adapterNum)
{
    LOG(dvb, debug, "add adapter number " + Poco::NumberFormatter::format(adapterNum) + " with id: " + id);

    Adapter* pAdapter;
    std::map<std::string, Adapter*>::iterator it = _adapters.find(id);
    if (it == _adapters.end()) {
        pAdapter = new Adapter(adapterNum);
        pAdapter->setId(id);
        _adapters[id] = pAdapter;
    }
    else {
        pAdapter = _adapters[id];
    }
    return pAdapter;
}


void
Device::initServiceMap()
{
    LOG(dvb, debug, "init service map ...");
    clearServiceMap();

    for (std::map<std::string, Adapter*>::iterator ait = _adapters.begin(); ait != _adapters.end(); ++ait) {
        for (std::vector<Frontend*>::iterator fit = ait->second->_frontends.begin(); fit != ait->second->_frontends.end(); ++fit) {
            for (std::vector<Transponder*>::iterator tit = (*fit)->_transponders.begin(); tit != (*fit)->_transponders.end(); ++tit) {
                for (std::vector<Service*>::iterator sit = (*tit)->_services.begin(); sit != (*tit)->_services.end(); ++sit) {
                    _serviceMap[(*sit)->getName()].push_back(*tit);
//                    LOG(dvb, trace, (*sit)->getName() + " service map has " + Poco::NumberFormatter::format(_serviceMap[(*sit)->getName()].size()) + " transponders");
//                    LOG(dvb, trace, "first transponder has freq: " + Poco::NumberFormatter::format(_serviceMap[(*sit)->getName()][0]->getFrequency()));
                }
            }
        }
    }
    LOG(dvb, debug, "init service map finished.");
}


void
Device::clearServiceMap()
{
    // TODO: delete whole adapter tree
    _serviceMap.clear();
}


void
Device::clearAdapters()
{
    // TODO: delete whole adapter tree
    _adapters.clear();
}


Transponder*
Device::tuneToService(const std::string& serviceName, bool unscrambledOnly)
{
    std::vector<Transponder*>& transponders = getTransponders(serviceName);

    Transponder* pTransponder = 0;
    bool tuneSuccess = false;
    LOG(dvb, debug, "number of available frontends: " + Poco::NumberFormatter::format(transponders.size()));
    for (int t = 0; t < transponders.size(); t++) {
        LOG(dvb, debug, "try frontend " + Poco::NumberFormatter::format(t));
        pTransponder = transponders[t];
        Service* pService = pTransponder->getService(serviceName);
        if (unscrambledOnly && pService->getScrambled()) {
            LOG(dvb, debug, "service is scrambled on this transponder, skipping");
            continue;
        }

        Frontend* pFrontend = pTransponder->_pFrontend;
        if (!pFrontend->isTuned()) {
            LOG(dvb, debug, "frontend not tuned, doing so");
            if (!pFrontend->tune(pTransponder)) {
                if (transponders.size() > t + 1) {
                    LOG(dvb, debug, "failed to tune to transponder, trying next one");
                    continue;
                }
                else {
                    tuneSuccess = false;
                    break;
                }
            }
            else {
                tuneSuccess = true;
                break;
            }
        }
        if (!pFrontend->isTunedTo(pTransponder)) {
            if (transponders.size() > t + 1) {
                // there are more frontends available that can tune to a transponder with this service
                LOG(dvb, debug, "available frontend already tuned to different transponder, try next frontend");
                continue;
            }
            else {
                // interrupt the services on current transponder and tune to newly requested one
                LOG(dvb, debug, "no more frontends available, interrupt services and tune to different transponder");
                stopServiceStreamsOnTransponder(pFrontend->_pTunedTransponder);
                if (!pFrontend->tune(pTransponder)) {
                    if (transponders.size() > t + 1) {
                        LOG(dvb, debug, "failed to tune to transponder, trying next one");
                        continue;
                    }
                    else {
                        tuneSuccess = false;
                        break;
                    }
                }
                else {
                    tuneSuccess = true;
                    break;
                }
            }
        }
        else {
            LOG(dvb, debug, "frontend already tuned to requested transponder, skip tuning");
            tuneSuccess = true;
            break;
        }
    }
    if (!tuneSuccess) {
        LOG(dvb, error, "failed to tune to transponder");
        return 0;
    }
    else {
        return pTransponder;
    }
}


Service*
Device::startService(Service* pService)
{
    LOG(dvb, debug, "reading service stream " + pService->getName() + " ...");

    Transponder* pTransponder = pService->getTransponder();
    Frontend* pFrontend = pTransponder->_pFrontend;
    Demux* pDemux = pFrontend->_pDemux;
    Dvr* pDvr = pFrontend->_pDvr;

    pService = pDvr->addService(pService);
    pDemux->selectService(pService, Demux::TargetDvr, false);
    pDemux->runService(pService, true);
    pTransponder->markServiceStarted(pService);

    return pService;
}


void
Device::stopService(Service* pService)
{
    LOG(dvb, debug, "stop reading service stream " + pService->getName() + ".");

    Transponder* pTransponder = pService->getTransponder();
    Demux* pDemux = pTransponder->_pFrontend->_pDemux;
    Dvr* pDvr = pTransponder->_pFrontend->_pDvr;

    pDemux->runService(pService, false);
    pDemux->unselectService(pService);
    pDvr->delService(pService);
    pTransponder->markServiceStopped(pService);
}


void
Device::stopServiceStreamsOnTransponder(Transponder* pTransponder)
{
    Demux* pDemux = pTransponder->_pFrontend->_pDemux;
    Dvr* pDvr = pTransponder->_pFrontend->_pDvr;
    std::set<Dvb::Service*>& services = pTransponder->runningServices();
    for (std::set<Dvb::Service*>::iterator it = services.begin(); it != services.end(); ++it) {
        LOG(dvb, debug, "stop reading service stream " + (*it)->getName() + ".");
        (*it)->stopStream();
    }
    services.clear();
}


}  // namespace Omm
}  // namespace Dvb
