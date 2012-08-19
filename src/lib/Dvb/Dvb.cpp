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
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>

#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

#include "Dvb.h"
#include "Log.h"

using namespace Omm;
using namespace Omm::Dvb;

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


DvbChannel::DvbChannel(const std::string& name, unsigned int satNum, unsigned int freq, Polarization pol, unsigned int symbolRate, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid) :
_name(name),
_satNum(satNum),
_freq(freq),
_pol(pol),
_symbolRate(symbolRate),
_vpid(vpid),
_cpid(cpid),
_apid(apid),
_sid(sid),
_tid(tid)
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
        _channelMap[channelName[0]] = new Omm::Dvb::DvbChannel(channelName[0], 0, freq, pol, symbolRate, vpid, cpid, apid, sid, tid);
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
    _pDvr = new DvbDvr(this, 0);
}


DvbAdapter::~DvbAdapter()
{
    delete _pFrontend;
    delete _pDemux;
    delete _pDvr;
}


void
DvbAdapter::openAdapter()
{
    _pFrontend->openFrontend();
    _pDemux->openDemux();
    _pDvr->openDvr();
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
    close(_fileDesc);
}


void
DvbFrontend::openFrontend()
{
    LOG(dvb, debug, "opening frontend");

    if ((_fileDesc = open(_deviceName.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "opening frontend failed");
    }

    int result = ioctl(_fileDesc, FE_GET_INFO, &_feInfo);

    if (result < 0) {
        LOG(dvb, error, "ioctl FE_GET_INFO failed");
        close(_fileDesc);
    }

    if (_feInfo.type != FE_QPSK) {
        LOG(dvb, error, "frontend device is not a QPSK (DVB-S) device");
        close(_fileDesc);
    }
}


bool
DvbFrontend::tune(DvbChannel* pChannel)
{
    LOG(dvb, debug, "frontend tuning to channel");

    bool success = false;
    unsigned int ifreq;
    bool hiBand = _pAdapter->_pLnb->isHiBand(pChannel->_freq, ifreq);
    diseqc(pChannel->_satNum, pChannel->_pol, hiBand);

    if (tuneFrontend(ifreq, pChannel->_symbolRate)) {
        if (_pAdapter->_pDemux->setVideoPid(pChannel->_vpid) &&
            _pAdapter->_pDemux->setAudioPid(pChannel->_apid) &&
            _pAdapter->_pDemux->setPcrPid(pChannel->_cpid)) {
                success = true;
                if (_pAdapter->_recPsi) {
                    unsigned int pmtPid = _pAdapter->_pDemux->getPmtPid(pChannel->_tid, pChannel->_sid);
                    if (pmtPid == 0) {
                        LOG(dvb, error, "couldn't find pmt-pid for sid"); // %04x\n",sid);
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

    if (ioctl(_fileDesc, FE_SET_TONE, SEC_TONE_OFF) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }
    if (ioctl(_fileDesc, FE_SET_VOLTAGE, voltage) == -1) {
        LOG(dvb, error, "FE_SET_VOLTAGE failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDesc, FE_DISEQC_SEND_MASTER_CMD, &cmd.cmd) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_MASTER_CMD failed");
    }
    usleep(cmd.wait * 1000);
    usleep(15 * 1000);
    if (ioctl(_fileDesc, FE_DISEQC_SEND_BURST, burst) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_BURST failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDesc, FE_SET_TONE, tone) == -1) {
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
        if (ioctl(_fileDesc, FE_GET_EVENT, &event) == -1)
            break;
    }

    tuneto.frequency = freq;
    tuneto.inversion = INVERSION_AUTO;
    tuneto.u.qpsk.symbol_rate = symbolRate;
    tuneto.u.qpsk.fec_inner = FEC_AUTO;

    if (ioctl(_fileDesc, FE_SET_FRONTEND, &tuneto) == -1) {
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

    if (ioctl(_fileDesc, FE_READ_STATUS, &status) == -1) {
        LOG(dvb, error, "FE_READ_STATUS failed");
    }
    /* some frontends might not support all these ioctls, thus we
    * avoid printing errors */
    if (ioctl(_fileDesc, FE_READ_SIGNAL_STRENGTH, &signal) == -1) {
        signal = -2;
    }
    if (ioctl(_fileDesc, FE_READ_SNR, &snr) == -1) {
        snr = -2;
    }
    if (ioctl(_fileDesc, FE_READ_BER, &ber) == -1) {
        ber = -2;
    }
    if (ioctl(_fileDesc, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks) == -1) {
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
    close(_fileDescVideo);
    close(_fileDescAudio);
    close(_fileDescPcr);
    if (_pAdapter->_recPsi) {
        close(_fileDescPat);
        close(_fileDescPmt);
    }
}


void
DvbDemux::openDemux()
{
    LOG(dvb, debug, "opening demuxer");

    if ((_fileDescVideo = open(_deviceName.c_str(), O_RDWR)) < 0) {
        LOG(dvb, error, "opening video demux failed");
    }
    if ((_fileDescAudio = open(_deviceName.c_str(), O_RDWR)) < 0) {
        LOG(dvb, error, "opening audio demux failed");
    }
    if ((_fileDescPcr = open(_deviceName.c_str(), O_RDWR)) < 0) {
        LOG(dvb, error, "opening pcr demux failed");
    }

    if (_pAdapter->_recPsi){
        if ((_fileDescPat = open(_deviceName.c_str(), O_RDWR)) < 0) {
            LOG(dvb, error, "opening pat demux failed");
        }
        if ((_fileDescPmt = open(_deviceName.c_str(), O_RDWR)) < 0) {
            LOG(dvb, error, "opening pmt demux failed");
        }
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


bool
DvbDemux::setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType)
{
    if (pid >= 0x1fff) {  // ignore this pid to allow radio services
        return true;
    }

    int buffersize = 64 * 1024;
    if (ioctl(fileDesc, DMX_SET_BUFFER_SIZE, buffersize) == -1) {
        LOG(dvb, error, "DMX_SET_BUFFER_SIZE failed");
    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pid;
    pesfilter.input = DMX_IN_FRONTEND;
//     pesfilter.output = dvr ? DMX_OUT_TS_TAP : DMX_OUT_DECODER;
    pesfilter.output = DMX_OUT_TS_TAP;
    pesfilter.pes_type = pesType;
    pesfilter.flags = DMX_IMMEDIATE_START;

    if (ioctl(fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
//         fprintf(stderr, "DMX_SET_PES_FILTER failed "
//                 "(PID = 0x%04x): %d %m\n", pid, errno);
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
    f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

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


class UnixFileStreamBuf : public Poco::BufferedStreamBuf
{
public:
    UnixFileStreamBuf(int fileDesc) : BasicBufferedStreamBuf(1000, std::ios_base::in), _fileDesc(fileDesc) {}

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
    UnixFileIStream(int fileDesc) : _streamBuf(fileDesc), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    UnixFileStreamBuf   _streamBuf;
};


DvbDvr::DvbDvr(DvbAdapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num),
_pDvrStream(0)
{
    _deviceName = _pAdapter->_deviceName + "/dvr" + Poco::NumberFormatter::format(_num);
}


DvbDvr::~DvbDvr()
{
}


void
DvbDvr::openDvr()
{
    LOG(dvb, debug, "opening dvb rec device.");
//    if ((_fileDescDvr = open(_deviceName.c_str(), O_RDONLY | O_NONBLOCK)) < 0) {
    if ((_fileDescDvr = open(_deviceName.c_str(), O_RDONLY)) < 0) {
        LOG(dvb, error, "failed to open dvb rec device: " + _deviceName);
        return;
    }
    _pDvrStream = new UnixFileIStream(_fileDescDvr);
    if (!_pDvrStream) {
        LOG(dvb, error, "failed to open dvb rec stream.");
    }
}


std::istream*
DvbDvr::getStream()
{
    LOG(dvb, debug, "get dvb rec stream.");

    if (!_pDvrStream) {
        openDvr();
    }
    return _pDvrStream;
}


void
DvbDvr::freeStream()
{
    LOG(dvb, debug, "free dvb rec stream.");

    if (_pDvrStream) {
        delete _pDvrStream;
        _pDvrStream = 0;
        close(_fileDescDvr);
    }
}


DvbDevice* DvbDevice::_pInstance = 0;

DvbDevice::DvbDevice()
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
    return _adapters[0]->getDvr()->getStream();
}


void
DvbDevice::freeStream(std::istream* pIstream)
{
    _adapters[0]->getDvr()->freeStream();
}
