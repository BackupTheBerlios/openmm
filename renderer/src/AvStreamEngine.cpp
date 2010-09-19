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
_engineId("OMM AvStream engine " + Omm::OMM_VERSION),
_isPlaying(false),
_pClock(0),
_pDemuxer(0),
_pAudioSink(0),
_pVideoSink(0)
{
}


AvStreamEngine::~AvStreamEngine()
{
    destructPlayer();
}


void
AvStreamEngine::createPlayer()
{
    _pClock = new Omm::AvStream::Clock;
    _pDemuxer = new Omm::AvStream::Demuxer;
    
//     std::string audioPlugin("audiosink-alsa");
    std::string audioPlugin("audiosink-sdl");
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
    _pVideoSink->openWindow(_fullscreen, _width, _height);
}


void
AvStreamEngine::destructPlayer()
{
    delete _pClock;
    _pClock = 0;
    delete _pDemuxer;
    _pDemuxer = 0;
    delete _pAudioSink;
    _pAudioSink = 0;
    delete _pVideoSink;
    _pVideoSink = 0;
}


void
AvStreamEngine::setFullscreen(bool on)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::setUri(std::string mrl)
{
    if (_isPlaying) {
        stop();
    }
    
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
    std::clog << "<<<<<<<<<<<< ENGINE SET. >>>>>>>>>>>>" << std::endl;
    _pDemuxer->set(mrl);
    
    if (_pDemuxer->firstAudioStream() < 0 && _pDemuxer->firstVideoStream() < 0) {
        std::clog << "no audio or video stream found, exiting" << std::endl;
        return;
    }
        
    //////////// load and attach audio Sink ////////////
    if (_pDemuxer->firstAudioStream() >= 0) {
        _pDemuxer->attach(_pAudioSink, _pDemuxer->firstAudioStream());
        _pClock->attachAudioSink(_pAudioSink);
    }
    
    //////////// load and attach video sink ////////////
    if (_pDemuxer->firstVideoStream() >= 0) {
        _pDemuxer->attach(_pVideoSink, _pDemuxer->firstVideoStream());
        _pClock->attachVideoSink(_pVideoSink);
    }
}


void
AvStreamEngine::load()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

    std::clog << "<<<<<<<<<<<< ENGINE START ... >>>>>>>>>>>>" << std::endl;
    
    _pDemuxer->start();
    _pClock->setStartTime(true);

    std::clog << "<<<<<<<<<<<< ENGINE RUN ... >>>>>>>>>>>>" << std::endl;

    _pClock->start();
    _isPlaying = true;
}


void
AvStreamEngine::stop()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

    std::clog << "<<<<<<<<<<<< ENGINE HALT. >>>>>>>>>>>>" << std::endl;
    
    _pDemuxer->stop();
    _pClock->stop();
    
    std::clog << "<<<<<<<<<<<< ENGINE STOP. >>>>>>>>>>>>" << std::endl;
    
    ////////// deallocate meta data and packet queues ////////////
    if (_pDemuxer->firstAudioStream() >= 0) {
        _pDemuxer->detach(_pDemuxer->firstAudioStream());
    }
    if (_pDemuxer->firstVideoStream() >= 0) {
        _pDemuxer->detach(_pDemuxer->firstVideoStream());
    }
    
    std::clog << "<<<<<<<<<<<< ENGINE RESET. >>>>>>>>>>>>" << std::endl;
    
    _pClock->reset();
    _pVideoSink->reset();
    _pAudioSink->reset();
    // demuxer is last node to reset, because StreamInfo belongs to it and is refered to by downstream nodes.
    _pDemuxer->reset();
    
    _isPlaying = false;
}


void
AvStreamEngine::next()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

}


void
AvStreamEngine::previous()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::pause()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::seek(int seconds)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


bool
AvStreamEngine::isSeekable()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

    // TODO: depends on http header received from server
    return false;
}


void
AvStreamEngine::savePosition()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::setSpeed(int nom, int denom)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::getPosition(float &seconds)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::getLength(float &seconds)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


void
AvStreamEngine::setVolume(int channel, float vol)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
    _pAudioSink->setVolume(vol);
//     _pAudioSink->setVolume(channel, vol);
}


void
AvStreamEngine::getVolume(int channel, float &vol)
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);
}


POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(AvStreamEngine)
POCO_END_MANIFEST