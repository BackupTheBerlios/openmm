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

#include <Omm/UpnpAvServer.h>

extern "C" {
#include "szap.h"
}

class DvbDevice;
class DvbChannel;

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


// NOTE: The following API is linux specific
class DvbChannel
{
    friend class DvbDevice;
    friend class TuningThread;
    
public:
    DvbChannel(unsigned int sat_no, unsigned int freq, unsigned int pol, unsigned int symbol_rate, unsigned int vpid, unsigned int apid, int sid);
    
    
private:
    unsigned int _sat_no;
    unsigned int _freq;
    unsigned int _pol;
    unsigned int _symbol_rate;
    unsigned int _vpid;
    unsigned int _apid;
    int _sid;
};


class TuningThread : public Poco::Runnable
{
public:
    TuningThread(DvbChannel* pChannel);
    
    void run();
    void stop();
    
private:
    DvbChannel*         _pChannel;
};


class DvbDevice
{
    friend class TuningThread;
    
public:
    static DvbDevice* instance();
    
    void tune(DvbChannel* pChannel);
    void stopTune();
//     std::istream& getTransportStream(DvbChannel* pChannel);
//     std::ostream& getPacketizedElementaryStream(Channel* pChannel);
//     std::ostream& getProgramStream(Channel* pChannel);
    
    
private:
    DvbDevice();
    
    static DvbDevice*     _pInstance;
    Poco::Thread          _t;
    TuningThread*         _pt;
    Poco::FastMutex       _tuneLock;
//     int                   _frontend;
//     std::ifstream         _dvbAdapter;
};

#endif
