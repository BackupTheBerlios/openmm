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

Device::Device() :
_mode(ModeDvr),
//_mode(ModeMultiplex),
//_mode(ModeDvrMultiplex),
//
//_blockDvrDevice(false),
//_blockDvrDevice(true),
// _blockDvrDevice = true then reopen device fails (see _reopenDvrDevice), _blockDvrDevice = false then stream has zero length
//_reopenDvrDevice(true)
//_reopenDvrDevice(false)
// renderer in same process as dvb server: reopenDvrDevice = true then dvr device busy, reopenDvrDevice = false then stream sometimes broken
// renderer in different process as dvb server: reopenDvrDevice = true ok, reopenDvrDevice = false then stream sometimes broken
_pMux(0)
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
Device::scan(const std::string& initialTransponderData)
{
    // TODO: allocate adapters and frontend according to device nodes in system
    _adapters[0]->_frontends[0]->scan(initialTransponderData);
    initServiceMap();
}


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


//std::istream*
//Device::getStream(const std::string& serviceName)
//{
//    Transponder* pTransponder = getTransponder(serviceName);
//    Frontend* pFrontend = pTransponder->_pFrontend;
//    // TODO: check if not already tuned to transponder
//    bool tuneSuccess = pFrontend->tune(pTransponder);
//    if (!tuneSuccess) {
//        return 0;
//    }
//
//    Demux* pDemux = pFrontend->_pDemux;
//    Dvr* pDvr = pFrontend->_pDvr;
    // ... if (ModeDvr) ...
//    else if (getMode() == ModeMultiplex) {
//        pDemux->_pMultiplex = new Multiplex;
//        pDemux->selectStream(pDemux->_pMultiplex, Demux::TargetDemux, true);
//        pDemux->_pRemux = new Remux(pDemux->_pMultiplex->_fileDesc);
//        pDemux->addService(pService);
//        pService->startQueueThread();
//        pDemux->runStream(pDemux->_pMultiplex);
//        pDemux->startReadThread();
//        LOG(dvb, debug, "reading full TS ...");
//        pStream = pService->getStream();
//    }
//    else if (getMode() == ModeDvrMultiplex) {
//        Dvr* pDvr = pFrontend->_pDvr;
////        pDvr->openDvr(true);
//        pDvr->_pRemux->addService(pService);
//        for (std::vector<Dvb::Service*>::iterator it = pTransponder->_services.begin(); it != pTransponder->_services.end(); ++it) {
//            pDemux->selectService(*it, Demux::TargetDvr);
//            pDemux->runService(*it, true);
//        }
//        pService->startQueueThread();
//        pDvr->_pRemux->start();
//        LOG(dvb, debug, "reading full TS from dvr device ...");
//        pStream = pService->getStream();
//    }
//    else {
//        pDemux->selectService(pService, Demux::TargetDemux);
//        pDemux->runService(pService, true);
//        _pMux = new Mux;
//        _pMux->addStream(pService->getFirstAudioStream());
//        _pMux->addStream(pService->getFirstVideoStream());
//        _pMux->start();
//        LOG(dvb, debug, "reading from TS muxer ...");
//        pStream = _pMux->getMux();
//
//    //    return pService->getFirstAudioStream()->getStream();
////        return pService->getFirstVideoStream()->getStream();
//    }
//    _streamMap[pStream] = pService;
//    return pStream;
//}


std::istream*
Device::getStream(const std::string& serviceName)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    Transponder* pTransponder = getTransponder(serviceName);
    Frontend* pFrontend = pTransponder->_pFrontend;
    // TODO: check if not already tuned to transponder
    bool tuneSuccess = pFrontend->tune(pTransponder);
    if (!tuneSuccess) {
        return 0;
    }

    Demux* pDemux = pFrontend->_pDemux;
    Dvr* pDvr = pFrontend->_pDvr;
//    Remux* pRemux = pDvr->_pRemux;

    Service* pService = pTransponder->getService(serviceName);
    std::istream* pStream = 0;
    // TODO: check if service not already selected on demuxer

    LOG(dvb, debug, "reading from dvr device ...");
//    pDvr->_pRemux->startRemux();
    pDvr->addService(pService);
    pDemux->selectService(pService, Demux::TargetDvr, false);
    pDemux->runService(pService, true);
//    pService->startQueueThread();

    pStream = pService->getStream();
    _streamMap[pStream] = pService;
    return pStream;
}


void
Device::freeStream(std::istream* pIstream)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_deviceLock);

    // TODO: only stop and free service stream (not complete demux)
    Service* pService = _streamMap[pIstream];
    if (!pService) {
        return;
    }

    Transponder* pTransponder = pService->getTransponder();
    Demux* pDemux = pTransponder->_pFrontend->_pDemux;

    LOG(dvb, debug, "stop reading from dvr device.");
    Dvr* pDvr = pService->getTransponder()->_pFrontend->_pDvr;
//    Remux* pRemux = pDvr->_pRemux;

//    pDvr->_pRemux->stopRemux();
//    pService->stopQueueThread();
    pDemux->runService(pService, false);
//    pDvr->_pRemux->flush();
    pDemux->unselectService(pService);
    pDvr->delService(pService);

    _streamMap.erase(pIstream);


//    if (pDemux->_pMultiplex) {
//        pDemux->stopReadThread();
//        pDemux->runStream(pDemux->_pMultiplex, false);
//        pService->stopQueueThread();
//
//        pDemux->unselectStream(pDemux->_pMultiplex);
//        pDemux->delService(pService);
//        delete pDemux->_pRemux;
//        pDemux->_pRemux = 0;
//        delete pDemux->_pMultiplex;
//        pDemux->_pMultiplex = 0;
//    }
//    else if (getMode() == ModeDvrMultiplex) {
//        Dvr* pDvr = pService->getTransponder()->_pFrontend->_pDvr;
//        pDvr->_pRemux->stop();
//        pService->stopQueueThread();
//        for (std::vector<Dvb::Service*>::iterator it = pTransponder->_services.begin(); it != pTransponder->_services.end(); ++it) {
//            pDemux->runService(*it, false);
//            pDemux->unselectService(*it);
//        }
//        LOG(dvb, debug, "stop reading from dvr device.");
//        pDvr->_pRemux->delService(pService);
//        pDvr->clearBuffer();
////        pDvr->closeDvr();
//    }
//    else {
//        pDemux->runService(pService, false);
//        pDemux->unselectService(pService);
//
//        if (getMode() == ModeDvr) {
//            LOG(dvb, debug, "stop reading from dvr device.");
//            Dvr* pDvr = pService->getTransponder()->_pFrontend->_pDvr;
//            pDvr->_pRemux->stop();
//            pService->stopQueueThread();
//            pDvr->_pRemux->delService(pService);
//            pDvr->clearBuffer();
////            pDvr->closeDvr();
//        }
//        else {
//            LOG(dvb, debug, "stopping TS muxer.");
//            _pMux->stop();
//            delete _pMux;
//            _pMux = 0;
//        }
//    }
//    _streamMap.erase(pIstream);
}



Device::Mode
Device::getMode()
{
    return _mode;
}


//bool
//Device::blockDvrDevice()
//{
//    return _blockDvrDevice;
//}


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
