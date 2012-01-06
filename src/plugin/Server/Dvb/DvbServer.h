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


class DvbModel : public Omm::Av::SimpleDataModel
{
    friend class DvbServer;

public:
    DvbModel();

    virtual std::string getModelClass();
    virtual void scan(bool recurse = true);

    virtual std::string getClass(const std::string& path);
    virtual std::string getTitle(const std::string& path);

    virtual std::string getMime(const std::string& path);
    virtual std::string getDlna(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");

private:
    void scanChannelConfig(const std::string& channelConfig);

    std::map<std::string, std::string>                  _channelNames;
    std::map<std::string, Omm::Dvb::DvbChannel*>        _channels;
};



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


//class DvbServer : public Omm::Av::ServerContainer
//{
//public:
//    DvbServer();
//
//    virtual void setBasePath(const std::string& basePath);
//
//private:
//    void setTimer(const std::string& channel, Poco::DateTime startDate, Poco::DateTime stopDate);
//    void timerCallback(Poco::Timer& timer);
//    void stopCallback(Poco::Timer& timer);
//
//    Poco::Timer _timer;
//    Omm::Dvb::DvbChannel* _pChannel;
//};


#endif
