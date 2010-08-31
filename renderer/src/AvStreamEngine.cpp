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

#include "AvStreamEngine.h"

#include "Omm/Util.h"

#include <sys/stat.h>
#include <cerrno>
#include <string>


AvStreamEngine::AvStreamEngine() :
_fullscreen(true),
_engineId("OMM AvStream engine " + Omm::OMM_VERSION)
{
    std::string audioPlugin("audiosink-alsa");
    Omm::Util::PluginLoader<Omm::AvStream::AudioSink> audioPluginLoader;
    try {
        _pAudioSink = audioPluginLoader.load(audioPlugin, "AudioSink");
    }
    catch(Poco::NotFoundException) {
        std::cerr << "Error could not find avstream audio plugin: " << audioPlugin << std::endl;
        return;
    }
    std::string videoPlugin("videosink-sdl");
    Omm::Util::PluginLoader<Omm::AvStream::VideoSink> videoPluginLoader;
    try {
        _pVideoSink = videoPluginLoader.load(videoPlugin, "VideoSink");
    }
    catch(Poco::NotFoundException) {
        std::cerr << "Error could not find avstream video plugin: " << videoPlugin << std::endl;
        return;
    }
}


AvStreamEngine::~AvStreamEngine()
{
}


void
AvStreamEngine::setFullscreen(bool on)
{
}


void
AvStreamEngine::setUri(std::string mrl)
{
    std::clog << "<<<<<<<<<<<< ENGINE SET. >>>>>>>>>>>>" << std::endl;
    _demuxer.set(mrl);
    
    if (_demuxer.firstAudioStream() < 0 && _demuxer.firstVideoStream() < 0) {
        std::clog << "no audio or video stream found, exiting" << std::endl;
        return;
    }
        
    //////////// load and attach audio Sink ////////////
    if (_demuxer.firstAudioStream() >= 0) {
        _demuxer.attach(_pAudioSink, _demuxer.firstAudioStream());
        Omm::AvStream::Clock::instance()->attachAudioSink(_pAudioSink);
    }
    
    //////////// load and attach video sink ////////////
    if (_demuxer.firstVideoStream() >= 0) {
        _demuxer.attach(_pVideoSink, _demuxer.firstVideoStream());
        Omm::AvStream::Clock::instance()->attachVideoSink(_pVideoSink);
    }
}


void
AvStreamEngine::load()
{
    std::clog << "<<<<<<<<<<<< ENGINE START ... >>>>>>>>>>>>" << std::endl;
    
    _demuxer.start();
    Omm::AvStream::Clock::instance()->setStartTime();

    std::clog << "<<<<<<<<<<<< ENGINE RUN ... >>>>>>>>>>>>" << std::endl;

    Omm::AvStream::Clock::instance()->start();
}


void
AvStreamEngine::stop()
{
    
    std::clog << "<<<<<<<<<<<< ENGINE HALT. >>>>>>>>>>>>" << std::endl;
    
    _demuxer.stop();
    Omm::AvStream::Clock::instance()->stop();
    
    std::clog << "<<<<<<<<<<<< ENGINE STOP. >>>>>>>>>>>>" << std::endl;
    
    ////////// deallocate meta data and packet queues ////////////
    if (_demuxer.firstAudioStream() >= 0) {
        _demuxer.detach(_demuxer.firstAudioStream());
    }
    if (_demuxer.firstVideoStream() >= 0) {
        _demuxer.detach(_demuxer.firstVideoStream());
    }
    
    std::clog << "<<<<<<<<<<<< ENGINE RESET. >>>>>>>>>>>>" << std::endl;
    
    _demuxer.reset();
    _pAudioSink->reset();
    _pVideoSink->reset();
    Omm::AvStream::Clock::instance()->reset();
}


void
AvStreamEngine::next()
{
}


void
AvStreamEngine::previous()
{
}


void
AvStreamEngine::pause()
{

}


void
AvStreamEngine::seek(int seconds)
{
}


bool
AvStreamEngine::isSeekable()
{
    // TODO: depends on http header received from server
    return false;
}


void
AvStreamEngine::savePosition()
{

}


void
AvStreamEngine::setSpeed(int nom, int denom)
{
}


void
AvStreamEngine::getPosition(float &seconds)
{
}


void
AvStreamEngine::getLength(float &seconds)
{
}


void
AvStreamEngine::setVolume(int channel, float vol)
{
}


void
AvStreamEngine::getVolume(int channel, float &vol)
{
}



POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(AvStreamEngine)
POCO_END_MANIFEST