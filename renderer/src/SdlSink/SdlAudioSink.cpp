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
AudioSink("sdl audio sink")
{
}


SdlAudioSink::~SdlAudioSink()
{
}


void audioCallback(void* pThis, uint8_t* sdlBuffer, int sdlBufferSize)
{
    SdlAudioSink* pSdlAudioSink = static_cast<SdlAudioSink*>(pThis);
    if (!pSdlAudioSink) {
        Omm::AvStream::Log::instance()->avstream().error("sdl audio sink, audio callback, this is null");
        return;
    }
    
    Omm::AvStream::Log::instance()->avstream().trace(Poco::format("sdl audio sink, audio callback, sdl buffer size: %s",
        Poco::NumberFormatter::format(sdlBufferSize)));
    
    pSdlAudioSink->initSilence((char*)sdlBuffer, sdlBufferSize);
    // if no samples are available, don't block and leave the silence in the pcm buffer
    if (pSdlAudioSink->audioAvailable()) {
        int bytesRead = pSdlAudioSink->audioRead((char*)sdlBuffer, sdlBufferSize);
        Omm::AvStream::Log::instance()->avstream().trace(Poco::format("sdl audio sink, audio callback, bytes read: %s",
            Poco::NumberFormatter::format(bytesRead)));
    }
}


bool
SdlAudioSink::initAudio()
{
    Omm::AvStream::Log::instance()->avstream().debug("opening SDL audio sink ...");
    
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 ) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("failed to init SDL:  %s", std::string(SDL_GetError())));
        return false;
    }
    
    SDL_AudioSpec deviceParamsWanted;
    SDL_AudioSpec deviceParams;
    deviceParamsWanted.freq = sampleRate();
    deviceParamsWanted.format = AUDIO_S16SYS;
    deviceParamsWanted.channels = channels();
    deviceParamsWanted.silence = silence();
    deviceParamsWanted.samples = 1024;
    deviceParamsWanted.callback = audioCallback;
    deviceParamsWanted.userdata = this;
    
    if(SDL_OpenAudio(&deviceParamsWanted, &deviceParams) < 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("could not open SDL audio device: %s", std::string(SDL_GetError())));
        return false;
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("SDL audio sink opened.");
    return true;
}


void
SdlAudioSink::startAudio()
{
    SDL_PauseAudio(0);
}


void
SdlAudioSink::stopAudio()
{
    SDL_PauseAudio(1);
}


POCO_BEGIN_MANIFEST(Omm::AvStream::AudioSink)
POCO_EXPORT_CLASS(SdlAudioSink)
POCO_END_MANIFEST
