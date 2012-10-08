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
#include <vector>

#include <Omm/Dvb/DvbLogger.h>
#include <Omm/Dvb/Transponder.h>
#include <Omm/Dvb/Service.h>

#include "DvbServer.h"


DvbModel::DvbModel()
{
    LOGNS(Omm::Dvb, dvb, debug, "dvb model ctor.");
}


void
DvbModel::init()
{
    LOGNS(Omm::Dvb, dvb, debug, "dvb model init ...");

//    _channels.setConfFilePath(getBasePath());
//    _channels.scanChannels();
//    Omm::Dvb::Device::instance()->init();

    // TODO: base path should be initial transponder lists
    // dvb.xml should be set else where
    std::ifstream xmlDevice(getBasePath().c_str());
    Omm::Dvb::Device::instance()->readXml(xmlDevice);
    Omm::Dvb::Device::instance()->open();

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
//    _channels.scanChannels();
//    for (Omm::Dvb::Channels::ChannelIterator it = _channels.beginChannel(); it != _channels.endChannel(); ++it) {
//        addPath(it->first);
//    }

    // TODO: do a frontend scan

    std::ifstream xmlDevice(getBasePath().c_str());
    Omm::Dvb::Device::instance()->readXml(xmlDevice);
    for (Omm::Dvb::Device::ServiceIterator it = Omm::Dvb::Device::instance()->serviceBegin(); it != Omm::Dvb::Device::instance()->serviceEnd(); ++it) {
        for (std::vector<Omm::Dvb::Transponder*>::iterator tit = it->second.begin(); tit != it->second.end(); ++it) {
            Omm::Dvb::Service* pService = (*tit)->getService(it->first);
            if (pService && pService->getStatus() == Omm::Dvb::Service::StatusRunning && !pService->getScrambled()) {
                // there is one transponder that can receive the service unscrambled
                addPath(it->first);
                break;
            }
        }
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
    return path;
}


bool
DvbModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
DvbModel::getStream(const std::string& path, const std::string& resourcePath)
{
    return Omm::Dvb::Device::instance()->getStream(path);
}


void
DvbModel::freeStream(std::istream* pIstream)
{
    Omm::Dvb::Device::instance()->freeStream(pIstream);
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
