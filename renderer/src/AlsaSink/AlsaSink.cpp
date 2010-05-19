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
#include <Poco/Format.h>
#include <Poco/NumberFormatter.h>

#include "AlsaSink.h"


AlsaSinkPlugin::AlsaSinkPlugin() :
Sink(true),
pcm_playback(0),
device("default"),
format(SND_PCM_FORMAT_S16),
rate(44100),
channels(2),
periods(2),
periodsize(8192),
buffer(new char[periodsize]),
bufferPos(buffer),
frames(periodsize >> 2)
{
}


AlsaSinkPlugin::~AlsaSinkPlugin()
{
    delete buffer;
}


void
AlsaSinkPlugin::open()
{
    open("default");
}


void
AlsaSinkPlugin::open(const std::string& device)
{
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("opening ALSA audio sink with device: %s", device));
    
    int err = snd_pcm_open(&pcm_playback, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("could not open alsa device: %s", device));
        return;
    }
    
    initDevice();
    
    Omm::AvStream::Log::instance()->avstream().debug("ALSA audio sink opened.");
}


void
AlsaSinkPlugin::close()
{
    if (pcm_playback) {
        snd_pcm_drop(pcm_playback);
        snd_pcm_close(pcm_playback);
    }
    
    Omm::AvStream::Log::instance()->avstream().debug("ALSA audio sink closed.");
}


void
AlsaSinkPlugin::initDevice()
{
    snd_pcm_hw_params_alloca(&hw);
    if (snd_pcm_hw_params_any(pcm_playback, hw) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("can not configure PCM device.");
        return;
    }
    if (snd_pcm_hw_params_set_access(pcm_playback, hw, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device access.");
        return;
    }
    if (snd_pcm_hw_params_set_format(pcm_playback, hw, format) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device format.");
        return;
    }
    if (snd_pcm_hw_params_set_rate_near(pcm_playback, hw, &rate, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device rate.");
        return;
    }
    if (snd_pcm_hw_params_set_channels(pcm_playback, hw, channels) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device channels.");
        return;
    }
    if (snd_pcm_hw_params_set_periods(pcm_playback, hw, periods, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device periods.");
        return;
    }
    // Set buffer size (in frames). The resulting latency is given by
    // latency = periodsize * periods / (rate * bytes_per_frame)
    snd_pcm_uframes_t bufferSize = (periodsize * periods) >> 2;
    if (int ret = snd_pcm_hw_params_set_buffer_size(pcm_playback, hw, bufferSize)) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("setting up PCM device buffer to size: %s returns: %s",
            Poco::NumberFormatter::format(bufferSize),
            Poco::NumberFormatter::format(ret)
            ));
    }
    if (snd_pcm_hw_params(pcm_playback, hw) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("initializing alsa device.");
        return;
    }
}


void
AlsaSinkPlugin::writeFrame(Omm::AvStream::Frame* pFrame)
{
    Omm::AvStream::Log::instance()->avstream().debug("write frame to ALSA PCM device");
    if (!pFrame) {
        Omm::AvStream::Log::instance()->avstream().error("no frame to write");
        return;
    }
    
    int framesWritten;
    while ((framesWritten = snd_pcm_writei(pcm_playback, pFrame->data(), pFrame->size() >> 2)) < 0) {
        snd_pcm_prepare(pcm_playback);
        Omm::AvStream::Log::instance()->avstream().error("<<<<<<<<<<<<<<< audio device buffer underrun >>>>>>>>>>>>>>>");
    }
//     std::clog << "frames written: " << framesWritten << std::endl;
}


// void AlsaSinkPlugin::pause()
// {
//     if (d->error) return;
//     
//     if (d->can_pause) {
//         snd_pcm_pause(d->pcm_playback, 1);
//     }
//     
// }
// 
// // Do not confuse this with snd_pcm_resume which is used to resume from a suspend
// void AlsaSinkPlugin::resume()
// {
//     if (d->error) return;
//     
//     if (snd_pcm_state( d->pcm_playback ) == SND_PCM_STATE_PAUSED)
//         snd_pcm_pause(d->pcm_playback, 0);
// }

// static int resume(snd_pcm_t *pcm)
// {
//     int res;
//     while ((res = snd_pcm_resume(pcm)) == -EAGAIN)
//         sleep(1);
//     if (! res)
//         return 0;
//     return snd_pcm_prepare(pcm);
// }


// int AlsaSinkPlugin::latency()
// {
//     if (d->error || !d->initialized || d->config.sample_rate == 0) return 0;
//     
//     snd_pcm_sframes_t frames;
//     
//     snd_pcm_delay(d->pcm_playback, &frames);
//     
//     if (snd_pcm_state( d->pcm_playback ) != SND_PCM_STATE_RUNNING)
//         return 0;
//     
//     // delay in ms after normal rounding
//     int sample_rate = d->config.sample_rate;
//     long div = (frames / sample_rate) * 1000;
//     long rem = (frames % sample_rate) * 1000;
//     
//     return div + rem / sample_rate;
// }

POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(AlsaSinkPlugin)
POCO_END_MANIFEST
