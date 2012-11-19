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
#include <linux/dvb/dmx.h>
#include <sys/poll.h>

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
#include "Remux.h"
#include "Device.h"


namespace Omm {
namespace Dvb {

Demux::Demux(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num)
//_pMultiplex(0),
//_pRemux(0)
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
    // FIXME: only select stream, if service doesn't have it already
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
    LOG(dvb, debug, "demuxer " + std::string(run ? "start" : "stop") + " service: " + pService->getName());
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
    if (!incPidRefCount(pStream->_pid)) {
        // pid already selected by another service
        return true;
    }

    dmx_pes_type_t pesType;
    // other PES types (audio, video) are only relevant for full featured cards,
    // when feeding elementary streams into decoder
    pesType = DMX_PES_OTHER;

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
//        pStream->_pStream = new UnixFileIStream(pStream->_fileDesc);
        pStream->_fileDescPoll[0].fd = pStream->_fileDesc;
        pStream->_fileDescPoll[0].events = POLLIN;
        LOG(dvb, debug, "demuxer selected stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
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
    if (!decPidRefCount(pStream->_pid)) {
        // pid still selected by another service
        return true;
    }

    bool success = true;
    if (close(pStream->_fileDesc)) {
        LOG(dvb, error, "closing elementary stream: " + std::string(strerror(errno)));
        success = false;
    }
    if (success) {
//        delete pStream->_pStream;
//        pStream->_pStream = 0;
        pStream->_fileDesc = -1;
        LOG(dvb, debug, "demuxer unselected stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
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
    if (pidRefCount(pStream->_pid) > 1) {
        return true;
    }

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
        LOG(dvb, debug, "demuxer " + std::string(run ? "start" : "stop") + " stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
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


int
Demux::pidRefCount(Poco::UInt16 pid)
{
    if (_pidRefCount.find(pid) == _pidRefCount.end()) {
        return 0;
    }
    else {
        return _pidRefCount[pid];
    }
}


bool
Demux::incPidRefCount(Poco::UInt16 pid)
{
    std::map<Poco::UInt16, int>::const_iterator it = _pidRefCount.find(pid);
    if (it == _pidRefCount.end()) {
        _pidRefCount[pid] = 1;
        LOG(dvb, debug, "demux inc pid " + Poco::NumberFormatter::format(pid)  + " ref counter: " + Poco::NumberFormatter::format(_pidRefCount[pid]));
        return true;
    }
    else {
        _pidRefCount[pid]++;
        LOG(dvb, debug, "demux inc pid " + Poco::NumberFormatter::format(pid)  + " ref counter: " + Poco::NumberFormatter::format(_pidRefCount[pid]));
        return false;
    }
}


bool
Demux::decPidRefCount(Poco::UInt16 pid)
{
    std::map<Poco::UInt16, int>::iterator it = _pidRefCount.find(pid);
    if (it == _pidRefCount.end()) {
        LOG(dvb, debug, "cannot decrease demux pid refcount");
        return false;
    }
    else {
        if (_pidRefCount[pid] == 1) {
            _pidRefCount.erase(it);
            LOG(dvb, debug, "demux dec erase pid " + Poco::NumberFormatter::format(pid));
            return true;
        }
        else {
            _pidRefCount[pid]--;
            LOG(dvb, debug, "demux dec pid " + Poco::NumberFormatter::format(pid)  + " ref counter: " + Poco::NumberFormatter::format(_pidRefCount[pid]));
            return false;
        }
    }
}


}  // namespace Omm
}  // namespace Dvb
