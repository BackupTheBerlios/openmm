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
#include <Poco/ClassLibrary.h>
#include <Poco/StreamCopier.h>
#include <Poco/Timer.h>


#include "DvbServer.h"


class DvbDataModel : public Omm::Av::SimpleDataModel
{
    friend class DvbServer;

public:
    DvbDataModel(const std::string& channelConfig);

    virtual Omm::ui4 getChildCount();
    virtual std::string getClass(Omm::ui4 index);
    virtual std::string getTitle(Omm::ui4 index);

    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index, const std::string& resourcePath = "");
    virtual std::istream* getStream(Omm::ui4 index, const std::string& resourcePath = "");

private:
    void scanChannelConfig(const std::string& channelConfig);

    std::vector<std::string>             _channelNames;
    std::vector<Omm::Dvb::DvbChannel*>   _channels;
};


DvbDataModel::DvbDataModel(const std::string& channelConfig)
{
    scanChannelConfig(channelConfig);
    Omm::Dvb::DvbAdapter* pAdapter = new Omm::Dvb::DvbAdapter(0);
    Omm::Dvb::DvbDevice::instance()->addAdapter(pAdapter);
}


Omm::ui4
DvbDataModel::getChildCount()
{
    return _channelNames.size();
}


std::string
DvbDataModel::getClass(Omm::ui4 index)
{
    return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_BROADCAST);
}


std::string
DvbDataModel::getTitle(Omm::ui4 index)
{
    return _channelNames[index];
}


bool
DvbDataModel::isSeekable(Omm::ui4 index, const std::string& resourcePath)
{
    return false;
}


std::istream*
DvbDataModel::getStream(Omm::ui4 index, const std::string& resourcePath)
{
    Omm::Dvb::DvbDevice::instance()->tune(_channels[index]);

    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
    std::ifstream* pIstr = new std::ifstream("/dev/dvb/adapter0/dvr0");
    return pIstr;
}


std::string
DvbDataModel::getMime(Omm::ui4 index)
{
    return "video/mpeg";
}


std::string
DvbDataModel::getDlna(Omm::ui4 index)
{
    return "DLNA.ORG_PN=MPEG_PS_PAL";
}


void
DvbDataModel::scanChannelConfig(const std::string& channelConfig)
{
    std::ifstream channels(channelConfig.c_str());
    std::string line;
    while (getline(channels, line)) {
        Poco::StringTokenizer channelParams(line, ":");
        Poco::StringTokenizer channelName(channelParams[0], ";");
        _channelNames.push_back(channelName[0]);
        unsigned int freq = Poco::NumberParser::parseUnsigned(channelParams[1]) * 1000;
        Omm::Dvb::DvbChannel::Polarization pol = (channelParams[2][0] == 'h') ? Omm::Dvb::DvbChannel::HORIZ : Omm::Dvb::DvbChannel::VERT;
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
        _channels.push_back(new Omm::Dvb::DvbChannel(0, freq, pol, symbolRate, vpid, cpid, apid, sid));
    }
}


DvbServer::DvbServer() :
TorchServerContainer(8888)
{
}


void
DvbServer::setBasePath(const std::string& basePath)
{
    ServerContainer::setBasePath(basePath);
    setDataModel(new DvbDataModel(basePath));
    setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER, Omm::Av::AvClass::VIDEO_BROADCAST));
    //setTimer("ProSieben", Poco::DateTime(2011, 1, 9, 18, 27), Poco::DateTime(2011, 1, 9, 18, 8));
}


void
DvbServer::setTimer(const std::string& channel, Poco::DateTime startDate, Poco::DateTime stopDate)
{
    DvbDataModel* pDvbDataModel = static_cast<DvbDataModel*>(_pDataModel);
    Omm::ui4 index = std::find(pDvbDataModel->_channelNames.begin(), pDvbDataModel->_channelNames.end(), channel)
                    - pDvbDataModel->_channelNames.begin();

    _pChannel = pDvbDataModel->_channels[index];
    Poco::DateTime nowDate;
    Omm::Dvb::Log::instance()->dvb().debug("set timer, channel: " + Poco::NumberFormatter::format(index)
                + ", start:" + Poco::NumberFormatter::format((startDate - nowDate).milliseconds()));

    // FIXME: startInterval is wrong (too short)
    _timer.setStartInterval((startDate - nowDate).milliseconds());
    Poco::TimerCallback<DvbServer> callback(*this, &DvbServer::timerCallback);
    _timer.start(callback);
}


void
DvbServer::timerCallback(Poco::Timer& timer)
{
    Omm::Dvb::DvbDevice::instance()->tune(_pChannel);

    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
    std::ifstream istr("/dev/dvb/adapter0/dvr0");
    std::ofstream ostr("/var/local/ommrec.mpg");
    // TODO: stop StreamCopier::copyStream() in the timerCallback thread by closing istr and/or ostr
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
}


// RecTimer::RecTimer() :
// _pChannel(0)
// {
// }
//
//
// void
// RecTimer::setChannel(Omm::Dvb::DvbChannel* pChannel)
// {
//     _pChannel = pChannel;
// }


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::ServerContainer)
POCO_EXPORT_CLASS(DvbServer)
POCO_END_MANIFEST
#endif
