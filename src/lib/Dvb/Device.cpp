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
    _recPsi = true;

//    _pFrontend = new Frontend(this, 0);

}


Adapter::~Adapter()
{
//    delete _pFrontend;

}


void
Adapter::detectFrontends()
{
    std::string frontendBasePath = _deviceName + "/frontend";
    std::set<std::string> frontendFileNames;
    Poco::Glob::glob(frontendBasePath + "?", frontendFileNames);
    for (std::set<std::string>::iterator it = frontendFileNames.begin(); it != frontendFileNames.end(); ++it) {
        try {
            int frontendNum = Poco::NumberParser::parse(it->substr(frontendBasePath.length(), 1));
            Frontend* pFrontend = Frontend::detectFrontend(this, frontendNum);
            if (pFrontend) {
                addFrontend(pFrontend);
            }
            else {
                LOG(dvb, error, "failed to detect frontend " + frontendBasePath + Poco::NumberFormatter::format(frontendNum));
            }
        }
        catch (Poco::Exception& e) {
            LOG(dvb, error, "failed to retrieve frontend number: " + e.message());
        }
    }
}


void
Adapter::addFrontend(Frontend* pFrontend)
{
//    _pFrontend = pFrontend;
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


//Demux*
//Adapter::getDemux()
//{
//    return _pDemux;
//}
//
//
//Dvr*
//Adapter::getDvr()
//{
//    return _pDvr;
//}


void
Adapter::readXml(Poco::XML::Node* pXmlAdapter)
{
    LOG(dvb, debug, "read adapter ...");

    if (pXmlAdapter->hasChildNodes()) {
        Poco::XML::Node* pXmlFrontend = pXmlAdapter->firstChild();
        std::string frontendType = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("type");
        int numFrontend = 0;
        while (pXmlFrontend && pXmlFrontend->nodeName() == "frontend") {
            Frontend* pFrontend;
            if (frontendType == Frontend::DVBS) {
                pFrontend = new SatFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::DVBT) {
                pFrontend = new TerrestrialFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::DVBC) {
                pFrontend = new CableFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::ATSC) {
                pFrontend = new AtscFrontend(this, numFrontend);
            }
            addFrontend(pFrontend);
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
    pDvbDevice->appendChild(pAdapter);
//    if (_pFrontend) {
//        _pFrontend->writeXml(pAdapter);
//    }
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
    for(std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        delete *it;
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


//void
//Device::init()
//{
//    Omm::Dvb::Adapter* pAdapter = new Omm::Dvb::Adapter(0);
//    pAdapter->openAdapter();
//    Omm::Dvb::Device::instance()->addAdapter(pAdapter);
//}


void
Device::detectAdapters()
{
    std::string adapterBasePath = "/dev/dvb/adapter";
    std::set<std::string> adapterFileNames;
    Poco::Glob::glob(adapterBasePath + "?", adapterFileNames);
    for (std::set<std::string>::iterator it = adapterFileNames.begin(); it != adapterFileNames.end(); ++it) {
        try {
            int adapterNum = Poco::NumberParser::parse(it->substr(adapterBasePath.length(), 1));
            Adapter* pAdapter = new Adapter(adapterNum);
            addAdapter(pAdapter);
            pAdapter->detectFrontends();
        }
        catch (Poco::Exception& e) {
            LOG(dvb, error, "failed to retrieve adapter number: " + e.message());
        }
    }
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
    for (std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        (*it)->openAdapter();
    }
    LOG(dvb, debug, "device open finished.");
}


void
Device::close()
{
    LOG(dvb, debug, "device close ...");
    for (std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        (*it)->closeAdapter();
    }
    LOG(dvb, debug, "device close finished.");
}


void
Device::scan()
{
    for (std::vector<Adapter*>::iterator ait = _adapters.begin(); ait != _adapters.end(); ++ait) {
        LOG(dvb, debug, "scan adapter " + (*ait)->_deviceName);
        for (std::vector<Frontend*>::iterator fit = (*ait)->_frontends.begin(); fit != (*ait)->_frontends.end(); ++fit) {
            LOG(dvb, debug, "scan frontend " + (*fit)->_deviceName + " of type: " + (*fit)->getType());
            std::map<std::string, std::set<std::string> >::iterator tsit = _initialTransponders.find((*fit)->getType());
            LOG(dvb, debug, "number of initial transponder lists: " + Poco::NumberFormatter::format(tsit->second.size()));
            if (tsit != _initialTransponders.end()) {
                for (std::set<std::string>::iterator tit = tsit->second.begin(); tit != tsit->second.end(); ++tit) {
//                for (int t = 0; t < tsit->second.size(); ++t) {
                    std::string initialTransponders = (*fit)->getType() + "/" + *tit;
//                    std::string initialTransponders = (*fit)->getType() + "/" + tsit->second[t];
                    LOG(dvb, debug, "scan initial transponders " + initialTransponders);
                    (*fit)->scan(initialTransponders);
                }
            }
        }
    }
    initServiceMap();
}


//void
//Device::scan(const std::string& initialTransponderData)
//{
//    // TODO: allocate adapters and frontend according to device nodes in system
//    _adapters[0]->_frontends[0]->scan(initialTransponderData);
//    initServiceMap();
//}


void
Device::readXml(std::istream& istream)
{
    LOG(dvb, debug, "read device ...");

    clearAdapters();

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
        Poco::XML::Node* pXmlAdapter = pDvbDevice->firstChild();
        int numAdapter = 0;
        while (pXmlAdapter && pXmlAdapter->nodeName() == "adapter") {
            Adapter* pAdapter = new Adapter(numAdapter);
            addAdapter(pAdapter);
            pAdapter->readXml(pXmlAdapter);
            pXmlAdapter = pXmlAdapter->nextSibling();
            numAdapter++;
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
        for (std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
            (*it)->writeXml(pDvbDevice);
        }
        writer.writeNode(ostream, pXmlDoc);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "writing dvb description failed: " + e.displayText());
    }
    LOG(dvb, debug, "wrote device.");
}


void
Device::addAdapter(Adapter* pAdapter)
{
    _adapters.push_back(pAdapter);
}


Transponder*
Device::getTransponder(const std::string& serviceName)
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


    // FIXME: two subsequent getStream() without stopping the stream may lead to
    //        a blocked dvr device: engine stops reading the previous stream
    //        when receiving the new stream. This may overlap and the file
    //        handle is still open. Even if the engine is stopped right before
    //        playing a new stream, it could take a while until reading of stream
    //        is stopped, too (stop() and play() are typically async calls into
    //        the engine).
    //        DvbModel needs a way to interrupt current stream and close file handles.
    //        UPDATE: this only happens when renderer and dvb server run in the same process.
    //        UPDATE: man(2) close:
    //                It is probably unwise to close file descriptors while they may be in use by system calls in other threads  in  the
    //                same  process.  Since a file descriptor may be reused, there are some obscure race conditions that may cause unin-
    //                tended side effects.
    //        UPDATE: Even if the http request is run in the same thread as the previous http request, dvr device cannot be
    //                opened ("Device or resource busy").
    //        UPDATE: when run in main thread (test/tunedvb), opening and closing dvr works
    //        UPDATE: when not run in main thread and dvr device is kept open, streams sometimes seam to be corrupt, but are not!
    //                this is a vlc problem: recording the muxed transport streams can be played with mplayer and ts analysis with
    //                dvbsnoop shows correct transport stream container.


std::istream*
Device::getStream(const std::string& serviceName)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    Transponder* pTransponder = getTransponder(serviceName);
    Frontend* pFrontend = pTransponder->_pFrontend;
    if (!pFrontend->isTuned(pTransponder)) {
        bool tuneSuccess = pFrontend->tune(pTransponder);
        if (!tuneSuccess) {
            return 0;
        }
    }

    Demux* pDemux = pFrontend->_pDemux;
    Dvr* pDvr = pFrontend->_pDvr;

    Service* pService = pTransponder->getService(serviceName);

    LOG(dvb, debug, "reading service stream " + serviceName + " ...");
    pService = pDvr->addService(pService);
    pDemux->selectService(pService, Demux::TargetDvr, false);
    pDemux->runService(pService, true);

    std::istream* pStream = pService->getStream();
    _streamMap[pStream] = pService;
    return pStream;
}


void
Device::freeStream(std::istream* pIstream)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    Service* pService = _streamMap[pIstream];
    if (!pService) {
        return;
    }

    Transponder* pTransponder = pService->getTransponder();
    Demux* pDemux = pTransponder->_pFrontend->_pDemux;

    LOG(dvb, debug, "stop reading service stream " + pService->getName() + ".");
    Dvr* pDvr = pService->getTransponder()->_pFrontend->_pDvr;

    pDemux->runService(pService, false);
    pDemux->unselectService(pService);
    pDvr->delService(pService);

    _streamMap.erase(pIstream);
    delete pIstream;
}


void
Device::initServiceMap()
{
    LOG(dvb, debug, "init service map ...");
    clearServiceMap();

    for (std::vector<Adapter*>::iterator ait = _adapters.begin(); ait != _adapters.end(); ++ait) {
        for (std::vector<Frontend*>::iterator fit = (*ait)->_frontends.begin(); fit != (*ait)->_frontends.end(); ++fit) {
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


}  // namespace Omm
}  // namespace Dvb
