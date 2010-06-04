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
#ifndef PcmAudioSink_INCLUDED
#define PcmAudioSink_INCLUDED

#include <fstream>

#include <Omm/AvStream.h>


class PcmAudioSink : public Omm::AvStream::AudioSink
{
public:
    PcmAudioSink();
    virtual ~PcmAudioSink();
    
//     virtual int eventLoop();
    
private:
    virtual bool initDevice();
    virtual void startPresentation();
//     virtual void stopPresentation();
    
//     virtual bool init();
//     virtual void run();

    std::ofstream           _pcmStream;
    int                     _bufferSize;
    char*                   _buffer;
};

#endif
