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
    //        UPDATE: man(2) close:
    //        It is probably unwise to close file descriptors while they may be in use by system calls in other threads  in  the
    //        same  process.  Since a file descriptor may be reused, there are some obscure race conditions that may cause unin-
    //        tended side effects.
    //        UPDATE: as long as the same thread accesses the device, it is not busy. This happens, if the http request is
    //        run in the same thread as the previous http request. Correction: even in same thread, dvr device cannot be
    //        opened ("Device or resource busy").

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


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(DvbModel)
POCO_END_MANIFEST
#endif
