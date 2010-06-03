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
#include <Poco/RunnableAdapter.h>

#include "AlsaAudioSink.h"


AlsaAudioSink::AlsaAudioSink() :
Sink("alsa audio sink"),
_writeThread(getName() + " write thread"),
_pcmPlayback(0),
_device("default"),
_format(SND_PCM_FORMAT_S16),
_rate(44100),
_channels(2),
_periods(2),
_periodSize(8192),
_frames(_periodSize >> 2)
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
AlsaAudioSink::open(const std::string& _device)
{
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s opening with device: %s", getName(), _device));
    
    int err = snd_pcm_open(&_pcmPlayback, _device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("%s could not open device: %s", getName(), _device));
        return false;
    }
    
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s opened.", getName()));
    return true;
}


void
AlsaAudioSink::close()
{
    if (_pcmPlayback) {
        snd_pcm_drop(_pcmPlayback);
        snd_pcm_close(_pcmPlayback);
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
    snd_pcm_hw_params_alloca(&_hwParams);
    if (snd_pcm_hw_params_any(_pcmPlayback, _hwParams) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("can not configure PCM device.");
        return false;
    }
    if (snd_pcm_hw_params_set_access(_pcmPlayback, _hwParams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device access.");
        return false;
    }
    if (snd_pcm_hw_params_set_format(_pcmPlayback, _hwParams, _format) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device format.");
        return false;
    }
    if (snd_pcm_hw_params_set_rate_near(_pcmPlayback, _hwParams, &_rate, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device rate.");
        return false;
    }
    if (snd_pcm_hw_params_set_channels(_pcmPlayback, _hwParams, _channels) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device channels.");
        return false;
    }
    if (snd_pcm_hw_params_set_periods(_pcmPlayback, _hwParams, _periods, 0) < 0) {
        Omm::AvStream::Log::instance()->avstream().error("setting PCM device periods.");
        return false;
    }
    // Set buffer size (in frames). The resulting latency is given by
    // latency = periodSize * periods / (rate * bytes_per_frame)
    snd_pcm_uframes_t bufferSize = (_periodSize * _periods) >> 2;
    if (int ret = snd_pcm_hw_params_set_buffer_size_near(_pcmPlayback, _hwParams, &bufferSize)) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("setting up PCM device buffer to size: %s returns: %s",
            Poco::NumberFormatter::format(bufferSize),
            Poco::NumberFormatter::format(ret)
            ));
        return false;
    }
    if (int ret = snd_pcm_hw_params_set_period_size_near(_pcmPlayback, _hwParams, &_periodSize, 0)) {
        Omm::AvStream::Log::instance()->avstream().error(Poco::format("setting up PCM device period to size: %s returns: %s",
            Poco::NumberFormatter::format(bufferSize),
            Poco::NumberFormatter::format(ret)
            ));
        return false;
    }
    if (snd_pcm_hw_params(_pcmPlayback, _hwParams) < 0) {
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
    }
}


void
AlsaAudioSink::onTick(int64_t time)
{
    while (!_timerQuit) {
        Omm::AvStream::Frame* pFrame = _inStreams[0]->firstFrame();
        if (pFrame && pFrame->getPts() < time) {
            pFrame = _inStreams[0]->getFrame();
            
            if (pFrame) {
                Omm::AvStream::Log::instance()->avstream().warning(Poco::format("%s stream time: %s, frame %s too old, discarding frame.",
                    getName(),
                    Poco::NumberFormatter::format(time),
                    pFrame->getName()));
            }
        }
        else {
            break;
        }
    }
}


void
AlsaAudioSink::afterTimerStart()
{
    Poco::RunnableAdapter<AlsaAudioSink> ra(*this, &AlsaAudioSink::writeThread);
    _writeThread.start(ra);
}


void
AlsaAudioSink::writeThread()
{
    while (!_timerQuit)
    {
        Omm::AvStream::Frame* pFrame = _inStreams[0]->getFrame();
        if (pFrame) {
            Omm::AvStream::Log::instance()->avstream().trace(Poco::format("%s writing frame %s",
                getName(),
                pFrame->getName()));
            
            writeFrame(pFrame);
        }
    }
    close();
    Omm::AvStream::Log::instance()->avstream().debug(Poco::format("%s sink stopped.", getName()));
}


// TODO: implement asynchonous playback: http://alsa.opensrc.org/index.php/HowTo_Asynchronous_Playback
// FIXME: alsa device buffer underruns (try some simple test app and find out the difference)
void
AlsaAudioSink::writeFrame(Omm::AvStream::Frame* pFrame)
{
//     Omm::AvStream::Log::instance()->avstream().debug("write frame to ALSA PCM device");
    if (!pFrame) {
        Omm::AvStream::Log::instance()->avstream().warning("no frame to write");
        return;
    }
    
    int samplesWritten;
    while ((samplesWritten = snd_pcm_writei(_pcmPlayback, pFrame->data(), pFrame->size() >> 2)) < 0) {
        snd_pcm_prepare(_pcmPlayback);
        Omm::AvStream::Log::instance()->avstream().warning("<<<<<<<<<<<<<<< audio device buffer underrun >>>>>>>>>>>>>>>");
    }
    Omm::AvStream::Log::instance()->avstream().trace(Poco::format("%s, frame %s, samples written to device: %s.",
        getName(), pFrame->getName(), Poco::NumberFormatter::format(samplesWritten)));
}


POCO_BEGIN_MANIFEST(Omm::AvStream::Sink)
POCO_EXPORT_CLASS(AlsaAudioSink)
POCO_END_MANIFEST
