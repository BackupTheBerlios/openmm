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

#ifndef Dvb_INCLUDED
#define Dvb_INCLUDED

#include <Poco/Thread.h>
#include <Poco/RunnableAdapter.h>
#include <Poco/Mutex.h>
#include <Poco/Logger.h>
#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <sys/poll.h>

#include "AvStream.h"

namespace Omm {
namespace Dvb {

class DvbDevice;
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


class DvbChannel
{
    friend class DvbDevice;
    friend class DvbFrontend;
    friend class SignalCheckThread;

public:
    typedef enum {HORIZ = 0, VERT = 1} Polarization;

    DvbChannel(const std::string& name, unsigned int _satNum, unsigned int freq, Polarization pol, unsigned int _symbolRate, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid);

    std::string getName();

private:
    std::string         _name;
    unsigned int        _satNum;
    unsigned int        _freq;
    Polarization        _pol;
    unsigned int        _symbolRate;
    unsigned int        _vpid;
    unsigned int        _cpid;
    unsigned int        _apid;
    int                 _sid;
    unsigned int        _tid;
    unsigned int        _pmtid;
};


class DvbChannels
{
public:
    typedef std::map<std::string, DvbChannel*>::iterator ChannelIterator;
    ChannelIterator beginChannel();
    ChannelIterator endChannel();

    virtual void scanChannels() {}

    DvbChannel* getChannel(const std::string& name);

protected:
    std::map<std::string, DvbChannel*> _channelMap;
};


class ChannelsConf : public DvbChannels
{
public:
    virtual void scanChannels();

    void setConfFilePath(const std::string& confFilePath);

private:
    void scanChannelConfig(const std::string& channelConfig);
    void clearChannels();

    std::string _confFilePath;
};


// NOTE: The following API is linux specific

class DvbLnb;
class DvbFrontend;
class DvbDemux;
class DvbDvr;

class DvbAdapter
{
    friend class DvbDevice;
    friend class DvbFrontend;
    friend class DvbDemux;
    friend class DvbDvr;

public:
    DvbAdapter(int num);
    ~DvbAdapter();

    void openAdapter();
    void closeAdapter();

    void getDeviceInfo();  // TODO: implement DvbAdapter::getDeviceInfo()
    DvbDemux* getDemux();
    DvbDvr* getDvr();

private:
    int             _num;
    std::string     _deviceName;
    DvbLnb*         _pLnb;
    DvbFrontend*    _pFrontend;
    DvbDemux*       _pDemux;
    DvbDvr*         _pDvr;
    bool            _recPsi;
};


class DvbLnb
{
    friend class DvbAdapter;
    friend class DvbFrontend;

public:
    DvbLnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal);

    bool isHiBand(unsigned int freq, unsigned int& ifreq);

private:
    std::string         _desc;
    unsigned long	_lowVal;
    unsigned long	_highVal;	// zero indicates no hiband
    unsigned long	_switchVal;	// zero indicates no hiband
};


class DvbFrontend
{
    friend class DvbAdapter;
    friend class SignalCheckThread;

public:
    DvbFrontend(DvbAdapter* pAdapter, int num);
    ~DvbFrontend();

    void openFrontend();
    void closeFrontend();

    bool tune(DvbChannel* pChannel);
    void stopTune();

private:
    void diseqc(unsigned int satNum, DvbChannel::Polarization pol, bool hiBand);
    bool tuneFrontend(unsigned int freq, unsigned int symbolRate);
    void checkFrontend();

    DvbAdapter*                 _pAdapter;
    std::string                 _deviceName;
    int                         _num;
    int                         _fileDescFrontend;
    struct dvb_frontend_info    _feInfo;

    Poco::Thread                _t;
    SignalCheckThread*          _pt;
    Poco::FastMutex             _tuneLock;
};


class DvbDemux
{
    friend class DvbAdapter;

public:
    DvbDemux(DvbAdapter* pAdapter, int num);
    ~DvbDemux();

    void openDemux(bool blocking = true);
    void closeDemux();
    void start();
    void stop();

    unsigned int getPmtPid(unsigned int tid, int sid);

    bool setVideoPid(unsigned int pid);
    bool setAudioPid(unsigned int pid);
    bool setPcrPid(unsigned int pid);
    bool setPatPid(unsigned int pid);
    bool setPmtPid(unsigned int pid);

    std::istream* getVideoStream();
    std::istream* getAudioStream();

private:
    bool setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType);

    DvbAdapter*                 _pAdapter;
    std::string                 _deviceName;
    int                         _num;
    int                         _fileDescVideo;
    int                         _fileDescAudio;
    int                         _fileDescPcr;
    int                         _fileDescPat;
    int                         _fileDescPmt;
};


class DvbDvr
{
    friend class DvbAdapter;

public:
    DvbDvr(DvbAdapter* pAdapter, int num);
    ~DvbDvr();

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

    DvbAdapter*                         _pAdapter;
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
    Poco::RunnableAdapter<DvbDvr>       _readThreadRunnable;
    bool                                _readThreadRunning;
    Poco::FastMutex                     _readThreadLock;
};


class DvbDevice
{
    friend class DvbAdapter;
    friend class SignalCheckThread;

public:
    static DvbDevice* instance();

    void addAdapter(DvbAdapter* pAdapter);

    bool tune(DvbChannel* pChannel);
    void stopTune();

    std::istream* getStream();
    void freeStream(std::istream* pIstream);

    bool useDvrDevice();
    bool blockDvrDevice();

private:
    DvbDevice();
    ~DvbDevice();

    static DvbDevice*               _pInstance;

    std::map<std::string,DvbLnb*>   _lnbs;  // possible LNB types
    std::vector<DvbAdapter*>        _adapters;

    bool                            _useDvrDevice;
    bool                            _blockDvrDevice;
    bool                            _reopenDvrDevice;
};

}  // namespace Omm
}  // namespace Dvb

#endif
