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

#include <SDL/SDL.h>

#include <Omm/AvStream.h>


class SdlVideoSink : public Omm::AvStream::Sink
{
public:
    SdlVideoSink();
    virtual ~SdlVideoSink();
    
//     virtual void open();
//     virtual void close();
    
//     virtual void writeFrame(Omm::AvStream::Frame *pFrame);
    
    virtual int eventLoop();
    
private:
    virtual bool init();
    virtual void run();
    
    virtual void presentFrame();
    
    SDL_Surface*                  _pSdlScreen;
    SDL_Overlay*                  _pSdlOverlay;
    
    Omm::AvStream::Overlay*       _pOverlay;
};

#endif
