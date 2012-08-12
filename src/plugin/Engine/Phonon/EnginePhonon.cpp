/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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
#include "EnginePhonon.h"
#include "Omm/UpnpAv.h"

#include <phonon/phonon>


PhononEngine::PhononEngine()
{
    _engineId = "Phonon engine " + Omm::OMM_VERSION;
}


PhononEngine::~PhononEngine()
{
}


void
PhononEngine::createPlayer()
{

}


void
PhononEngine::setUri(const std::string& uri, const Omm::Av::ProtocolInfo& protInfo)
{
//     _uri = uri.substr(0, 4) + "/ffmpeg" + uri.substr(4);
    _uri = uri;
    _mime = Omm::Av::Mime(protInfo.getMimeString());
}


// void
// PhononEngine::setUri(std::istream& istr, const Omm::Av::ProtocolInfo& protInfo)
// {
// //     _fd = istr.socket().sockfd();
// }


void
PhononEngine::play()
{
    if (_mime.isImage()) {
        Omm::Icon image(0, 0, 0, "", _uri);
        _pVisual->renderImage(image.getBuffer());
    }
    else if (_mime.isVideo()) {
        LOGNS(Omm::Av, upnpav, debug, "phonon engine: play ...");
        Phonon::VideoPlayer* pPlayer = new Phonon::VideoPlayer(Phonon::VideoCategory, static_cast<QWidget*>(_pVisual->getWindow()));
        pPlayer->play(Phonon::MediaSource(_uri.c_str()));
        LOGNS(Omm::Av, upnpav, debug, "phonon engine: play media finished.");
    }
    else if (_mime.isAudio()) {
        LOGNS(Omm::Av, upnpav, debug, "phonon engine: play ...");
        Phonon::MediaObject* pPlayer = Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(_uri.c_str()));
        pPlayer->play();
        LOGNS(Omm::Av, upnpav, debug, "phonon engine: play media finished.");
    }
}


void
PhononEngine::setSpeed(int nom, int denom)
{
}


void
PhononEngine::pause()
{
}


void
PhononEngine::stop()
{
}


void
PhononEngine::seekByte(Poco::UInt64 byte)
{
}


void
PhononEngine::seekPercentage(float percentage)
{
}


void
PhononEngine::seekSecond(float second)
{
}


Poco::UInt64
PhononEngine::getPositionByte()
{
}


float
PhononEngine::getPositionPercentage()
{
}


float
PhononEngine::getPositionSecond()
{
}


float
PhononEngine::getLengthSeconds()
{
}


PhononEngine::TransportState
PhononEngine::getTransportState()
{
//    LOGNS(Omm::Av, upnpav, debug, "phonon engine transport state: " + Poco::NumberFormatter::format(res));

//    switch (res) {
//        default:
//            return Stopped;
//    }
}


void
PhononEngine::setVolume(const std::string& channel, float vol)
{
}


float
PhononEngine::getVolume(const std::string& channel)
{
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(PhononEngine)
POCO_END_MANIFEST
#endif
