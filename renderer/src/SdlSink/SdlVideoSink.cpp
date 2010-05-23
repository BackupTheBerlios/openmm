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
#include <Poco/Thread.h>
#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>

#include "SdlVideoSink.h"


SdlVideoSink::SdlVideoSink() :
Sink("sdl video sink"),
_pOverlay(new Omm::AvStream::Overlay)
{
    // video sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams.back()->setInfo(0);
    _inStreams.back()->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


SdlVideoSink::~SdlVideoSink()
{
    delete _pOverlay;
}


bool
SdlVideoSink::init()
{
    Omm::AvStream::Log::instance()->avstream().debug("opening SDL video sink ...");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("failed to init SDL:  %s", std::string(SDL_GetError())));
        return false;
    }
    
    _pSdlScreen = SDL_SetVideoMode(720, 576, 0, SDL_HWSURFACE|SDL_RESIZABLE|SDL_ASYNCBLIT|SDL_HWACCEL);
    if (_pSdlScreen == 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("could not open SDL window: %s", std::string(SDL_GetError())));
        return false;
    }
    
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->findCodec()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, could not find codec", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->isVideo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream is not a video stream", getName()));
        return false;
    }
    
    _pSdlOverlay = SDL_CreateYUVOverlay(720, 576, SDL_YV12_OVERLAY, _pSdlScreen);
    _pOverlay->_data[0] = _pSdlOverlay->pixels[0];
    _pOverlay->_data[1] = _pSdlOverlay->pixels[2];
    _pOverlay->_data[2] = _pSdlOverlay->pixels[1];
    
    _pOverlay->_pitch[0] = _pSdlOverlay->pitches[0];
    _pOverlay->_pitch[1] = _pSdlOverlay->pitches[2];
    _pOverlay->_pitch[2] = _pSdlOverlay->pitches[1];

    Omm::AvStream::Log::instance()->avstream().debug("SDL video sink opened.");
    return true;
}


void
SdlVideoSink::run()
{
    if (!_inStreams[0]->getQueue()) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to video sink, stopping.");
        return;
    }
    
    int frameCount = 0;
    Omm::AvStream::Frame* pFrame;
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s processing frame #%s",
            getName(), Poco::NumberFormatter::format0(++frameCount, 3)));
        
        Omm::AvStream::Frame* pDecodedFrame = pFrame->decode();
        if (!pDecodedFrame) {
            Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s decoding failed, discarding packet", getName()));
        }
        else {
            pDecodedFrame->write(_pOverlay);
        }
        presentFrame();
    }

    Omm::AvStream::Log::instance()->avstream().debug("video sink finished.");
}


// void
// SdlSinkPlugin::close()
// {
//     Omm::AvStream::Log::instance()->avstream().debug("SDL video sink closed.");
// }


// void
// SdlVideoSink::writeFrame(Omm::AvStream::Frame* pFrame)
// {
//     Omm::AvStream::Log::instance()->avstream().debug("write frame to SDL video overlay");
//     _pCurrentFrame = pFrame;
//     pFrame->write(_pOverlay);
// }


void
SdlVideoSink::presentFrame()
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = 720;
    rect.h = 576;
    // FIXME: lock access to Stream::height() and Stream::width()
//     rect.w = _pCurrentFrame->getStream()->width();
//     rect.h = _pCurrentFrame->getStream()->height();
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("SDL video Sink::present() frame width: %s, height: %s",
        Poco::NumberFormatter::format(rect.w),
        Poco::NumberFormatter::format(rect.h)));
    
    SDL_DisplayYUVOverlay(_pSdlOverlay, &rect);
}


int
SdlVideoSink::eventLoop()
{
    Omm::AvStream::Log::instance()->avstream().debug("event loop ...");
//     Poco::Thread::sleep(10000);
    
}



POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(SdlVideoSink)
POCO_END_MANIFEST
