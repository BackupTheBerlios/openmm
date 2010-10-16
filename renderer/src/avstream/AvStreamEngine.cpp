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
#include <string>

#include <Poco/ClassLibrary.h>

#include "Omm/Util.h"
#include "AvStreamEngine.h"


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
    
    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        _pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream tagger plugin: " + taggerPlugin);
        return;
    }
    std::string audioPlugin("audiosink-alsa");
//     std::string audioPlugin("audiosink-sdl");
    Omm::Util::PluginLoader<Omm::AvStream::AudioSink> audioPluginLoader;
    try {
        _pAudioSink = audioPluginLoader.load(audioPlugin, "AudioSink");
    }
    catch(Poco::NotFoundException) {
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream audio plugin: " + audioPlugin);
        return;
    }
    std::string videoPlugin("videosink-sdl");
    Omm::Util::PluginLoader<Omm::AvStream::VideoSink> videoPluginLoader;
    try {
        _pVideoSink = videoPluginLoader.load(videoPlugin, "VideoSink");
    }
    catch(Poco::NotFoundException) {
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream video plugin: " + videoPlugin);
        return;
    }
    _pVideoSink->openWindow(_fullscreen, _width, _height);

    _pAudioSink->registerStreamEventObserver(new Poco::Observer<AvStreamEngine, Omm::AvStream::Sink::EndOfStream>(*this, &AvStreamEngine::endOfStream));
}


void
AvStreamEngine::destructPlayer()
{
    delete _pTagger;
    _pTagger = 0;
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
    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE SET ... >>>>>>>>>>>>");
//     Omm::AvStream::Log::instance()->avstream().debug("trying to open ifstream with url: " + mrl + " ...");
//     _fileStream.open(mrl.c_str());
//     Omm::AvStream::Log::instance()->avstream().debug("opened ifstream.");
//     _pDemuxer->set(_fileStream);
//     Omm::AvStream::Log::instance()->avstream().debug("set ifstream.");
    _pDemuxer->set(_pTagger->tag(mrl));
    /**/
    if (_pDemuxer->firstAudioStream() < 0 && _pDemuxer->firstVideoStream() < 0) {
        Omm::AvStream::Log::instance()->avstream().error("no audio or video stream found, exiting");;
        return;
    }
    
    //////////// load and attach audio Sink ////////////
    if (_pDemuxer->firstAudioStream() >= 0) {
        _pDemuxer->attach(_pAudioSink, _pDemuxer->firstAudioStream());
        _pClock->attachAudioSink(_pAudioSink);
    }
    
    /**/
    //////////// load and attach video sink ////////////
    if (_pDemuxer->firstVideoStream() >= 0) {
        _pDemuxer->attach(_pVideoSink, _pDemuxer->firstVideoStream());
        _pClock->attachVideoSink(_pVideoSink);
    }
    /**/
}


void
AvStreamEngine::load()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE START ... >>>>>>>>>>>>");
    
    _pDemuxer->start();
    /**/
//     _pClock->setStartTime(true);
    _pClock->setStartTime(false);

    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE RUN ... >>>>>>>>>>>>");

    _pClock->start();
    /**/
    _isPlaying = true;
}


void
AvStreamEngine::stop()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_actionLock);

    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE HALT. >>>>>>>>>>>>");
    
    _pDemuxer->stop();
    /**/
    _pClock->stop();
    
    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE STOP. >>>>>>>>>>>>");
    
    ////////// deallocate meta data and packet queues ////////////
    if (_pDemuxer->firstAudioStream() >= 0) {
        _pDemuxer->detach(_pDemuxer->firstAudioStream());
    }
    
    /**/
    if (_pDemuxer->firstVideoStream() >= 0) {
        _pDemuxer->detach(_pDemuxer->firstVideoStream());
    }
    /**/
    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE RESET. >>>>>>>>>>>>");
    _pClock->reset();
    
    if (_pDemuxer->firstAudioStream() >= 0) {
        _pAudioSink->reset();
    }
    
    /**/
    if (_pDemuxer->firstVideoStream() >= 0) {
        _pVideoSink->reset();
    }
    // demuxer is last node to reset, because StreamInfo belongs to it and is refered to by downstream nodes.
    /**/
    _pDemuxer->reset();
    
//     Omm::AvStream::Log::instance()->avstream().debug("trying to close ifstream ...");
//     _fileStream.close();
//     Omm::AvStream::Log::instance()->avstream().debug("deleted ifstream.");
    
    _isPlaying = false;
    Omm::AvStream::Log::instance()->avstream().debug("<<<<<<<<<<<< ENGINE OFF. >>>>>>>>>>>>");
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


void
AvStreamEngine::endOfStream(Omm::AvStream::Sink::EndOfStream* eof)
{
    stop();
    endOfStream();
}


POCO_BEGIN_MANIFEST(Omm::Av::Engine)
POCO_EXPORT_CLASS(AvStreamEngine)
POCO_END_MANIFEST