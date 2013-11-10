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


class PidSelector
{
    friend class Demux;

private:
    PidSelector() : _refCount(1), _fileDesc(-1) {}
    void setFileDesc(int fileDesc);

    int                 _refCount;
    int                 _fileDesc;
    struct pollfd       _fileDescPoll[1];
};


void
PidSelector::setFileDesc(int fileDesc)
{
    _fileDesc = fileDesc;
    _fileDescPoll[0].fd = fileDesc;
    _fileDescPoll[0].events = POLLIN;
}


Demux::Demux(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num)
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
            LOG(dvb, error, "demuxer failed to start/stop service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::selectStream(Stream* pStream, Target target, bool blocking)
{
    Poco::UInt16 pid = pStream->_pid;

    std::map<Poco::UInt16, PidSelector*>::const_iterator it = _pidSelectors.find(pid);
    if (it != _pidSelectors.end()) {
        // pid already selected by another service
        it->second->_refCount++;
        LOG(dvb, debug, "demuxer pid " + Poco::NumberFormatter::format(pid)  + " inc ref counter: " + Poco::NumberFormatter::format(it->second->_refCount));
        return true;
    }

    dmx_pes_type_t pesType;
    // other PES types (audio, video) are only relevant for full featured cards, when feeding elementary streams into decoder
    pesType = DMX_PES_OTHER;

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pid;
    pesfilter.input = DMX_IN_FRONTEND;
    if (target == TargetDvr) {
        pesfilter.output = DMX_OUT_TS_TAP; // send output to dvr device
    }
    else {
        pesfilter.output = DMX_OUT_TAP; // send output to demux device
    }
    pesfilter.pes_type = pesType;
    pesfilter.flags = 0;

    int flag = blocking ? O_RDWR : O_RDWR | O_NONBLOCK;
    int fileDesc;
    if ((fileDesc = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "demuxer failed to open stream: " + std::string(strerror(errno)));
        return false;
    }
    if (ioctl(fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    _pidSelectors[pid] = new PidSelector;
    _pidSelectors[pid]->setFileDesc(fileDesc);
    LOG(dvb, debug, "demuxer selected stream with pid: " + Poco::NumberFormatter::format(pid));
    return true;
}


bool
Demux::unselectStream(Stream* pStream)
{
    Poco::UInt16 pid = pStream->_pid;

    std::map<Poco::UInt16, PidSelector*>::iterator it = _pidSelectors.find(pid);
    if (it == _pidSelectors.end()) {
        LOG(dvb, debug, "demuxer already unselected stream with pid: " + Poco::NumberFormatter::format(pid));
        return false;
    }
    if (it->second->_refCount == 1) {
        if (close(_pidSelectors[pid]->_fileDesc)) {
            LOG(dvb, error, "demuxer closing stream: " + std::string(strerror(errno)));
            return false;
        }
        delete _pidSelectors[pid];
        _pidSelectors.erase(pid);
        LOG(dvb, debug, "demuxer unselected stream with pid: " + Poco::NumberFormatter::format(pid));
    }
    else {
        it->second->_refCount--;
        LOG(dvb, debug, "demuxer pid " + Poco::NumberFormatter::format(pid)  + " dec ref counter: " + Poco::NumberFormatter::format(it->second->_refCount));
    }
    return true;
}


bool
Demux::runStream(Stream* pStream, bool run)
{
    Poco::UInt16 pid = pStream->_pid;

    if (_pidSelectors.find(pid) == _pidSelectors.end()) {
        return false;
    }
    if (_pidSelectors[pid]->_refCount > 1) {
        return true;
    }

    if (ioctl(_pidSelectors[pid]->_fileDesc, run ? DMX_START : DMX_STOP) == -1) {
        LOG(dvb, error, "demuxer failed to " + std::string(run ? "start" : "stop") + " stream with pid: " + Poco::NumberFormatter::format(pid));
        return false;
    }
    LOG(dvb, debug, "demuxer " + std::string(run ? "start" : "stop") + " stream with pid: " + Poco::NumberFormatter::format(pid));
    return true;
}


bool
Demux::setSectionFilter(Stream* pStream, Poco::UInt8 tableId)
{
    Poco::UInt16 pid = pStream->_pid;

    struct dmx_sct_filter_params sectionFilter;

    memset(&sectionFilter, 0, sizeof(sectionFilter));
    sectionFilter.pid = pid;
    sectionFilter.filter.filter[0] = tableId;
    sectionFilter.filter.mask[0] = 0xff;
    sectionFilter.flags |= DMX_CHECK_CRC;

    if (ioctl(_pidSelectors[pid]->_fileDesc, DMX_SET_FILTER, &sectionFilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    else {
        return true;
    }
}


void
Demux::readStream(Stream* pStream, Poco::UInt8* buf, int size, int timeout)
{
    Poco::UInt16 pid = pStream->_pid;
    int bytesToRead = size;
    int bytesRead = 0;

    while (bytesToRead > 0) {
        int pollRes = poll(_pidSelectors[pid]->_fileDescPoll, 1, timeout);
        if (pollRes > 0) {
            if (_pidSelectors[pid]->_fileDescPoll[0].revents & POLLIN){
                bytesRead += ::read(_pidSelectors[pid]->_fileDesc, buf + bytesRead, bytesToRead);
                if (bytesRead > 0) {
                    bytesToRead -= bytesRead;
                }
                else if (bytesRead == -1) {
                    LOG(dvb, error, "demux read failed to read from device: " + std::string(strerror(errno)));
                    throw Poco::Exception("demux read failed to read from device: " + std::string(strerror(errno)));
                }
            }
            else {
                LOG(dvb, warning, "demux read uncatched poll event");
            }
        }
        else if (pollRes == 0) {
            LOG(dvb, trace, "demux read timeout");
            throw Poco::TimeoutException("demux read timeout");
        }
        else if (pollRes == -1) {
            LOG(dvb, error, "demux read failure: " + std::string(strerror(errno)));
            throw Poco::Exception("demux read failure: " + std::string(strerror(errno)));
        }
    }
}


bool
Demux::readSection(Section* pSection)
{
    bool success = true;
    Stream stream(Stream::Other, pSection->packetId());
    selectStream(&stream, Demux::TargetDemux, false);
    setSectionFilter(&stream, pSection->tableId());
    runStream(&stream, true);
    try {
        pSection->read(this, &stream);
        pSection->parse();
    }
    catch (Poco::Exception& e) {
        success = false;
        LOG(dvb, error, pSection->name() + " section read failed");
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
    selectStream(&stream, Demux::TargetDemux, false);
    setSectionFilter(&stream, pTable->getFirstSection()->tableId());
    runStream(&stream, true);
    try {
        pTable->read(this, &stream);
        pTable->parse();
    }
    catch (Poco::Exception& e) {
        success = false;
        LOG(dvb, error, pTable->getFirstSection()->name() + " table read failed");
    }
    runStream(&stream, false);
    unselectStream(&stream);
    return success;
}


}  // namespace Omm
}  // namespace Dvb
