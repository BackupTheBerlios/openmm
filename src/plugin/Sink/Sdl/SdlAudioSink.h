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
#ifndef SdlAudioSink_INCLUDED
#define SdlAudioSink_INCLUDED

#include <Omm/AvStream.h>


class SdlAudioSink : public Omm::AvStream::AudioSink
{
public:
    SdlAudioSink();
    virtual ~SdlAudioSink();

private:
    virtual bool initDevice();
    virtual bool closeDevice();
    virtual void startPresentation();
    virtual void stopPresentation();
    virtual void setVolume(int channel, float vol);
};

#endif
