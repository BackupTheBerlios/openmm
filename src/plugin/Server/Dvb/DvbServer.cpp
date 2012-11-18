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


void
DvbModel::deInit()
{
    LOGNS(Omm::Dvb, dvb, debug, "dvb model deInit ...");

    Omm::Dvb::Device::instance()->close();

    LOGNS(Omm::Dvb, dvb, debug, "dvb model deInit finished.");
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

    LOGNS(Omm::Dvb, dvb, debug, "dvb model scan ...");
    std::ifstream xmlDevice(getBasePath().c_str());
    Omm::Dvb::Device::instance()->readXml(xmlDevice);
    for (Omm::Dvb::Device::ServiceIterator it = Omm::Dvb::Device::instance()->serviceBegin(); it != Omm::Dvb::Device::instance()->serviceEnd(); ++it) {
//        LOGNS(Omm::Dvb, trace, debug, "add service " + it->first + " to dvb model");
//        LOGNS(Omm::Dvb, trace, debug, "transponder count: " + Poco::NumberFormatter::format(it->second.size()));
//        LOGNS(Omm::Dvb, trace, debug, "first transponder has freq: " + Poco::NumberFormatter::format(it->second[0]->getFrequency()));

        // NOTE: iterator for transponder vector doesn't work ... (?!)
        for (int i = 0; i < it->second.size(); i++) {
//            LOGNS(Omm::Dvb, trace, debug, "add transponder with freq: " + Poco::NumberFormatter::format(it->second[i]->getFrequency()));
            Omm::Dvb::Service* pService = it->second[i]->getService(it->first);
            if (pService && pService->getStatus() == Omm::Dvb::Service::StatusRunning
                    && !pService->getScrambled()
                    && (pService->isAudio() || pService->isSdVideo())) {
                // there is one transponder that can receive the service unscrambled
                addPath(it->first);
                break;
            }
        }
//        for (std::vector<Omm::Dvb::Transponder*>::iterator tit = it->second.begin(); tit != it->second.end(); ++it) {
//            LOGNS(Omm::Dvb, dvb, debug, "add transponder with freq: " + Poco::NumberFormatter::format((*tit)->getFrequency()));
//            Omm::Dvb::Service* pService = (*tit)->getService(it->first);
//            if (pService && pService->getStatus() == Omm::Dvb::Service::StatusRunning && !pService->getScrambled()) {
//                // there is one transponder that can receive the service unscrambled
//                addPath(it->first);
//                break;
//            }
//        }
    }
    LOGNS(Omm::Dvb, dvb, debug, "dvb model scan finished.");
}


std::string
DvbModel::getClass(const std::string& path)
{
    Omm::Dvb::Service* pService = Omm::Dvb::Device::instance()->getTransponder(path)->getService(path);
    if (pService->isAudio()) {
        return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_BROADCAST);
    }
    else if (pService->isSdVideo() || pService->isHdVideo()) {
        return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_BROADCAST);
    }
    else {
        return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM);
    }
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
    Omm::Dvb::Service* pService = Omm::Dvb::Device::instance()->getTransponder(path)->getService(path);
    if (pService->isAudio()) {
        return Omm::Av::Mime::AUDIO_MPEG;
    }
    else if (pService->isSdVideo() || pService->isHdVideo()) {
        return Omm::Av::Mime::VIDEO_MPEG;
    }
    else {
        return "";
    }
}


std::string
DvbModel::getDlna(const std::string& path)
{
    // TODO: add DLNA string for mpeg audio streams
    return "DLNA.ORG_PN=MPEG_PS_PAL";
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(DvbModel)
POCO_END_MANIFEST
#endif
