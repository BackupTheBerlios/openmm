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

#ifndef Dvb_INCLUDED
#define Dvb_INCLUDED

#include <Poco/Timestamp.h>
#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Mutex.h>
#include <Poco/Logger.h>
#include <Poco/Format.h>
#include <Poco/ByteOrder.h>
#include <Poco/StringTokenizer.h>
#include <Poco/TextConverter.h>
#include <Poco/TextEncoding.h>
#include <Poco/UTF8Encoding.h>
#include <Poco/DOM/Document.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <sys/poll.h>

#include "AvStream.h"

namespace Omm {
namespace Dvb {

class Stream;
class Device;
class Frontend;
class Transponder;
class SignalCheckThread;

#ifndef NDEBUG
class Log
{
public:
    static Log* instance();

    Poco::Logger& dvb();

private:
    Log();

    static Log*     _pInstance;
    Poco::Logger*   _pDvbLogger;
};
#endif // NDEBUG


//class Channel
//{
//    friend class Device;
//    friend class Frontend;
//    friend class SignalCheckThread;
//
//public:
//    typedef enum {HORIZ = 0, VERT = 1} Polarization;
//
//    Channel(const std::string& name, unsigned int _satNum, unsigned int freq, Polarization pol, unsigned int _symbolRate, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid);
//
//    std::string getName();
//
//private:
//    std::string         _name;
//    unsigned int        _satNum;
//    unsigned int        _freq;
//    Polarization        _pol;
//    unsigned int        _symbolRate;
//    unsigned int        _vpid;
//    unsigned int        _cpid;
//    unsigned int        _apid;
//    int                 _sid;
//    unsigned int        _tid;
//    unsigned int        _pmtid;
//};
//
//
//class Channels
//{
//public:
//    typedef std::map<std::string, Channel*>::iterator ChannelIterator;
//    ChannelIterator beginChannel();
//    ChannelIterator endChannel();
//
//    virtual void scanChannels() {}
//
//    Channel* getChannel(const std::string& name);
//
//protected:
//    std::map<std::string, Channel*> _channelMap;
//};
//
//
//class ChannelsConf : public Channels
//{
//public:
//    virtual void scanChannels();
//
//    void setConfFilePath(const std::string& confFilePath);
//
//private:
//    void scanChannelConfig(const std::string& channelConfig);
//    void clearChannels();
//
//    std::string _confFilePath;
//};




class Section
{
public:
    Section(Poco::UInt8 tableId);
    ~Section();

    void read(Stream* pStream);

    template<typename T>
    T getValue(unsigned int offset, unsigned int length)
    /// getValue(offset, length) cuts out bits at offset and length in bits and interprets value as of type T
    /// no boundary checks, so you have to know what you do
    {
        unsigned int byteOffsetLeft = offset >> 3;

        if (length <= 8) {
            Poco::UInt8 buf = *((Poco::UInt8*)(_data) + byteOffsetLeft);
            unsigned int bitOffsetLeft = offset % 8;
            if (bitOffsetLeft) {
                Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
                buf &= leftMask;
            }
            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
                Poco::UInt8 rightMask = 0xff << bitOffsetRight;
                buf &= rightMask;
                buf >>= bitOffsetRight;
            }
            return (T)buf;
        }
        else {
            Poco::UInt8 buf[sizeof(T)];
            memcpy(buf, (Poco::UInt8*)(_data) + byteOffsetLeft, sizeof(T));

            unsigned int bitOffsetLeft = offset % 8;
            if (bitOffsetLeft) {
                Poco::UInt8 leftMask = 0xff >> bitOffsetLeft;
                buf[0] &= leftMask;
            }

            unsigned int bitOffsetRight = (offset + length) % 8;
            if (bitOffsetRight) {
                bitOffsetRight = 8 - bitOffsetRight;
                Poco::UInt8 rightMask = 0xff << bitOffsetRight;
                buf[sizeof(T) - 1] &= rightMask;
            }

            T val = *((T*)buf);
#ifdef POCO_ARCH_LITTLE_ENDIAN
            val = Poco::ByteOrder::flipBytes(val);
#endif
            val >>= bitOffsetRight;
            return val;
        }
    }


//    template<>
//    Poco::UInt8 getValue<Poco::UInt8>(unsigned int offset, unsigned int length)
//    {
//        unsigned int byteOffsetLeft = offset >> 3;
//        return *((Poco::UInt8*)(_data) + byteOffsetLeft);
//    }


    unsigned int length();
    void* data();

private:
    Poco::UInt8         _tableId;
    const unsigned int  _sizeMax;
    unsigned int        _size;
    void*               _data;
};


class Stream
{
    friend class Demux;

public:
    static const std::string Video;
    static const std::string Audio;
    static const std::string VideoMpeg1_11172;
    static const std::string VideoMpeg2_H262;
    static const std::string AudioMpeg1_11172;
    static const std::string AudioMpeg2_13818_3;
    static const std::string Mpeg2PrivateTableSections;
    static const std::string Mpeg2PesPrivateData;
    static const std::string MhegPackets;
    static const std::string Mpeg2AnnexA_DSMCC;
    static const std::string ITUTRecH2221;
    static const std::string ISO13818_6_typeA;
    static const std::string ISO13818_6_typeB;
    static const std::string ISO13818_6_typeC;
    static const std::string ISO13818_6_typeD;
    static const std::string Mpeg2ISO13818_1_Aux;
    static const std::string AudioISO13818_7_ADTS;
    static const std::string Mpeg4ISO14496_2;
    static const std::string AudioISO14496_3;
    static const std::string ISO14496_1_PesPackets;
    static const std::string ISO14496_1_Sections;
    static const std::string ISO13818_6_DownloadProt;
    static const std::string MetaDataPesPackets;
    static const std::string MetaDataSections;
    static const std::string Mpeg2UserPrivate;
    static const std::string AudioAtscAc3;
    static const std::string ProgramClock;
    static const std::string ProgramMapTable;
    static const std::string Other;

    Stream(const std::string& type, Poco::UInt16 pid);

    void readXml(Poco::XML::Node* pXmlStream);
    void writeXml(Poco::XML::Element* pService);

    std::string getType();
    unsigned int getPid();
    int getFileDesc();
    void read(Poco::UInt8* buf, int size);

    static Poco::UInt8 streamTypeFromString(const std::string& val);
    static std::string streamTypeToString(Poco::UInt8 val);

private:
    std::string         _type;
    Poco::UInt16        _pid;

    int                 _fileDesc;
};


class Service
{
    friend class Transponder;
    friend class Frontend;
    friend class Device;
    friend class Demux;

public:
//    Service(const std::string& name, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int pmtid);
    Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid);

    void addStream(Stream* pStream);
    void readXml(Poco::XML::Node* pXmlService);
    void writeXml(Poco::XML::Element* pTransponder);

private:
    Transponder*                _pTransponder;
    std::string                 _name;
    unsigned int                _sid;
    unsigned int                _pmtid;

    std::vector<Stream*>        _streams;
};


class Transponder
{
    friend class Device;
    friend class Frontend;
    friend class SatFrontend;
    friend class TerrestrialFrontend;
    friend class CableFrontend;
    friend class AtscFrontend;

public:
    Transponder(Frontend* pFrontend, unsigned int freq, unsigned int tid);
//    Transponder(unsigned int freq, unsigned int tid);

    void addService(Dvb::Service* pService);
    Service* getService(unsigned int serviceId);
    Service* getService(const std::string& serviceName);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

protected:
    virtual bool initTransponder(Poco::StringTokenizer& params) {}

    std::vector<Dvb::Service*>          _services;
    Poco::AutoPtr<Poco::XML::Element>   _pXmlTransponder;

private:
    Frontend*           _pFrontend;
    unsigned int        _freq;
    unsigned int        _tid;
};


class SatTransponder : public Transponder
{
    friend class SatFrontend;

public:
    static const std::string POL_HORIZ;
    static const std::string POL_VERT;

    SatTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid);

    void init(unsigned int satNum, unsigned int symbolRate, const std::string& polarization);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

    unsigned int        _satNum;
    unsigned int        _symbolRate;
    std::string         _polarization;
};


class TerrestrialTransponder : public Transponder
{
    friend class TerrestrialFrontend;

public:
    static const std::string BANDWIDTH_8_MHZ;
    static const std::string BANDWIDTH_7_MHZ;
    static const std::string BANDWIDTH_6_MHZ;
    static const std::string BANDWIDTH_AUTO;
    static const std::string BANDWIDTH_5_MHZ;
    static const std::string BANDWIDTH_10_MHZ;
    static const std::string BANDWIDTH_1_712_MHZ;
    static const std::string FEC_NONE;
    static const std::string FEC_1_2;
    static const std::string FEC_2_3;
    static const std::string FEC_3_4;
    static const std::string FEC_4_5;
    static const std::string FEC_5_6;
    static const std::string FEC_6_7;
    static const std::string FEC_7_8;
    static const std::string FEC_8_9;
    static const std::string FEC_AUTO;
    static const std::string FEC_3_5;
    static const std::string FEC_9_10;
    static const std::string QPSK;
    static const std::string QAM_16;
    static const std::string QAM_32;
    static const std::string QAM_64;
    static const std::string QAM_128;
    static const std::string QAM_256;
    static const std::string QAM_AUTO;
    static const std::string VSB_8;
    static const std::string VSB_16;
    static const std::string PSK_8;
    static const std::string APSK_16;
    static const std::string APSK_32;
    static const std::string DQPSK;
    static const std::string TRANSMISSION_MODE_2K;
    static const std::string TRANSMISSION_MODE_8K;
    static const std::string TRANSMISSION_MODE_AUTO;
    static const std::string TRANSMISSION_MODE_4K;
    static const std::string TRANSMISSION_MODE_1K;
    static const std::string TRANSMISSION_MODE_16K;
    static const std::string TRANSMISSION_MODE_32K;
    static const std::string GUARD_INTERVAL_1_32;
    static const std::string GUARD_INTERVAL_1_16;
    static const std::string GUARD_INTERVAL_1_8;
    static const std::string GUARD_INTERVAL_1_4;
    static const std::string GUARD_INTERVAL_AUTO;
    static const std::string GUARD_INTERVAL_1_128;
    static const std::string GUARD_INTERVAL_19_128;
    static const std::string GUARD_INTERVAL_19_256;
    static const std::string HIERARCHY_NONE;
    static const std::string HIERARCHY_1;
    static const std::string HIERARCHY_2;
    static const std::string HIERARCHY_4;
    static const std::string HIERARCHY_AUTO;

    TerrestrialTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid);

    void init(fe_bandwidth_t bandwidth,
            fe_code_rate_t code_rate_HP,
            fe_code_rate_t code_rate_LP,
            fe_modulation_t constellation,
            fe_transmit_mode_t transmission_mode,
            fe_guard_interval_t guard_interval,
            fe_hierarchy_t hierarchy_information);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

    fe_bandwidth_t bandwidthFromString(const std::string& val);
    std::string bandwidthToString(fe_bandwidth_t val);
    fe_code_rate_t coderateFromString(const std::string& val);
    std::string coderateToString(fe_code_rate_t val);
    fe_modulation_t modulationFromString(const std::string& val);
    std::string modulationToString(fe_modulation_t val);
    fe_transmit_mode_t transmitModeFromString(const std::string& val);
    std::string transmitModeToString(fe_transmit_mode_t val);
    fe_guard_interval_t guard_intervalFromString(const std::string& val);
    std::string guard_intervalToString(fe_guard_interval_t val);
    fe_hierarchy_t hierarchyFromString(const std::string& val);
    std::string hierarchyToString(fe_hierarchy_t val);

    fe_bandwidth_t              _bandwidth;
    fe_code_rate_t              _code_rate_HP;
    fe_code_rate_t              _code_rate_LP;
    fe_modulation_t             _constellation;
    fe_transmit_mode_t          _transmission_mode;
    fe_guard_interval_t         _guard_interval;
    fe_hierarchy_t              _hierarchy_information;
};


class CableTransponder : public Transponder
{
public:
    CableTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid);
//    CableTransponder(unsigned int freq, unsigned int tid);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

};


class AtscTransponder : public Transponder
{
public:
    AtscTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid);
//    AtscTransponder(unsigned int freq, unsigned int tid);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

};


// NOTE: The following API is linux specific

class Lnb;
class Frontend;
class Demux;
class Dvr;
class Adapter;


class Frontend
{
    friend class Device;
    friend class Adapter;
    friend class SignalCheckThread;

public:
    static const std::string Unknown;
    static const std::string DVBS;
    static const std::string DVBT;
    static const std::string DVBC;
    static const std::string ATSC;

    Frontend(Adapter* pAdapter, int num);
    ~Frontend();

    void addTransponder(Transponder* pTransponder);
    void openFrontend();
    void closeFrontend();

    void scan(const std::string& initialTransponderData);
    void readXml(Poco::XML::Node* pXmlFrontend);
    void writeXml(Poco::XML::Element* pAdapter);

    const std::string getType();
    const std::string getName();
    bool typeSupported();
//    bool tune(Channel* pChannel);
    virtual bool tune(Transponder* pTransponder) {}
//    void stopTune();
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tid) {}

    void listInitialTransponderData();
    void getInitialTransponderKeys(const std::string& countryCode, std::vector<std::string>& keys);
    void getInitialTransponderData(const std::string& key);

protected:
    bool waitForLock(Poco::Timestamp::TimeDiff timeout = 0);  // timeout in microseconds, 0 means forever
    bool hasLock();
    std::string trim(const std::string& str);

    int                         _fileDescFrontend;
    struct dvb_frontend_info    _feInfo;
    Poco::Timestamp::TimeDiff   _frontendTimeout;

private:
//    void diseqc(unsigned int satNum, Channel::Polarization pol, bool hiBand);
//    bool tuneFrontend(unsigned int freq, unsigned int symbolRate);
    void checkFrontend();

    Adapter*                    _pAdapter;
    std::string                 _deviceName;
    std::string                 _name;
    int                         _num;
    std::string                 _type;
    std::vector<Transponder*>   _initialTransponders;
    std::vector<Transponder*>   _transponders;
    Demux*                      _pDemux;
    Dvr*                        _pDvr;

    Poco::Thread                _t;
    SignalCheckThread*          _pt;
    Poco::FastMutex             _tuneLock;

    Poco::UTF8Encoding          _sourceEncoding;
    Poco::UTF8Encoding          _targetEncoding;
    Poco::TextConverter*        _pTextConverter;
};


class Lnb
{
    friend class Adapter;
    friend class Frontend;

public:
    Lnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal);

    bool isHiBand(unsigned int freq, unsigned int& ifreq);

private:
    std::string         _desc;
    unsigned long	_lowVal;
    unsigned long	_highVal;	// zero indicates no hiband
    unsigned long	_switchVal;	// zero indicates no hiband
};


class SatFrontend : public Frontend
{
public:
    SatFrontend(Adapter* pAdapter, int num);

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tid);

private:
    void diseqc(unsigned int satNum, const std::string& polarization, bool hiBand);

    std::map<std::string,Lnb*>  _lnbs;  // possible LNB types
    Lnb*                        _pLnb;
};


class TerrestrialFrontend : public Frontend
{
public:
    TerrestrialFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tid);
};


class CableFrontend : public Frontend
{
public:
    CableFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tid);
};


class AtscFrontend : public Frontend
{
public:
    AtscFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tid);
};


class Demux
{
    friend class Adapter;

public:
    enum Target { TargetDemux, TargetDvr };

    Demux(Adapter* pAdapter, int num);
    ~Demux();


    bool selectService(Service* pService, Target target, bool blocking = true);
    bool unselectService(Service* pService);
    bool runService(Service* pService, bool run = true);

    bool selectStream(Stream* pStream, Target target, bool blocking = true);
    bool unselectStream(Stream* pStream);
    bool runStream(Stream* pStream, bool run = true);
    bool setSectionFilter(Stream* pStream, Poco::UInt8 tableId);

    // deprecated
//    void start();
//    void stop();
//
//    void openDemux(bool blocking = true);
//    void closeDemux();
//
//    std::istream* getVideoStream();
//    std::istream* getAudioStream();

    unsigned int getPmtPid(unsigned int tid, int sid);
//    bool setVideoPid(unsigned int pid);
//    bool setAudioPid(unsigned int pid);
//    bool setPcrPid(unsigned int pid);
//    bool setPatPid(unsigned int pid);
//    bool setPmtPid(unsigned int pid);

private:
    bool setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType);
    bool setSectionFilter(Stream* pStream, uint8_t filter[18], uint8_t mask[18]);

    Adapter*                    _pAdapter;
    std::string                 _deviceName;
    int                         _num;

    // deprecated
    int                         _fileDescVideo;
    int                         _fileDescAudio;
    int                         _fileDescPcr;
    int                         _fileDescPat;
    int                         _fileDescPmt;
};


class Dvr
{
    friend class Adapter;

public:
    Dvr(Adapter* pAdapter, int num);
    ~Dvr();

    void openDvr(bool blocking = true);
    void closeDvr();
    void clearBuffer();
    void prefillBuffer();
    void startReadThread();
    void stopReadThread();
    bool readThreadRunning();
    void setBlocking(bool blocking = true);

    std::istream* getStream();

private:
    void readThread();

    Adapter*                            _pAdapter;
    std::string                         _deviceName;
    int                                 _num;
    int                                 _fileDescDvr;
    AvStream::ByteQueue                 _byteQueue;
    const int                           _bufferSize;
    std::istream*                       _pDvrStream;

    bool                                _useByteQueue;
    struct pollfd                       _fileDescPoll[1];
    const int                           _pollTimeout;  // wait for _pollTimeout millisec for new data on dvr device
    Poco::Thread*                       _pReadThread;
    Poco::RunnableAdapter<Dvr>          _readThreadRunnable;
    bool                                _readThreadRunning;
    Poco::FastMutex                     _readThreadLock;
};


class Adapter
{
    friend class Device;
    friend class Frontend;
    friend class Demux;
    friend class Dvr;

public:
    Adapter(int num);
    ~Adapter();

    void addFrontend(Frontend* pFrontend);
    void openAdapter();
    void closeAdapter();

    void getDeviceInfo();  // TODO: implement DvbAdapter::getDeviceInfo()
//    Demux* getDemux();
//    Dvr* getDvr();

    void readXml(Poco::XML::Node* pXmlAdapter);
    void writeXml(Poco::XML::Element* pDvbDevice);

private:
    int                         _num;
    std::string                 _deviceName;
//    Frontend*           _pFrontend;
    std::vector<Frontend*>      _frontends;
    bool                        _recPsi;
};


class Device
{
    friend class Adapter;
    friend class SignalCheckThread;

public:
    static Device* instance();

    typedef std::map<std::string, std::vector<Transponder*> >::iterator ServiceIterator;
    ServiceIterator serviceBegin();
    ServiceIterator serviceEnd();

//    void init();
    void open();
    void scan(const std::string& initialTransponderData);
    void readXml(std::istream& istream);
    void writeXml(std::ostream& ostream);

    void addAdapter(Adapter* pAdapter);

    Transponder* getTransponder(const std::string& serviceName);
//    bool tune(Transponder* pTransponder);
//    void stopTune();

    std::istream* getStream(const std::string& serviceName);
//    std::istream* getStream();
    void freeStream(std::istream* pIstream);

    bool useDvrDevice();
    bool blockDvrDevice();

private:
    Device();
    ~Device();

    void initServiceMap();

    static Device*                                      _pInstance;

    std::vector<Adapter*>                               _adapters;
    std::map<std::string, std::vector<Transponder*> >   _serviceMap;
    std::map<std::istream*, Service*>                   _streamMap;

    bool                                                _useDvrDevice;
    bool                                                _blockDvrDevice;
    bool                                                _reopenDvrDevice;
};

}  // namespace Omm
}  // namespace Dvb

#endif
