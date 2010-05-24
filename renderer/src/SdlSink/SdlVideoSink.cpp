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


SdlOverlay::SdlOverlay(Omm::AvStream::Sink* pSink) :
Overlay(pSink)
{
}


SdlVideoSink::SdlVideoSink() :
// reserve 5 overlays for SdlVideoSink
Sink("sdl video sink", 720, 576, PIX_FMT_YUV420P, 5),
_writeOverlayNumber(0)
{
    Omm::AvStream::Log::instance()->avstream().debug("SdlVideoSink().");
    
    // video sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams.back()->setInfo(0);
    _inStreams.back()->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


SdlVideoSink::~SdlVideoSink()
{
}


bool
SdlVideoSink::init()
{
    Omm::AvStream::Log::instance()->avstream().debug("opening SDL video sink ...");
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("failed to init SDL:  %s", std::string(SDL_GetError())));
        return false;
    }
    
    SDL_Surface* pSdlScreen = SDL_SetVideoMode(getWidth(), getHeight(), 0, SDL_HWSURFACE | SDL_RESIZABLE | SDL_ASYNCBLIT | SDL_HWACCEL);
    
    if (pSdlScreen == 0) {
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
    
    for (int numOverlay = 0; numOverlay < _overlayCount; numOverlay++) {
        // SDL_YV12_OVERLAY corresponds to ffmpeg::PixelFormat == PIX_FMT_YUV420P
        // TODO: catch, if SDL_Overlay could not be created (video card has to few memory)
        SDL_Overlay* pSDLOverlay = SDL_CreateYUVOverlay(getWidth(), getHeight(), SDL_YV12_OVERLAY, pSdlScreen);
        SdlOverlay* pOverlay = new SdlOverlay(this);
        
        pOverlay->_pSDLOverlay = pSDLOverlay;
        
        pOverlay->_data[0] = pSDLOverlay->pixels[0];
        pOverlay->_data[1] = pSDLOverlay->pixels[2];
        pOverlay->_data[2] = pSDLOverlay->pixels[1];
        
        pOverlay->_pitch[0] = pSDLOverlay->pitches[0];
        pOverlay->_pitch[1] = pSDLOverlay->pitches[2];
        pOverlay->_pitch[2] = pSDLOverlay->pitches[1];
        
        _overlayVector.push_back(pOverlay);
//         _overlayVector[numOverlay] = pOverlay;
    }

    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s opened.", getName()));
    return true;
}


void
SdlVideoSink::run()
{
    if (!_inStreams[0]->getQueue()) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to video sink, stopping.");
        return;
    }
    
    Omm::AvStream::Clock::instance()->attachSink(this, 40);
    startTimer();
    
    Omm::AvStream::Frame* pFrame;
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s decoding frame %s",
            getName(), pFrame->getName()));
        
        Omm::AvStream::Frame* pDecodedFrame = pFrame->decode();
        if (!pDecodedFrame) {
            Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s decoding failed, discarding frame %s",
                getName(), pFrame->getName()));
        }
        else {
            SdlOverlay* pWriteOverlay = static_cast<SdlOverlay*>(_overlayVector[_writeOverlayNumber]);
            pDecodedFrame->write(pWriteOverlay);
            pWriteOverlay->_pFrame = pDecodedFrame;
            
            _overlayQueue.put(pWriteOverlay);
            // increment modulo _overlayCount
            _writeOverlayNumber++;
            if (_writeOverlayNumber >= _overlayCount) {
                _writeOverlayNumber = 0;
            }
        }
    }

    stopTimer();
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s finished.", getName()));
}


void
SdlVideoSink::onTick()
{
    Omm::AvStream::Log::instance()->avstream().trace(Poco::format("%s on tick.", getName()));
    
    Omm::AvStream::Overlay* pOverlay = _overlayQueue.get();
    if (pOverlay) {
        SdlOverlay* pSdlOverlay = static_cast<SdlOverlay*>(pOverlay);
        displayFrame(pSdlOverlay);
    }
}


void
SdlVideoSink::displayFrame(SdlOverlay* pOverlay)
{
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = getWidth();
    rect.h = getHeight();
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s display frame %s, width: %s, height: %s",
        getName(),
        pOverlay->_pFrame->getName(),
        Poco::NumberFormatter::format(rect.w),
        Poco::NumberFormatter::format(rect.h)));
    
    SDL_DisplayYUVOverlay(pOverlay->_pSDLOverlay, &rect);
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
