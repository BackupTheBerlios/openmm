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

#ifndef DvbServer_INCLUDED
#define DvbServer_INCLUDED

#include <Poco/DateTime.h>

#include <Omm/UpnpAvServer.h>
#include <Omm/Dvb.h>


// class RecTimer : public Poco::Timer
// {
// public:
//     RecTimer();
//     
//     virtual void run();
//     void setChannel(Omm::Dvb::DvbChannel* pChannel);
//     
// private:
//     
// };


class DvbServer : public Omm::Av::TorchServer
{
public:
    DvbServer();
    
    virtual void setOption(const std::string& key, const std::string& value);
    
private:
    void setTimer(const std::string& channel, Poco::DateTime startDate, Poco::DateTime stopDate);
    void timerCallback(Poco::Timer& timer);
    void stopCallback(Poco::Timer& timer);
    
    Poco::Timer _timer;
    Omm::Dvb::DvbChannel* _pChannel;
};


#endif
