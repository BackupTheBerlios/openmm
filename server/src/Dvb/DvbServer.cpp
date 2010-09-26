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

#include "DvbServer.h"


class DvbDataModel : public Omm::Av::AbstractDataModel
{
public:
    DvbDataModel(const std::string& channelConfig);
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getTitle(Omm::ui4 index);
    
    virtual Omm::ui4 getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::streamsize stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek);

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
DvbDataModel::getTitle(Omm::ui4 index)
{
    return _channelNames[index];
}


bool
DvbDataModel::isSeekable(Omm::ui4 index)
{
    return false;
}


std::streamsize
DvbDataModel::stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek)
{
    Omm::Dvb::DvbDevice::instance()->tune(_channels[index]);
    
    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
    std::ifstream istr("/dev/dvb/adapter0/dvr0");
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
    return bytes;
}


Omm::ui4
DvbDataModel::getSize(Omm::ui4 index)
{
    return 0;
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
        Poco::StringTokenizer audioChannel(channelParams[6], ";");
        Poco::StringTokenizer audioPid(audioChannel[0], "=");
        unsigned int apid = Poco::NumberParser::parseUnsigned(audioPid[0]);
        int sid = Poco::NumberParser::parseUnsigned(channelParams[9]);
        _channels.push_back(new Omm::Dvb::DvbChannel(0, freq, pol, symbolRate, vpid, apid, sid));
    }
}


DvbServer::DvbServer() :
TorchServer(8888)
{
}


void
DvbServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        setDataModel(new DvbDataModel(value));
    }
}


POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(DvbServer)
POCO_END_MANIFEST
