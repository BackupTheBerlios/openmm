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
#include <Poco/StringTokenizer.h>
#include <Poco/Timer.h>
#include <Poco/File.h>
#include <fstream>

#include "DvbServer.h"


DvbModel::DvbModel()
{
    LOGNS(Omm::Dvb, dvb, debug, "dvb model ctor.");
}


void
DvbModel::init()
{
    LOGNS(Omm::Dvb, dvb, debug, "dvb model init ...");

    _channels.setConfFilePath(getBasePath());
    _channels.scanChannels();
    Omm::Dvb::DvbAdapter* pAdapter = new Omm::Dvb::DvbAdapter(0);
    Omm::Dvb::DvbDevice::instance()->addAdapter(pAdapter);

    LOGNS(Omm::Dvb, dvb, debug, "dvb model init finished.");
}


std::string
DvbModel::getModelClass()
{
    return "DvbModel";
}


Omm::ui4
DvbModel::getSystemUpdateId(bool checkMod)
{
    return Poco::File(getBasePath()).getLastModified().epochTime();
}


void
DvbModel::scan()
{
    _channels.scanChannels();
    for (Omm::Dvb::DvbChannels::ChannelIterator it = _channels.beginChannel(); it != _channels.endChannel(); ++it) {
        addPath(it->first);
    }
}


std::string
DvbModel::getClass(const std::string& path)
{
    return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_BROADCAST);
}


std::string
DvbModel::getTitle(const std::string& path)
{
    return _channels.getChannel(path)->getName();
}


bool
DvbModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
DvbModel::getStream(const std::string& path, const std::string& resourcePath)
{
    // NOTE:  two subsequent getStream() without stopping the stream may lead to
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

    bool tuneSuccess = Omm::Dvb::DvbDevice::instance()->tune(_channels.getChannel(path));
    if (!tuneSuccess) {
        return 0;
    }

    LOGNS(Omm::Dvb, dvb, debug, "reading from dvr device ...");
    return Omm::Dvb::DvbDevice::instance()->getStream();
}


void
DvbModel::freeStream(std::istream* pIstream)
{
    Omm::Dvb::DvbDevice::instance()->freeStream(pIstream);
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
//    LOGNS(Omm::Dvb, dvb, debug, "set timer, channel: " + Poco::NumberFormatter::format(index)\
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
//    LOGNS(Omm::Dvb, dvb, debug, "reading from dvr device ...");
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
