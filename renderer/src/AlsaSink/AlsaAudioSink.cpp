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

#include "AlsaAudioSink.h"


AlsaAudioSink::AlsaAudioSink() :
Sink("alsa audio sink"),
pcm_playback(0),
device("default"),
format(SND_PCM_FORMAT_S16),
rate(44100),
channels(2),
periods(2),
periodsize(8192),
frames(periodsize >> 2)
{
    // audio sink has one input stream
    _inStreams.push_back(new Omm::AvStream::Stream(this));
    _inStreams[0]->setInfo(0);
    _inStreams[0]->setQueue(new Omm::AvStream::StreamQueue(this));
    
    // and no output stream
}


AlsaAudioSink::~AlsaAudioSink()
{
}


bool
AlsaAudioSink::open()
{
    return open("default");
}


bool
AlsaAudioSink::open(const std::string& device)
{
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s opening with device: %s", getName(), device));
    
    int err = snd_pcm_open(&pcm_playback, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("%s could not open device: %s", getName(), device));
        return false;
    }
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s opened.", getName()));
    return true;
}


void
AlsaAudioSink::close()
{
    if (pcm_playback) {
        snd_pcm_drop(pcm_playback);
        snd_pcm_close(pcm_playback);
    }
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s closed.", getName()));
}


bool
AlsaAudioSink::init()
{
    if (!open()) {
        Omm::AvStream::Log::instance()->avstream().error("can not open ALSA PCM device.");
        return false;
    }
    snd_pcm_hw_params_alloca(&hw);
    if (snd_pcm_hw_params_any(pcm_playback, hw) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("can not configure PCM device.");
        return false;
    }
    if (snd_pcm_hw_params_set_access(pcm_playback, hw, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device access.");
        return false;
    }
    if (snd_pcm_hw_params_set_format(pcm_playback, hw, format) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device format.");
        return false;
    }
    if (snd_pcm_hw_params_set_rate_near(pcm_playback, hw, &rate, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device rate.");
        return false;
    }
    if (snd_pcm_hw_params_set_channels(pcm_playback, hw, channels) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device channels.");
        return false;
    }
    if (snd_pcm_hw_params_set_periods(pcm_playback, hw, periods, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device periods.");
        return false;
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
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("%s initializing device.", getName()));
        return false;
    }
    
    if (!_inStreams[0]->getInfo()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream info not allocated", getName()));
        return false;
    }
    if (!_inStreams[0]->getInfo()->isAudio()) {
        Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s init failed, input stream is not a audio stream", getName()));
        return false;
    }
    
    return true;
}


void
AlsaAudioSink::run()
{
    if (!_inStreams[0]->getQueue()) {
        Omm::AvStream::Log::instance()->avstream().warning("no in stream attached to audio sink, stopping.");
        return;
    }
    
    int frameCount = 0;
    Omm::AvStream::Frame* pFrame;
    while (!_quit && (pFrame = _inStreams[0]->getFrame()))
    {
        Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s processing frame #%s",
            getName(), Poco::NumberFormatter::format0(++frameCount, 3)));
        
        writeFrame(pFrame);
    }
    
    close();
    Omm::AvStream::Log::instance()->avstream().debug("video sink finished.");
}

void
AlsaAudioSink::writeFrame(Omm::AvStream::Frame* pFrame)
{
    Omm::AvStream::Log::instance()->avstream().debug("write frame to ALSA PCM device");
    if (!pFrame) {
        Omm::AvStream::Log::instance()->avstream().warning("no frame to write");
        return;
    }
    
    int framesWritten;
    while ((framesWritten = snd_pcm_writei(pcm_playback, pFrame->data(), pFrame->size() >> 2)) < 0) {
        snd_pcm_prepare(pcm_playback);
        Omm::AvStream::Log::instance()->avstream().warning("<<<<<<<<<<<<<<< audio device buffer underrun >>>>>>>>>>>>>>>");
    }
//     std::clog << "frames written: " << framesWritten << std::endl;
}


POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(AlsaAudioSink)
POCO_END_MANIFEST
