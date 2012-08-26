/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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
#include <Poco/File.h>
#include <Poco/BufferedStreamBuf.h>
//#include <Poco/UnbufferedStreamBuf.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>

#include <fstream>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

#include "Dvb.h"
#include "Log.h"


namespace Omm {
namespace Dvb {

#ifndef NDEBUG
Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::Channel* pChannel = Util::Log::instance()->channel();
//    _pDvbLogger = &Poco::Logger::create("DVB", pChannel, Poco::Message::PRIO_DEBUG);
    _pDvbLogger = &Poco::Logger::create("DVB", pChannel, Poco::Message::PRIO_TRACE);
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::dvb()
{
    return *_pDvbLogger;
}
#endif // NDEBUG


class UnixFileStreamBuf : public Poco::BufferedStreamBuf
//class UnixFileStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    UnixFileStreamBuf(int fileDesc, int bufSize) : BasicBufferedStreamBuf(bufSize, std::ios_base::in), _fileDesc(fileDesc) {}
//    UnixFileStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        return read(_fileDesc, buffer, length);
    }

private:
    int         _fileDesc;
};


class UnixFileIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    UnixFileIStream(int fileDesc, int bufSize = 2048) : _streamBuf(fileDesc, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    UnixFileStreamBuf   _streamBuf;
};


class ByteQueueStreamBuf : public Poco::BufferedStreamBuf
//class ByteQueueStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    ByteQueueStreamBuf(AvStream::ByteQueue& byteQueue) : BasicBufferedStreamBuf(byteQueue.size(), std::ios_base::in), _byteQueue(byteQueue) {}
//    ByteQueueStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        _byteQueue.read(buffer, length);
        return length;
    }

private:
    AvStream::ByteQueue&         _byteQueue;
};


class ByteQueueIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    ByteQueueIStream(AvStream::ByteQueue& byteQueue) : _streamBuf(byteQueue), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    ByteQueueStreamBuf   _streamBuf;
};


DvbChannel::DvbChannel(const std::string& name, unsigned int satNum, unsigned int freq, Polarization pol, unsigned int symbolRate, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid) :
_name(name),
_satNum(satNum),
_freq(freq),
_pol(pol),
_symbolRate(symbolRate),
_vpid(vpid),
_cpid(cpid),
_apid(apid),
_sid(sid),
_tid(tid),
_pmtid(pmtid)
{
}


std::string
DvbChannel::getName()
{
    return _name;
}


DvbChannels::ChannelIterator
DvbChannels::beginChannel()
{
    return _channelMap.begin();
}


DvbChannels::ChannelIterator
DvbChannels::endChannel()
{
    return _channelMap.end();
}


DvbChannel*
DvbChannels::getChannel(const std::string& name)
{
    return _channelMap[name];
}


void
ChannelsConf::setConfFilePath(const std::string& confFilePath)
{
    _confFilePath = confFilePath;
}


void
ChannelsConf::scanChannels()
{
    scanChannelConfig(_confFilePath);
}


void
ChannelsConf::scanChannelConfig(const std::string& channelConfig)
{
    LOG(dvb, debug, "scan channel config ...");

    clearChannels();

    std::ifstream channels(channelConfig.c_str());
    std::string line;
    while (getline(channels, line)) {
        Poco::StringTokenizer channelParams(line, ":");
        Poco::StringTokenizer channelName(channelParams[0], ";");
        unsigned int freq = Poco::NumberParser::parseUnsigned(channelParams[1]) * 1000;
        DvbChannel::Polarization pol = (channelParams[2][0] == 'h') ? DvbChannel::HORIZ : DvbChannel::VERT;
        unsigned int symbolRate = Poco::NumberParser::parseUnsigned(channelParams[4]) * 1000;
        Poco::StringTokenizer videoPid(channelParams[5], "+");
        unsigned int vpid = Poco::NumberParser::parseUnsigned(videoPid[0]);
        unsigned int cpid = vpid;
        if (videoPid.count() > 1) {
            cpid = Poco::NumberParser::parseUnsigned(videoPid[1]);
        }
        Poco::StringTokenizer audioChannel(channelParams[6], ";");
        Poco::StringTokenizer audioPid(audioChannel[0], "=");
        unsigned int apid = Poco::NumberParser::parseUnsigned(audioPid[0]);
        int sid = Poco::NumberParser::parseUnsigned(channelParams[9]);
        unsigned int tid = Poco::NumberParser::parseUnsigned(channelParams[11]);
        unsigned int pmtid = 0;
        if (channelParams.count() > 13) {
            pmtid = Poco::NumberParser::parseUnsigned(channelParams[13]);
        }
        _channelMap[channelName[0]] = new Omm::Dvb::DvbChannel(channelName[0], 0, freq, pol, symbolRate, vpid, cpid, apid, sid, tid, pmtid);
    }

    LOG(dvb, debug, "scan channel config finished.");
}


void
ChannelsConf::clearChannels()
{
    for (std::map<std::string, Omm::Dvb::DvbChannel*>::iterator it = _channelMap.begin(); it != _channelMap.end(); ++it) {
        delete it->second;
    }
    _channelMap.clear();
}


DvbAdapter::DvbAdapter(int num)
{
    _deviceName = "/dev/dvb/adapter" + Poco::NumberFormatter::format(num);
    _pLnb = DvbDevice::instance()->_lnbs["UNIVERSAL"];
    _recPsi = true;

    _pFrontend = new DvbFrontend(this, 0);
    _pDemux = new DvbDemux(this, 0);
    if (DvbDevice::instance()->useDvrDevice()) {
        _pDvr = new DvbDvr(this, 0);
    }
}


DvbAdapter::~DvbAdapter()
{
    delete _pFrontend;
    delete _pDemux;
    if (DvbDevice::instance()->useDvrDevice()) {
        delete _pDvr;
    }
}


void
DvbAdapter::openAdapter()
{
    _pFrontend->openFrontend();
    _pDemux->openDemux(false);
    if (DvbDevice::instance()->useDvrDevice()) {
        _pDvr->openDvr(DvbDevice::instance()->blockDvrDevice());
    }
}


void
DvbAdapter::closeAdapter()
{
    if (DvbDevice::instance()->useDvrDevice()) {
        _pDvr->closeDvr();
    }
    _pDemux->closeDemux();
    _pFrontend->closeFrontend();
}


DvbDemux*
DvbAdapter::getDemux()
{
    return _pDemux;
}


DvbDvr*
DvbAdapter::getDvr()
{
    return _pDvr;
}


DvbLnb::DvbLnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal) :
_desc(desc),
_lowVal(lowVal),
_highVal(highVal),
_switchVal(switchVal)
{
}


bool
DvbLnb::isHiBand(unsigned int freq, unsigned int& ifreq)
{
    bool hiBand = false;

    if (_switchVal && _highVal && freq >= _switchVal) {
        hiBand = true;
    }

    if (hiBand)
        ifreq = freq - _highVal;
    else {
        if (freq < _lowVal)
            ifreq = _lowVal - freq;
        else
            ifreq = freq - _lowVal;
    }

    return hiBand;
}


class SignalCheckThread : public Poco::Runnable
{
public:
    SignalCheckThread(DvbFrontend* pFrontend);

    void run();
    void stop();

private:
    DvbFrontend*        _pFrontend;
    bool                _stop;
};


SignalCheckThread::SignalCheckThread(DvbFrontend* pFrontend) :
_pFrontend(pFrontend),
_stop(false)
{
}


void
SignalCheckThread::run()
{
//     Poco::ScopedLock<Poco::FastMutex> lock(DvbDevice::instance()->_tuneLock);
    do {
        _pFrontend->checkFrontend();
        Poco::Thread::sleep(1000);
    } while(!_stop);
}


void
SignalCheckThread::stop()
{
    _stop = true;
}


DvbFrontend::DvbFrontend(DvbAdapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num)
{
    _deviceName = _pAdapter->_deviceName + "/frontend" + Poco::NumberFormatter::format(_num);
}


DvbFrontend::~DvbFrontend()
{
    closeFrontend();
}


void
DvbFrontend::openFrontend()
{
    LOG(dvb, debug, "opening frontend");

    if ((_fileDescFrontend = open(_deviceName.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "opening frontend failed: " + std::string(strerror(errno)));
    }

    int result = ioctl(_fileDescFrontend, FE_GET_INFO, &_feInfo);

    if (result < 0) {
        LOG(dvb, error, "ioctl FE_GET_INFO failed");
        closeFrontend();
    }

    if (!typeSupported()) {
        LOG(dvb, error, "frontend device is not a DVB-S or DVB-T device, not yet supported");
        closeFrontend();
    }
}


void
DvbFrontend::closeFrontend()
{
    LOG(dvb, debug, "closing frontend");

    if (close(_fileDescFrontend)) {
        LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
    }
}


DvbFrontend::FrontendType
DvbFrontend::getType()
{
    LOG(dvb, debug, "frontend type: " + Poco::NumberFormatter::format(_feInfo.type));

    switch (_feInfo.type) {
        case FE_QPSK:
            return DVBS;
        case FE_OFDM:
            return DVBT;
        case FE_QAM:
            return DVBC;
        case FE_ATSC:
            return ATSC;
        default:
            return None;
    }
}


bool
DvbFrontend::typeSupported()
{
    return getType() == DVBS || getType() == DVBT;
}


bool
DvbFrontend::tune(DvbChannel* pChannel)
{
    LOG(dvb, debug, "frontend tuning to channel");

    bool success = false;
    unsigned int ifreq;
    if (getType() == DVBS) {
        bool hiBand = _pAdapter->_pLnb->isHiBand(pChannel->_freq, ifreq);
        diseqc(pChannel->_satNum, pChannel->_pol, hiBand);
    }
    else if (getType() == DVBT) {
        ifreq = pChannel->_freq;
    }
    else {
        LOG(dvb, error, "frontend type not yet supported, stop tuning.");
        return false;
    }

    if (tuneFrontend(ifreq, pChannel->_symbolRate)) {
        if (_pAdapter->_pDemux->setVideoPid(pChannel->_vpid) &&
            _pAdapter->_pDemux->setAudioPid(pChannel->_apid) &&
            _pAdapter->_pDemux->setPcrPid(pChannel->_cpid)) {
                success = true;
                if (_pAdapter->_recPsi) {
                    unsigned int pmtPid = pChannel->_pmtid;
                    if (pmtPid == 0) {
                        pmtPid = _pAdapter->_pDemux->getPmtPid(pChannel->_tid, pChannel->_sid);
                    }
                    LOG(dvb, debug, "pmt pid: " + Poco::NumberFormatter::format(pmtPid));
                    if (pmtPid == 0) {
                        LOG(dvb, error, "couldn't find pmt-pid for sid");
                        success = false;
                    }
                    else if (!_pAdapter->_pDemux->setPatPid(0) ||
                             !_pAdapter->_pDemux->setPmtPid(pmtPid))
                    {
                        success = false;
                    }
                }
        }
    }

    LOG(dvb, debug, "frontend tuning " + std::string(success ? "success." : "failed."));
    return success;
}


void
DvbFrontend::stopTune()
{
    _pt->stop();
    delete _pt;
}


void
DvbFrontend::diseqc(unsigned int satNum, DvbChannel::Polarization pol, bool hiBand)
{
    LOG(dvb, debug, "diseqc command on sat: " + Poco::NumberFormatter::format(satNum) + " ...");

    struct diseqc_cmd {
        struct dvb_diseqc_master_cmd cmd;
        uint32_t wait;
    } cmd = { {{0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00}, 4}, 0 };

    // param: high nibble: reset bits, low nibble set bits,
    // bits are: option, position, polarization, band

    cmd.cmd.msg[3] =
        0xf0 | (((satNum * 4) & 0x0f) | (hiBand ? 1 : 0) | (pol ? 0 : 2));

    fe_sec_voltage_t voltage = pol ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18;
    fe_sec_tone_mode_t tone = hiBand ? SEC_TONE_ON : SEC_TONE_OFF;
    fe_sec_mini_cmd_t burst = satNum % 2 ? SEC_MINI_B : SEC_MINI_A;

    if (ioctl(_fileDescFrontend, FE_SET_TONE, SEC_TONE_OFF) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }
    if (ioctl(_fileDescFrontend, FE_SET_VOLTAGE, voltage) == -1) {
        LOG(dvb, error, "FE_SET_VOLTAGE failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_MASTER_CMD, &cmd.cmd) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_MASTER_CMD failed");
    }
    usleep(cmd.wait * 1000);
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_BURST, burst) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_BURST failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_SET_TONE, tone) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }

    LOG(dvb, debug, "diseqc command finished.");
}


bool
DvbFrontend::tuneFrontend(unsigned int freq, unsigned int symbolRate)
{
    LOG(dvb, debug, "tune frontend to frequency: " + Poco::NumberFormatter::format(freq) + " ...");

    struct dvb_frontend_parameters tuneto;
    struct dvb_frontend_event event;

    // discard stale QPSK events
    while (1) {
        if (ioctl(_fileDescFrontend, FE_GET_EVENT, &event) == -1)
            break;
    }

    tuneto.frequency = freq;
    tuneto.inversion = INVERSION_AUTO;
    if (getType() == DVBS) {
        tuneto.u.qpsk.symbol_rate = symbolRate;
        tuneto.u.qpsk.fec_inner = FEC_AUTO;
    }
    else if (getType() == DVBT) {
        tuneto.u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
        tuneto.u.ofdm.code_rate_HP = FEC_2_3;
        tuneto.u.ofdm.code_rate_LP = FEC_NONE;
        tuneto.u.ofdm.constellation = QAM_16;
        tuneto.u.ofdm.transmission_mode = TRANSMISSION_MODE_8K;
        tuneto.u.ofdm.guard_interval = GUARD_INTERVAL_1_4;
        tuneto.u.ofdm.hierarchy_information = HIERARCHY_NONE;
    }

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, error, "FE_SET_FRONTEND failed");
        return false;
    }

    LOG(dvb, debug, "tune frontend to frequency finished.");

    return true;
}


void
DvbFrontend::checkFrontend()
{
    fe_status_t status;
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;
//     int timeout = 0;

    if (ioctl(_fileDescFrontend, FE_READ_STATUS, &status) == -1) {
        LOG(dvb, error, "FE_READ_STATUS failed");
    }
    /* some frontends might not support all these ioctls, thus we
    * avoid printing errors */
    if (ioctl(_fileDescFrontend, FE_READ_SIGNAL_STRENGTH, &signal) == -1) {
        signal = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_SNR, &snr) == -1) {
        snr = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_BER, &ber) == -1) {
        ber = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks) == -1) {
        uncorrected_blocks = -2;
    }

    /*
    if (human_readable) {
        printf ("status %02x | signal %3u%% | snr %3u%% | ber %d | unc %d | ",
                status, (signal * 100) / 0xffff, (snr * 100) / 0xffff, ber, uncorrected_blocks);
    } else {
        printf ("status %02x | signal %04x | snr %04x | ber %08x | unc %08x | ",
                status, signal, snr, ber, uncorrected_blocks);
    }
    */

    if (status & FE_HAS_LOCK) {
        LOG(dvb, debug, "FE_HAS_LOCK");
    }
}


DvbDemux::DvbDemux(DvbAdapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num)
{
    _deviceName = _pAdapter->_deviceName + "/demux" + Poco::NumberFormatter::format(_num);
}


DvbDemux::~DvbDemux()
{
    closeDemux();
}


void
DvbDemux::openDemux(bool blocking)
{
    LOG(dvb, debug, "opening demuxer");

    int flag = blocking ? O_RDWR : O_RDWR | O_NONBLOCK;

    if ((_fileDescVideo = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "opening video elementary stream failed: " + std::string(strerror(errno)));
    }
    if ((_fileDescAudio = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "opening audio elementary stream failed: " + std::string(strerror(errno)));
    }
    if ((_fileDescPcr = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "opening pcr elementary stream failed: " + std::string(strerror(errno)));
    }
    if (_pAdapter->_recPsi){
        if ((_fileDescPat = open(_deviceName.c_str(), flag)) < 0) {
            LOG(dvb, error, "opening pat elementary stream failed: " + std::string(strerror(errno)));
        }
        if ((_fileDescPmt = open(_deviceName.c_str(), flag)) < 0) {
            LOG(dvb, error, "opening pmt elementary stream failed: " + std::string(strerror(errno)));
        }
    }
}


void
DvbDemux::closeDemux()
{
    LOG(dvb, debug, "closing demuxer");


    if (close(_fileDescVideo)) {
        LOG(dvb, error, "failed to close video elementary stream: " + std::string(strerror(errno)));
    }
    if (close(_fileDescAudio)) {
        LOG(dvb, error, "failed to close audio elementary stream: " + std::string(strerror(errno)));
    }
    if (close(_fileDescPcr)) {
        LOG(dvb, error, "failed to close pcr elementary stream: " + std::string(strerror(errno)));
    }
    if (_pAdapter->_recPsi) {
        if (close(_fileDescPat)) {
            LOG(dvb, error, "failed to close pat elementary stream: " + std::string(strerror(errno)));
        }
        if (close(_fileDescPmt)) {
            LOG(dvb, error, "failed to close pmt elementary stream: " + std::string(strerror(errno)));
        }
    }
}


void
DvbDemux::start()
{
    LOG(dvb, debug, "start demuxer");

    if (ioctl(_fileDescVideo, DMX_START) == -1) {
        LOG(dvb, error, "starting video dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescAudio, DMX_START) == -1) {
        LOG(dvb, error, "starting audio dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPcr, DMX_START) == -1) {
        LOG(dvb, error, "starting pcr dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPat, DMX_START) == -1) {
        LOG(dvb, error, "starting pat dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPmt, DMX_START) == -1) {
        LOG(dvb, error, "starting pmt dmx filter failed: " + std::string(strerror(errno)));
    }
}


void
DvbDemux::stop()
{
    LOG(dvb, debug, "stop demuxer");

    if (ioctl(_fileDescVideo, DMX_STOP) == -1) {
        LOG(dvb, error, "stopping video dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescAudio, DMX_STOP) == -1) {
        LOG(dvb, error, "stopping audio dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPcr, DMX_STOP) == -1) {
        LOG(dvb, error, "stopping pcr dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPat, DMX_STOP) == -1) {
        LOG(dvb, error, "stopping pat dmx filter failed: " + std::string(strerror(errno)));
    }
    if (ioctl(_fileDescPmt, DMX_STOP) == -1) {
        LOG(dvb, error, "stopping pmt dmx filter failed: " + std::string(strerror(errno)));
    }
}


bool
DvbDemux::setVideoPid(unsigned int pid)
{
    return setPid(_fileDescVideo, pid, DMX_PES_VIDEO);
}


bool
DvbDemux::setAudioPid(unsigned int pid)
{
    return setPid(_fileDescAudio, pid, DMX_PES_AUDIO);
}


bool
DvbDemux::setPcrPid(unsigned int pid)
{
    return setPid(_fileDescPcr, pid, DMX_PES_PCR);
}


bool
DvbDemux::setPatPid(unsigned int pid)
{
    if (_pAdapter->_recPsi) {
        return setPid(_fileDescPat, pid, DMX_PES_OTHER);
    }
    return false;
}


bool
DvbDemux::setPmtPid(unsigned int pid)
{
    if (_pAdapter->_recPsi) {
        return setPid(_fileDescPmt, pid, DMX_PES_OTHER);
    }
    return false;
}


std::istream*
DvbDemux::getVideoStream()
{
    return new UnixFileIStream(_fileDescVideo);
}


std::istream*
DvbDemux::getAudioStream()
{
    return new UnixFileIStream(_fileDescAudio);
}


bool
DvbDemux::setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType)
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
    if (DvbDevice::instance()->useDvrDevice()) {
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


unsigned int
DvbDemux::getPmtPid(unsigned int tid, int sid)
{
    LOG(dvb, trace, "get PMT PID for service id: " + Poco::NumberFormatter::format(sid) + " ...");

    int pmt_pid = 0;
    int count;
    int section_length;
    unsigned int transport_stream_id;
    unsigned char buft[4096];
    unsigned char *buf = buft;
    struct dmx_sct_filter_params f;

    memset(&f, 0, sizeof(f));
    f.pid = 0;
    f.filter.filter[0] = 0x00;
    f.filter.mask[0] = 0xff;
    f.timeout = 0;
    // FIXME: don't set DMX_IMMEDIATE_START, use start() and stop()
    f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

    // FIXME: pat filter set twice?
    LOG(dvb, trace, "set demuxer filter ...");
    if (ioctl(_fileDescPat, DMX_SET_FILTER, &f) == -1) {
        LOG(dvb, error, "DMX_SET_FILTER failed");
        return 0;
    }
    LOG(dvb, trace, "set demuxer filter finished.");

    bool patread = false;
    while (!patread) {
        LOG(dvb, trace, "read PAT (pid 0) section into buffer ...");
        if (((count = read(_fileDescPat, buf, sizeof(buft))) < 0) && errno == EOVERFLOW) {
            LOG(dvb, trace, "read elementary stream pid 0 into buffer failed, second try ...");
            count = read(_fileDescPat, buf, sizeof(buft));
        }
        if (count < 0) {
            LOG(dvb, error, "while reading sections");
            return 0;
        }
        LOG(dvb, trace, "read " + Poco::NumberFormatter::format(count) + " bytes from elementary stream pid 0 into buffer.");

        section_length = ((buf[1] & 0x0f) << 8) | buf[2];
        LOG(dvb, trace, "section length: " + Poco::NumberFormatter::format(section_length));

        if (count != section_length + 3) {
            LOG(dvb, error, "PAT size mismatch, next read attempt.");
            continue;
        }

        transport_stream_id = (buf[3] << 8) | buf[4];
        LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transport_stream_id));
        if (transport_stream_id != tid) {
            LOG(dvb, error, "PAT tid mismatch (" + Poco::NumberFormatter::format(tid) + "), next read attempt.");
            continue;
        }

        buf += 8;
        section_length -= 8;

        patread = true;    // assumes one section contains the whole pat
        while (section_length > 0) {
            int service_id = (buf[0] << 8) | buf[1];
            LOG(dvb, trace, "search for service id in section, found: " + Poco::NumberFormatter::format(service_id));
            if (service_id == sid) {
                pmt_pid = ((buf[2] & 0x1f) << 8) | buf[3];
                section_length = 0;
                LOG(dvb, trace, "found service id, pmt pid is: " + Poco::NumberFormatter::format(pmt_pid));
            }
            buf += 4;
            section_length -= 4;
        }
    }
    LOG(dvb, trace, "get PMT PID for service id finished.");

    return pmt_pid;
}


DvbDvr::DvbDvr(DvbAdapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num),
_pDvrStream(0),
_useByteQueue(true),
//_useByteQueue(false),
_fileDescDvr(-1),
_byteQueue(100*1024),
_bufferSize(50*1024),
_pollTimeout(1),
_pReadThread(0),
_readThreadRunnable(*this, &DvbDvr::readThread),
_readThreadRunning(false)
{
    _deviceName = _pAdapter->_deviceName + "/dvr" + Poco::NumberFormatter::format(_num);
}


DvbDvr::~DvbDvr()
{
}


void
DvbDvr::openDvr(bool blocking)
{
    LOG(dvb, debug, "opening dvb rec device.");

    if (_pDvrStream) {
        LOG(dvb, debug, "dvb rec device already open.");
        return;
    }
    else {
        int flags = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if ((_fileDescDvr = open(_deviceName.c_str(), flags)) < 0) {
            LOG(dvb, error, "failed to open dvb rec device \"" + _deviceName + "\": " + strerror(errno));
            return;
        }
        if (_useByteQueue) {
            _fileDescPoll[0].fd = _fileDescDvr;
            _fileDescPoll[0].events = POLLIN;
            _pDvrStream = new ByteQueueIStream(_byteQueue);
        }
        else {
            _pDvrStream = new UnixFileIStream(_fileDescDvr, _bufferSize);
        }
        if (!_pDvrStream) {
            LOG(dvb, error, "failed to open dvb rec stream.");
            return;
        }
//        if (_useByteQueue) {
//            startReadThread();
//        }
    }
}


void
DvbDvr::closeDvr()
{
    LOG(dvb, debug, "closing dvb rec device.");

//    stopReadThread();
    if (_pDvrStream) {
        delete _pDvrStream;
        _pDvrStream = 0;
        if (close(_fileDescDvr)) {
            LOG(dvb, error, "failed to close dvb rec device \"" + _deviceName + "\": " + strerror(errno));
        }
        _fileDescDvr = -1;
    }
    else {
        LOG(dvb, debug, "dvb rec device already closed.");
    }
}


void
DvbDvr::clearBuffer()
{
    if (_pDvrStream) {
        const int bufsize(_bufferSize);
        char buf[bufsize];
        while (int bytes = _pDvrStream->readsome(buf, bufsize)) {
            LOG(dvb, debug, "clear buffer: " + Poco::NumberFormatter::format(bytes) + " bytes");
        }
    }
}


void
DvbDvr::prefillBuffer()
{
    if (_pReadThread) {
        while (_byteQueue.level() < _byteQueue.size() * 0.5) {
            Poco::Thread::sleep(1);
        }
    }
}


void
DvbDvr::setBlocking(bool blocking)
{
    LOG(dvb, debug, "set dvb rec device to " + std::string(blocking ? "blocking" : "non-blocking"));

    if (_pDvrStream) {
        long fcntlFlag = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if (fcntl(_fileDescDvr, F_SETFL, fcntlFlag) < 0) {
            LOG(dvb, error, "failed to set dvb rec device \"" + _deviceName + "\" to " + (blocking ? "blocking" : "non-blocking") + ":" + strerror(errno));
        }
    }
}


void
DvbDvr::startReadThread()
{
    LOG(dvb, debug, "start read thread ...");

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
DvbDvr::stopReadThread()
{
    LOG(dvb, debug, "stop read thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        if (!_pReadThread->tryJoin(_pollTimeout)) {
            LOG(dvb, error, "failed to join read thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }

    LOG(dvb, debug, "read thread stopped.");
}


bool
DvbDvr::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


std::istream*
DvbDvr::getStream()
{
    LOG(dvb, debug, "get dvb rec stream.");

    return _pDvrStream;
}


void
DvbDvr::readThread()
{
    LOG(dvb, debug, "read thread started.");

    char buf[_bufferSize];
    while (readThreadRunning()) {
        if (poll(_fileDescPoll, 1, _pollTimeout)) {
            if (_fileDescPoll[0].revents & POLLIN){
                int len = read(_fileDescDvr, buf, _bufferSize);
                if (len > 0) {
                    _byteQueue.write(buf, len);
                }
                else if (len < 0) {
                    LOG(dvb, error, "dvr read thread failed to read from device");
                    break;
                }
            }
        }
    }

    LOG(dvb, debug, "read thread finished.");
}


DvbDevice* DvbDevice::_pInstance = 0;

DvbDevice::DvbDevice() :
_useDvrDevice(true),
_blockDvrDevice(false),
//_blockDvrDevice(true),
// _blockDvrDevice = true then reopen device fails (see _reopenDvrDevice), _blockDvrDevice = false then stream has zero length
//_reopenDvrDevice(true)
_reopenDvrDevice(false)
// renderer in same process as dvb server: reopenDvrDevice = true then dvr device busy, reopenDvrDevice = false then stream sometimes broken
// renderer in different process as dvb server: reopenDvrDevice = true ok, reopenDvrDevice = false then stream sometimes broken
{
    // FIXME: when do we need to multiply freqs with 1000?
    _lnbs["UNIVERSAL"] = new DvbLnb("Europe | 10800 to 11800 MHz and 11600 to 12700 Mhz | Dual LO, loband 9750, hiband 10600 MHz",
                                    9750000, 10600000, 11700000);
    _lnbs["DBS"] = new DvbLnb("Expressvu, North America | 12200 to 12700 MHz | Single LO, 11250 MHz",
                              11250, 0, 0);
    _lnbs["STANDARD"] = new DvbLnb("10945 to 11450 Mhz | Single LO, 10000 Mhz",
                                   10000, 0, 0);
    _lnbs["ENHANCED"] = new DvbLnb("Astra | 10700 to 11700 MHz | Single LO, 9750 MHz",
                                   9750, 0, 0);
    _lnbs["C-BAND"] = new DvbLnb("Big Dish | 3700 to 4200 MHz | Single LO, 5150 Mhz",
                              5150, 0, 0);
}


DvbDevice::~DvbDevice()
{
    for(std::vector<DvbAdapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        delete *it;
    }
}


DvbDevice*
DvbDevice::instance()
{
    if (!_pInstance) {
        _pInstance = new DvbDevice;
    }
    return _pInstance;
}


void
DvbDevice::addAdapter(DvbAdapter* pAdapter)
{
    pAdapter->openAdapter();
    _adapters.push_back(pAdapter);
}


bool
DvbDevice::tune(DvbChannel* pChannel)
{
    LOG(dvb, debug, "start tuning ...");

    if (_adapters.size() == 0) {
        LOG(dvb, error, "no adapter found, tuning aborted.");
        return false;
    }
    if (!_adapters[0]->_pFrontend) {
        LOG(dvb, error, "no frontend found, tuning aborted.");
        return false;
    }
    return _adapters[0]->_pFrontend->tune(pChannel);
}


void
DvbDevice::stopTune()
{
    _adapters[0]->_pFrontend->stopTune();
    LOG(dvb, debug, "stopped tuning.");
}


std::istream*
DvbDevice::getStream()
{
    if (_useDvrDevice && _reopenDvrDevice) {
        _adapters[0]->getDvr()->openDvr(_blockDvrDevice);
    }
    _adapters[0]->getDemux()->start();
    if (_useDvrDevice) {
        _adapters[0]->getDvr()->startReadThread();
        _adapters[0]->getDvr()->prefillBuffer();
        return _adapters[0]->getDvr()->getStream();
    }
    else {
        return _adapters[0]->getDemux()->getAudioStream();
//        return _adapters[0]->getDemux()->getVideoStream();
    }
}


void
DvbDevice::freeStream(std::istream* pIstream)
{
    _adapters[0]->getDemux()->stop();
    if (_useDvrDevice) {
        _adapters[0]->getDvr()->stopReadThread();
        _adapters[0]->getDvr()->clearBuffer();
        if (_reopenDvrDevice) {
//            _adapters[0]->getDvr()->setBlocking(false);
            _adapters[0]->getDvr()->closeDvr();
        }
    }
}


bool
DvbDevice::useDvrDevice()
{
    return _useDvrDevice;
}


bool
DvbDevice::blockDvrDevice()
{
    return _blockDvrDevice;
}


}  // namespace Omm
}  // namespace Dvb
