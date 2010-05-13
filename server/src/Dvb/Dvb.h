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

#ifndef OmmDvb_INCLUDED
#define OmmDvb_INCLUDED

#include <Poco/Thread.h>
#include <Poco/Mutex.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <linux/dvb/audio.h>

#include <Omm/UpnpAvServer.h>

class DvbDevice;
class DvbChannel;


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


class MediaContainerPlugin : public Omm::Av::MediaServerContainer
{
public:
    MediaContainerPlugin();
};


class DvbResource : public Omm::Av::ServerResource
{
public:
    DvbResource(const std::string& resourceId, const std::string& protInfo, DvbChannel* pChannel);
    
    virtual bool isSeekable() { return false; }
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
private:
    DvbChannel*         _pChannel;
};


class DvbChannel
{
    friend class DvbDevice;
    friend class DvbFrontend;
    friend class SignalCheckThread;
    
public:
    typedef enum {HORIZ = 0, VERT = 1} Polarization;
    
    DvbChannel(unsigned int _satNum, unsigned int freq, Polarization pol, unsigned int _symbolRate, unsigned int vpid, unsigned int apid, int sid);
    
    
private:
    unsigned int _satNum;
    unsigned int _freq;
    Polarization _pol;
    unsigned int _symbolRate;
    unsigned int _vpid;
    unsigned int _apid;
    int _sid;
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
    void getDeviceInfo();  // TODO: implement DvbAdapter::getDeviceInfo()
    
    
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


class SignalCheckThread : public Poco::Runnable
{
public:
    SignalCheckThread(DvbFrontend* pFrontend);
    
    void run();
    void stop();
    
private:
//     DvbChannel*         _pChannel;
    DvbFrontend*        _pFrontend;
    bool                _stop;
};


class DvbFrontend
{
    friend class DvbAdapter;
    friend class SignalCheckThread;
    
public:
    DvbFrontend(DvbAdapter* pAdapter, int num);
    ~DvbFrontend();
    
    void openFrontend();
    bool tune(DvbChannel* pChannel);
    void stopTune();
    
private:
    void diseqc(unsigned int satNum, DvbChannel::Polarization pol, bool hiBand);
    bool tuneFrontend(unsigned int freq, unsigned int symbolRate);
    void checkFrontend();
    
    DvbAdapter*                 _pAdapter;
    std::string                 _deviceName;
    int                         _num;
    int                         _fileDesc;
    struct dvb_frontend_info    _feInfo;
    
    Poco::Thread                    _t;
    SignalCheckThread*                   _pt;
    Poco::FastMutex                 _tuneLock;
};


class DvbDemux
{
    friend class DvbAdapter;
    
public:
    DvbDemux(DvbAdapter* pAdapter, int num);
    ~DvbDemux();
    
    void openDemux();
    
    unsigned int getPmtPid(int sid);
    
    bool setVideoPid(unsigned int pid);
    bool setAudioPid(unsigned int pid);
    bool setPatPid(unsigned int pid);
    bool setPmtPid(unsigned int pid);
    
private:
    bool setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType);
    
    DvbAdapter*                 _pAdapter;
    std::string                 _deviceName;
    int                         _num;
    int                         _fileDescVideo;
    int                         _fileDescAudio;
    int                         _fileDescPat;
    int                         _fileDescPmt;
};


class DvbDvr
{
    friend class DvbAdapter;
    
public:
    DvbDvr(DvbAdapter* pAdapter, int num);
    ~DvbDvr();
    
    void openDvr();
    
private:
    DvbAdapter*                 _pAdapter;
    std::string                 _deviceName;
    int                         _num;
    std::ifstream               _dvrStream;
};


class DvbDevice
{
    friend class DvbAdapter;
    friend class SignalCheckThread;
    
public:
    static DvbDevice* instance();
    
    void addAdapter(DvbAdapter* pAdapter);
    
    void tune(DvbChannel* pChannel);
    void stopTune();
    
//     std::istream& getTransportStream(DvbChannel* pChannel);
//     std::ostream& getPacketizedElementaryStream(Channel* pChannel);
//     std::ostream& getProgramStream(Channel* pChannel);
    
    
private:
    DvbDevice();
    ~DvbDevice();
    
    static DvbDevice*               _pInstance;
    
    std::map<std::string,DvbLnb*>   _lnbs;  // possible LNB types
    std::vector<DvbAdapter*>        _adapters;
};

#endif
