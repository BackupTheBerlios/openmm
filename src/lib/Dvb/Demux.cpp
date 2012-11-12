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

#include <fcntl.h>
#include <sys/ioctl.h>

#include <Poco/NumberParser.h>
#include <Poco/Timestamp.h>
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

#include "DvbLogger.h"
#include "Stream.h"
#include "TransportStream.h"
#include "Section.h"
#include "Service.h"
#include "Demux.h"
#include "Device.h"
#include "Dvb/Service.h"
#include "Dvb/TransportStream.h"


namespace Omm {
namespace Dvb {

Demux::Demux(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num),
_pMultiplex(0),
_pReadThread(0),
_readThreadRunnable(*this, &Demux::readThread),
_readThreadRunning(false),
_readTimeout(1)
{
    _deviceName = _pAdapter->_deviceName + "/demux" + Poco::NumberFormatter::format(_num);
}


Demux::~Demux()
{
//    closeDemux();
}


bool
Demux::selectService(Service* pService, Target target, bool blocking)
{
    // FIXME: only add stream, if service doesn't have it already
    pService->addStream(new Stream(Stream::Other, 0));
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!selectStream(*it, target, blocking)) {
            LOG(dvb, error, "demuxer failed to select service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::unselectService(Service* pService)
{
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!unselectStream(*it)) {
            LOG(dvb, error, "demuxer failed to unselect service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::runService(Service* pService, bool run)
{
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!runStream(*it, run)) {
            LOG(dvb, error, "demuxer failed to run service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::selectStream(Stream* pStream, Target target, bool blocking)
{
    dmx_pes_type_t pesType;
//    if (pStream->_type == Stream::Audio) {
//        pesType = DMX_PES_AUDIO;
//    }
//    else if (pStream->_type == Stream::Video) {
//        pesType = DMX_PES_VIDEO;
//    }
//    else if (pStream->_type == Stream::ProgramClock) {
//        pesType = DMX_PES_PCR;
//    }
//    else {
        pesType = DMX_PES_OTHER;
//    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pStream->_pid;
    pesfilter.input = DMX_IN_FRONTEND;
    if (target == TargetDvr) {
        pesfilter.output = DMX_OUT_TS_TAP; // send output to dvr device
    }
    else {
        pesfilter.output = DMX_OUT_TAP; // send output to demux device
    }
    pesfilter.pes_type = pesType;
    pesfilter.flags = 0;

    bool success = true;
    int flag = blocking ? O_RDWR : O_RDWR | O_NONBLOCK;
    if ((pStream->_fileDesc = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "opening elementary stream: " + std::string(strerror(errno)));
        success = false;
    }
    if (success && ioctl(pStream->_fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        success = false;
    }
    if (success) {
        pStream->_pStream = new UnixFileIStream(pStream->_fileDesc);
        pStream->_fileDescPoll[0].fd = pStream->_fileDesc;
        pStream->_fileDescPoll[0].events = POLLIN;
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to select stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::unselectStream(Stream* pStream)
{
    bool success = true;
    if (close(pStream->_fileDesc)) {
        LOG(dvb, error, "closing elementary stream: " + std::string(strerror(errno)));
        success = false;
    }
    if (success) {
        delete pStream->_pStream;
        pStream->_pStream = 0;
        pStream->_fileDesc = -1;
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to unselect stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::runStream(Stream* pStream, bool run)
{
    bool success = true;
    if (run) {
        if (ioctl(pStream->_fileDesc, DMX_START) == -1) {
            LOG(dvb, error, "starting stream: " + std::string(strerror(errno)));
            success = false;
        }
    }
    else {
        if (ioctl(pStream->_fileDesc, DMX_STOP) == -1) {
            LOG(dvb, error, "stopping stream: " + std::string(strerror(errno)));
            success = false;
        }
    }
    if (success) {
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to set run state of stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::setSectionFilter(Stream* pStream, Poco::UInt8 tableId)
{
    struct dmx_sct_filter_params sectionFilter;

    memset(&sectionFilter, 0, sizeof(sectionFilter));
    sectionFilter.pid = pStream->_pid;
    sectionFilter.filter.filter[0] = tableId;
    sectionFilter.filter.mask[0] = 0xff;
    sectionFilter.flags |= DMX_CHECK_CRC;

    if (ioctl(pStream->_fileDesc, DMX_SET_FILTER, &sectionFilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    else {
        return true;
    }
}


bool
Demux::readSection(Section* pSection)
{
    bool success = true;
    Stream stream(Stream::Other, pSection->packetId());
    selectStream(&stream, Demux::TargetDemux, true);
    setSectionFilter(&stream, pSection->tableId());
    runStream(&stream, true);
    try {
        pSection->read(&stream);
        pSection->parse();
    }
    catch (Poco::TimeoutException& e) {
        success = false;
        LOG(dvb, error, pSection->name() + " section read timeout");
    }
    runStream(&stream, false);
    unselectStream(&stream);
    return success;
}


bool
Demux::readTable(Table* pTable)
{
    bool success = true;
    Stream stream(Stream::Other, pTable->getFirstSection()->packetId());
    selectStream(&stream, Demux::TargetDemux, true);
    setSectionFilter(&stream, pTable->getFirstSection()->tableId());
    runStream(&stream, true);
    try {
        pTable->read(&stream);
        pTable->parse();
    }
    catch (Poco::TimeoutException& e) {
        success = false;
        LOG(dvb, error, pTable->getFirstSection()->name() + " table read timeout");
    }
    runStream(&stream, false);
    unselectStream(&stream);
    return success;
}



TransportStreamPacket*
Demux::getTransportStreamPacket(int timeout)
{
    if (!_pMultiplex) {
        LOG(dvb, error, "frontend not tuned, cannot get multiplex.");
        return 0;
    }

    try {
        TransportStreamPacket* pPacket = new TransportStreamPacket;
        _pMultiplex->read((Poco::UInt8*)pPacket->getData(), pPacket->getSize(), timeout);
        if (pPacket->getBytes<Poco::UInt8>(0) != pPacket->getSyncByte()) {
            LOG(dvb, error, "TS packet wrong sync byte: " + Poco::NumberFormatter::formatHex(pPacket->getBytes<Poco::UInt8>(0)));
            return 0;
        }
        else {
            return pPacket;
        }
    }
    catch(Poco::Exception& e) {
        LOG(dvb, error, "timeout while reading TS packet (" + e.displayText() + ")");
        return 0;
    }
}


void
Demux::addService(Service* pService)
{
    _pServices.insert(pService);
}


void
Demux::delService(Service* pService)
{
    _pServices.erase(pService);
}


void
Demux::startReadThread()
{
    LOG(dvb, debug, "start TS remux thread ...");

//    std::ofstream rawTs("raw.ts");
////    const int bufSize = 188 * 10000 * 3;
//    const int bufSize = 188 * 1000 * 3;
////    const int bufSize = 188 * 100 * 3;
//    Poco::UInt8 buf[bufSize];
//    _pMultiplex->read(buf, bufSize, 0);
//    rawTs.write((const char*)buf, bufSize);
//    LOG(dvb, debug, "TS remux thread finished.");
//    return;

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
Demux::stopReadThread()
{
    LOG(dvb, debug, "stop TS remux thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        if (!_pReadThread->tryJoin(_readTimeout * 1000)) {
            LOG(dvb, error, "failed to join TS remux thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }

    LOG(dvb, debug, "TS remux thread stopped.");
}


bool
Demux::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


void
Demux::readThread()
{
    LOG(dvb, debug, "TS remux thread started.");

    Poco::Timestamp t;
    long unsigned int tsPacketCounter = 0;
    Poco::UInt8 continuityCounter = 0;

    while (readThreadRunning()) {
        TransportStreamPacket* pTsPacket = getTransportStreamPacket(_readTimeout);
        if (!pTsPacket) {
            break;
        }
        tsPacketCounter++;

//        rawTs.write((char*)pTsPacket->getData(), pTsPacket->getSize());
//        if (tsPacketCounter == 100) {
//            break;
//        }
//        continue;

        Poco::UInt16 pid = pTsPacket->getPacketIdentifier();
//        LOG(dvb, information, "demux received packet no: " + Poco::NumberFormatter::format(tsPacketCounter) + ", pid: " + Poco::NumberFormatter::format(pid));
        for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
            if (pid == 0) {
                (*it)->_pTsPacket->setContinuityCounter(continuityCounter);
                continuityCounter++;
                continuityCounter %= 16;
                (*it)->_byteQueue.write((char*)(*it)->_pTsPacket->getData(), (*it)->_pTsPacket->getSize());
            }
            else if ((*it)->hasPacketIdentifier(pid)) {
                (*it)->_byteQueue.write((char*)pTsPacket->getData(), pTsPacket->getSize());
            }
        }
        delete pTsPacket;
    }
    for (std::set<Service*>::const_iterator it = _pServices.begin(); it != _pServices.end(); ++it) {
        (*it)->flushStream();
    }

    LOG(dvb, information, "remux received " + Poco::NumberFormatter::format(tsPacketCounter) + " TS packets in "
            + Poco::NumberFormatter::format(t.elapsed() / 1000) + " msec ("
            + Poco::NumberFormatter::format((float)tsPacketCounter * 1000 / t.elapsed(), 2) + " packets/msec)");
    LOG(dvb, debug, "TS remux thread finished.");
}

}  // namespace Omm
}  // namespace Dvb
