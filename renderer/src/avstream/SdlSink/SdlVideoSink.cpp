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


SdlOverlay::SdlOverlay(Omm::AvStream::VideoSink* pVideoSink, int width, int height, SDL_Surface* sdlScreen) :
Overlay(pVideoSink, width, height),
_pSdlScreen(sdlScreen)
{
    // SDL_YV12_OVERLAY corresponds to ffmpeg::PixelFormat == PIX_FMT_YUV420P
    // TODO: catch, if SDL_Overlay could not be created (video card has to few memory)
    _pSDLOverlay = SDL_CreateYUVOverlay(_width, _height, SDL_YV12_OVERLAY, sdlScreen);
    _data[0] = _pSDLOverlay->pixels[0];
    _data[1] = _pSDLOverlay->pixels[2];
    _data[2] = _pSDLOverlay->pixels[1];
    
    _pitch[0] = _pSDLOverlay->pitches[0];
    _pitch[1] = _pSDLOverlay->pitches[2];
    _pitch[2] = _pSDLOverlay->pitches[1];
}


SdlOverlay::~SdlOverlay()
{
    if (_pSDLOverlay) {
        SDL_FreeYUVOverlay(_pSDLOverlay);
        _pSDLOverlay = 0;
    }
    _data[0] = 0;
    _data[1] = 0;
    _data[2] = 0;
    _pitch[0] = 0;
    _pitch[1] = 0;
    _pitch[2] = 0;
}


SdlVideoSink::SdlVideoSink() :
// reserve 3 overlays for SdlVideoSink
VideoSink("sdl video sink", 720, 576, Omm::AvStream::Meta::CC_YUV420P, 3)
{
}


SdlVideoSink::~SdlVideoSink()
{
    SDL_Quit();
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s closed.", getName()));
}


void
SdlVideoSink::openWindow(bool fullScreen, int width, int height)
{
    Omm::AvStream::Log::instance()->avstream().debug("SDL video sink opening window ...");
    
    _fullScreen = fullScreen;
    _width = width;
    _height = height;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        Omm::AvStream::Log::instance()->avstream().error("failed to init SDL: " + std::string(SDL_GetError()));
    }
    
    int flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;
    if (_fullScreen) {
        flags |= SDL_FULLSCREEN;
    }
    else {
        flags |= SDL_RESIZABLE;
    }
    
    int bitsPerPixel = 0;  // 0 = current display bits per pixel
#ifdef __DARWIN__
    bitsPerPixel = 24;
#endif
    
    _pSdlScreen = SDL_SetVideoMode(getWidth(), getHeight(), bitsPerPixel, flags);
    
    SDL_ShowCursor(SDL_DISABLE);
    
    if (_pSdlScreen == 0) {
        Omm::AvStream::Log::instance()->avstream().error("could not open SDL window: " + std::string(SDL_GetError()));
        return;
    }
    Omm::AvStream::Log::instance()->avstream().debug("SDL video sink window opened.");
}


bool
SdlVideoSink::initDevice()
{
    for (int numOverlay = 0; numOverlay < _overlayCount; numOverlay++) {
        SdlOverlay* pOverlay = new SdlOverlay(this, getInStream(0)->getInfo()->width(), getInStream(0)->getInfo()->height(), _pSdlScreen);
        _overlayVector[numOverlay] = pOverlay;
    }
    return true;
}


bool
SdlVideoSink::closeDevice()
{
    Omm::AvStream::Log::instance()->avstream().debug("closing SDL video sink ...");
    for (int numOverlay = 0; numOverlay < _overlayCount; numOverlay++) {
        Omm::AvStream::Log::instance()->avstream().debug("SDL video sink deleting overlay ...");
        delete static_cast<SdlOverlay*>(_overlayVector[numOverlay]);
        Omm::AvStream::Log::instance()->avstream().debug("SDL video sink overlay deleted.");
    }
    Omm::AvStream::Log::instance()->avstream().debug("SDL video sink closed.");
    return true;
}


void
SdlVideoSink::displayFrame(Omm::AvStream::Overlay* pOverlay)
{
    SDL_Rect rect;
    int x, y, w, h;
    displayRect(x, y, w, h);
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    Omm::AvStream::Log::instance()->avstream().debug("sdl video sink display overlay frame");
    SDL_DisplayYUVOverlay(static_cast<SdlOverlay*>(pOverlay)->_pSDLOverlay, &rect);
}


int
SdlVideoSink::displayWidth()
{
    return _pSdlScreen->w;
}


int
SdlVideoSink::displayHeight()
{
    return _pSdlScreen->h;
}


void
SdlVideoSink::blankDisplay()
{
    Omm::AvStream::Log::instance()->avstream().debug("clear display");
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = displayWidth();
    rect.h = displayHeight();
    SDL_FillRect(_pSdlScreen, &rect, SDL_MapRGB(_pSdlScreen->format, 0x00, 0x00, 0x00));
    SDL_UpdateRect(_pSdlScreen, 0, 0, 0, 0);
}



POCO_BEGIN_MANIFEST(Omm::AvStream::VideoSink)
POCO_EXPORT_CLASS(SdlVideoSink)
POCO_END_MANIFEST
