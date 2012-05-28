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
#include <Poco/File.h>
#include <fstream>


#include "DvbServer.h"


DvbModel::DvbModel() :
//_pRecDevice(0)
_pRecDevice(new std::ifstream("/dev/dvb/adapter0/dvr0"))
{
}


std::string
DvbModel::getModelClass()
{
    return "DvbModel";
}


Omm::ui4
DvbModel::getUpdateId(bool recurse)
{
    return Poco::File(getBasePath()).getLastModified().epochTime();
}


void
DvbModel::scan(bool recurse)
{
    scanChannelConfig(getBasePath());
    Omm::Dvb::DvbAdapter* pAdapter = new Omm::Dvb::DvbAdapter(0);
    Omm::Dvb::DvbDevice::instance()->addAdapter(pAdapter);
}


std::string
DvbModel::getClass(const std::string& path)
{
    return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_BROADCAST);
}


std::string
DvbModel::getTitle(const std::string& path)
{
    return _channelNames[path];
}


bool
DvbModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
DvbModel::getStream(const std::string& path, const std::string& resourcePath)
{
    // FIXME: two subsequent getStream() without stopping the stream may lead to
    //        a blocked dvr device: engine stops reading the previous stream
    //        when receiving the new stream. This may overlap and the file
    //        handle is still open. Even if the engine is stopped right before
    //        playing a new stream, it could take a while until reading of stream
    //        is stopped, too (stop() and play() are typically async calls into
    //        the engine).
    //        DvbModel needs a way to interrupt current stream and close file
    //        handles.
    //        UPDATE: this only happens when renderer and dvb server run in the
    //        same process.
    if (_pRecDevice) {
        Omm::Dvb::Log::instance()->dvb().debug("releasing dvr device");
        _pRecDevice->close();
//        delete _pRecDevice;
//        _pRecDevice = 0;
    }

    bool tuneSuccess = Omm::Dvb::DvbDevice::instance()->tune(_channels[path]);
    if (!tuneSuccess) {
        return 0;
    }

//    Poco::Thread::sleep(250);
    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
//    std::ifstream* pIstr = new std::ifstream("/dev/dvb/adapter0/dvr0");
//    _pRecDevice = new std::ifstream("/dev/dvb/adapter0/dvr0");
    _pRecDevice->open("/dev/dvb/adapter0/dvr0");
    if (!*_pRecDevice) {
        Omm::Dvb::Log::instance()->dvb().error("dvr device busy");
//        delete _pRecDevice;
//        _pRecDevice = 0;
    }
    return _pRecDevice;
}


std::string
DvbModel::getMime(const std::string& path)
{
    return "video/mpeg";
}


std::string
DvbModel::getDlna(const std::string& path)
{
    return "DLNA.ORG_PN=MPEG_PS_PAL";
}


void
DvbModel::scanChannelConfig(const std::string& channelConfig)
{
    std::ifstream channels(channelConfig.c_str());
    std::string line;
    while (getline(channels, line)) {
        Poco::StringTokenizer channelParams(line, ":");
        Poco::StringTokenizer channelName(channelParams[0], ";");
        _channelNames[line] = channelName[0];
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
        _channels[line] = new Omm::Dvb::DvbChannel(0, freq, pol, symbolRate, vpid, cpid, apid, sid);
        addPath(line);
    }
}


//    //setTimer("ProSieben", Poco::DateTime(2011, 1, 9, 18, 27), Poco::DateTime(2011, 1, 9, 18, 8));
//void
//DvbServer::setTimer(const std::string& channel, Poco::DateTime startDate, Poco::DateTime stopDate)
//{
    // TODO: need to convert to new path based data model.
//    DvbDataModel* pDvbDataModel = static_cast<DvbDataModel*>(_pDataModel);
//    Omm::ui4 index = std::find(pDvbDataModel->_channelNames.begin(), pDvbDataModel->_channelNames.end(), channel)
//                    - pDvbDataModel->_channelNames.begin();
//
//    _pChannel = pDvbDataModel->_channels[index];
//    Poco::DateTime nowDate;
//    Omm::Dvb::Log::instance()->dvb().debug("set timer, channel: " + Poco::NumberFormatter::format(index)
//                + ", start:" + Poco::NumberFormatter::format((startDate - nowDate).milliseconds()));
//
//    // FIXME: startInterval is wrong (too short)
//    _timer.setStartInterval((startDate - nowDate).milliseconds());
//    Poco::TimerCallback<DvbServer> callback(*this, &DvbServer::timerCallback);
//    _timer.start(callback);
//}


//void
//DvbServer::timerCallback(Poco::Timer& timer)
//{
//    Omm::Dvb::DvbDevice::instance()->tune(_pChannel);
//
//    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
//    std::ifstream istr("/dev/dvb/adapter0/dvr0");
//    std::ofstream ostr("/var/local/ommrec.mpg");
//    // TODO: stop StreamCopier::copyStream() in the timerCallback thread by closing istr and/or ostr
//    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
//}


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
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(DvbModel)
POCO_END_MANIFEST
#endif
