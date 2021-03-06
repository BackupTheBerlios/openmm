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
#ifndef SdlVideoSink_INCLUDED
#define SdlVideoSink_INCLUDED

#include <SDL.h>

#include <Omm/AvStream.h>


class SdlOverlay : public Omm::AvStream::Overlay
{
public:
    SdlOverlay(Omm::AvStream::VideoSink* pVideoSink, int width, int height, SDL_Surface* sdlScreen);
    ~SdlOverlay();
    
    SDL_Overlay*    _pSDLOverlay;
    SDL_Surface*    _pSdlScreen;
};


class SdlVideoSink : public Omm::AvStream::VideoSink
{
public:
    SdlVideoSink();
    virtual ~SdlVideoSink();
    
private:
    virtual void openWindow(bool fullScreen = false, int width = 720, int height = 576);
    virtual bool initDevice();
    virtual bool closeDevice();
    virtual void displayFrame(Omm::AvStream::Overlay* pOverlay);
    
    virtual int displayWidth();
    virtual int displayHeight();
    
    virtual void blankDisplay();
    
    SDL_Surface* _pSdlScreen;
};

#endif
