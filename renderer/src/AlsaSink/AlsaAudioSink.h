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
#ifndef AlsaAudioSink_INCLUDED
#define AlsaAudioSink_INCLUDED

#include <Omm/AvStream.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

class AlsaAudioSink : public Omm::AvStream::Sink
{
public:
    AlsaAudioSink();
    virtual ~AlsaAudioSink();

private:
    virtual bool init();
    virtual void run();
    
    virtual bool open();
    bool open(const std::string& device);
    virtual void close();
    
    // Writes blocking
    virtual void writeFrame(Omm::AvStream::Frame *pFrame);

    snd_pcm_t*              pcm_playback;
    snd_pcm_hw_params_t     *hw;
    std::string             device;
    snd_pcm_format_t        format;
    unsigned int            rate;
    int                     channels;
    int                     periods;       // number of periods
    snd_pcm_uframes_t       periodsize;    // periodsize (bytes)
    snd_pcm_uframes_t       frames;
};

#endif
