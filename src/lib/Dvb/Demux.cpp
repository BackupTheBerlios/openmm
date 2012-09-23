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
#include "Section.h"
#include "Service.h"
#include "Demux.h"
#include "Device.h"


namespace Omm {
namespace Dvb {

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


void
Demux::readSection(Section* pSection, int timeout)
{
    Stream stream(Stream::Other, pSection->packetId());
    selectStream(&stream, Demux::TargetDemux, true);
    setSectionFilter(&stream, pSection->tableId());
    runStream(&stream, true);
    try {
        pSection->read(&stream, timeout);
        pSection->parse();
    }
    catch (Poco::TimeoutException& e) {
        LOG(dvb, error, pSection->name() + " table read timeout");
    }
    runStream(&stream, false);
    unselectStream(&stream);
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
        pSection->read(&stream, pSection->timeout());
        pSection->parse();
    }
    catch (Poco::TimeoutException& e) {
        success = false;
        LOG(dvb, error, pSection->name() + " table read timeout");
    }
    runStream(&stream, false);
    unselectStream(&stream);
    return success;
}


bool
Demux::setSectionFilter(Stream* pStream, uint8_t filter[18], uint8_t mask[18])
{
    struct dmx_sct_filter_params sctfilter;

    memset(&sctfilter, 0, sizeof(sctfilter));
    sctfilter.pid = pStream->_pid;
    memcpy(sctfilter.filter.filter, filter, 1);
    memcpy(sctfilter.filter.filter+1, filter+3, 15);
    memcpy(sctfilter.filter.mask, mask, 1);
    memcpy(sctfilter.filter.mask+1, mask+3, 15);
    memset(sctfilter.filter.mode, 0, 16);
    sctfilter.flags |= DMX_CHECK_CRC;

    if (ioctl(pStream->_fileDesc, DMX_SET_FILTER, &sctfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    else {
        return true;
    }
}


bool
Demux::setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType)
{
    if (pid >= 0x1fff) {  // ignore this pid to allow radio services
        return true;
    }

//    int buffersize = 64 * 1024;
//    if (ioctl(fileDesc, DMX_SET_BUFFER_SIZE, buffersize) == -1) {
//        LOG(dvb, error, "DMX_SET_BUFFER_SIZE failed: " + std::string(strerror(errno)));
//    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pid;
    pesfilter.input = DMX_IN_FRONTEND;
    if (Device::instance()->useDvrDevice()) {
        pesfilter.output = DMX_OUT_TS_TAP; // send output to dvr device
    }
    else {
        pesfilter.output = DMX_OUT_TAP; // send output to demux device
    }
    pesfilter.pes_type = pesType;
    pesfilter.flags = 0;

    if (ioctl(fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed");
        return false;
    }

    return true;
}


//unsigned int
//Demux::getPmtPid(unsigned int tid, int sid)
//{
//    LOG(dvb, trace, "get PMT PID for service id: " + Poco::NumberFormatter::format(sid) + " ...");
//
//    int pmt_pid = 0;
//    int count;
//    int section_length;
//    unsigned int transport_stream_id;
//    unsigned char buft[4096];
//    unsigned char *buf = buft;
//    struct dmx_sct_filter_params f;
//
//    memset(&f, 0, sizeof(f));
//    f.pid = 0;
//    f.filter.filter[0] = 0x00;
//    f.filter.mask[0] = 0xff;
//    f.timeout = 0;
//    // FIXME: don't set DMX_IMMEDIATE_START, use start() and stop()
//    f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;
//
//    // FIXME: pat filter set twice?
//    LOG(dvb, trace, "set demuxer filter ...");
//    if (ioctl(_fileDescPat, DMX_SET_FILTER, &f) == -1) {
//        LOG(dvb, error, "DMX_SET_FILTER failed");
//        return 0;
//    }
//    LOG(dvb, trace, "set demuxer filter finished.");
//
//    bool patread = false;
//    while (!patread) {
//        LOG(dvb, trace, "read PAT (pid 0) section into buffer ...");
//        if (((count = read(_fileDescPat, buf, sizeof(buft))) < 0) && errno == EOVERFLOW) {
//            LOG(dvb, trace, "read elementary stream pid 0 into buffer failed, second try ...");
//            count = read(_fileDescPat, buf, sizeof(buft));
//        }
//        if (count < 0) {
//            LOG(dvb, error, "while reading sections");
//            return 0;
//        }
//        LOG(dvb, trace, "read " + Poco::NumberFormatter::format(count) + " bytes from elementary stream pid 0 into buffer.");
//
//        section_length = ((buf[1] & 0x0f) << 8) | buf[2];
//        LOG(dvb, trace, "section length: " + Poco::NumberFormatter::format(section_length));
//
//        if (count != section_length + 3) {
//            LOG(dvb, error, "PAT size mismatch, next read attempt.");
//            continue;
//        }
//
//        transport_stream_id = (buf[3] << 8) | buf[4];
//        LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transport_stream_id));
//        if (transport_stream_id != tid) {
//            LOG(dvb, error, "PAT tid mismatch (" + Poco::NumberFormatter::format(tid) + "), next read attempt.");
//            continue;
//        }
//
//        buf += 8;
//        section_length -= 8;
//
//        patread = true;    // assumes one section contains the whole pat
//        while (section_length > 0) {
//            int service_id = (buf[0] << 8) | buf[1];
//            LOG(dvb, trace, "search for service id in section, found: " + Poco::NumberFormatter::format(service_id));
//            if (service_id == sid) {
//                pmt_pid = ((buf[2] & 0x1f) << 8) | buf[3];
//                section_length = 0;
//                LOG(dvb, trace, "found service id, pmt pid is: " + Poco::NumberFormatter::format(pmt_pid));
//            }
//            buf += 4;
//            section_length -= 4;
//        }
//    }
//    LOG(dvb, trace, "get PMT PID for service id finished.");
//
//    return pmt_pid;
//}

}  // namespace Omm
}  // namespace Dvb
