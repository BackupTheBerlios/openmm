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

#include "SdlAudioSink.h"


SdlAudioSink::SdlAudioSink() :
Sink("sdl audio sink"),
// allocate byte queue for 5k s16-2chan-samples
_byteQueue(5 * 1024 * 2 * 2)
{
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, constructor");
    
    // audio sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams[0]->setInfo(0);
    _inStreams[0]->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
    
    // allocate byte queue for 5k s16-2chan-samples
//     _pByteQueue = new Omm::AvStream::ByteQueue(5 * 1024 * 2 * 2);
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, constructor finished");
}


SdlAudioSink::~SdlAudioSink()
{
}


void audioCallback(void* pQueue, uint8_t* sdlBuffer, int sdlBufferSize)
{
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, audio callback");
    
    Omm::AvStream::ByteQueue* pByteQueue = static_cast<Omm::AvStream::ByteQueue*>(pQueue);
    if (!pByteQueue) {
        Omm::AvStream::Log::instance()->avstream().error("sdl audio sink, audio callback, byte stream is null");
        return;
    }
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("sdl audio sink, audio callback, sdl buffer size: %s",
        Poco::NumberFormatter::format(sdlBufferSize)/*, Poco::NumberFormatter::format(pFrame->size())*/));
    
    // initialize pcm buffer with silence
    memset(sdlBuffer, 0, sdlBufferSize);
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, audio callback, init pcm buffer with silence");
    // if no samples are available, don't block and leave the silence in the pcm buffer
    if (pByteQueue->level()) {
        Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, audio callback, try to read bytes from byte queue ...");
        int bytesRead = pByteQueue->readSome((char*)sdlBuffer, sdlBufferSize);
        Omm::AvStream::Log::instance()->avstream().debug(Poco::format("sdl audio sink, audio callback, bytes read: %s",
            Poco::NumberFormatter::format(bytesRead)));
    }
}


bool
SdlAudioSink::init()
{
    Omm::AvStream::Log::instance()->avstream().debug("opening SDL audio sink ...");
    
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 ) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("failed to init SDL:  %s", std::string(SDL_GetError())));
        return false;
    }
    
    
    // audio device parameters
    _deviceParamsWanted.freq = _inStreams[0]->getInfo()->sampleRate();
    _deviceParamsWanted.format = AUDIO_S16SYS;
    _deviceParamsWanted.channels = _inStreams[0]->getInfo()->channels();
    _deviceParamsWanted.silence = 0;
    _deviceParamsWanted.samples = 1024;
    _deviceParamsWanted.callback = audioCallback;
    _deviceParamsWanted.userdata = &_byteQueue;
//     _deviceParamsWanted.userdata = _pByteQueue;
    
    if(SDL_OpenAudio(&_deviceParamsWanted, &_deviceParams) < 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("could not open SDL audio device: %s", std::string(SDL_GetError())));
        return false;
    }
    
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->isAudio()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream is not a audio stream", getName()));
        return false;
    }

    
    Omm::AvStream::Log::instance()->avstream().debug("SDL audio sink opened.");
    return true;
}


void
SdlAudioSink::run()
{
    if (!_inStreams[0]->getQueue()) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to audio sink, stopping.");
        return;
    }
    Omm::AvStream::Frame* pFrame;
    while (!_quit && (pFrame = _inStreams[0]->getFrame())) {
        _byteQueue.write(pFrame->data(), pFrame->size());
//         _pByteQueue->write(pFrame->data(), pFrame->size());
    }
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s finished.", getName()));
}


void
SdlAudioSink::onTick(int64_t time)
{
//     while (!_timerQuit) {
//         Omm::AvStream::Frame* pFrame = _inStreams[0]->firstFrame();
//         if (pFrame && pFrame->getPts() < time) {
//             pFrame = _inStreams[0]->getFrame();
//             
//             if (pFrame) {
//                 Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s stream time: %s, frame %s too old, discarding frame.",
//                     getName(),
//                     Poco::NumberFormatter::format(time),
//                     pFrame->getName()));
//             }
//         }
//         else {
//             break;
//         }
//     }
}


void
SdlAudioSink::afterTimerStart()
{
    SDL_PauseAudio(0);
}



POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(SdlAudioSink)
POCO_END_MANIFEST
