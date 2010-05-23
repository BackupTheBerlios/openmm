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
_frameCount(0)
{
    // audio sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams[0]->setInfo(0);
    _inStreams[0]->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


SdlAudioSink::~SdlAudioSink()
{
}


void audioCallback(void* sinkNode, uint8_t* sdlBuffer, int sdlBufferSize)
{
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, audioCallback");
    
    Omm::AvStream::Sink* pSdlAudioSink = (Omm::AvStream::Sink*)sinkNode;
    Omm::AvStream::Frame* pFrame;
    if (!pSdlAudioSink->doStop() && (pFrame = pSdlAudioSink->getInStream(0)->getFrame()))
    {
        Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink, audioCallback processing frame");
        
//         pFrame->data();
//         pFrame->size();
//         uint8_t buffer[];
        int bufferSize = (pFrame->size() > sdlBufferSize ? sdlBufferSize : pFrame->size());
        memcpy(sdlBuffer, (uint8_t*)pFrame->data(), bufferSize);
    }
    else {
        // How to stop that? Or better generate silence?
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
    _deviceParamsWanted.userdata = this;
    
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
    
    SDL_PauseAudio(0);
    
    // TODO: do we need to block here? I think, no ...
    Poco::Thread::sleep(5000);
    
    Omm::AvStream::Log::instance()->avstream().debug("sdl audio sink finished.");
}



POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(SdlAudioSink)
POCO_END_MANIFEST
