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
AudioSink("alsa audio sink"),
_writeThread(getName() + " write thread"),
_writeThreadRunnable(*this, &AlsaAudioSink::writeThread),
_quitWriteThread(false),
_pcmPlayback(0),
_device("default"),
_format(SND_PCM_FORMAT_S16),
_rate(48000),
_channels(2),
_periods(2),
_startPeriodSize(8192),
_periodSize(_startPeriodSize),
_bufferSize(0),
_buffer(0)
{
    if (!open()) {
        LOGNS(Omm::AvStream, avstream, error, "can not open ALSA PCM device.");
    }
}


AlsaAudioSink::~AlsaAudioSink()
{
    close();
    delete _buffer;
}


bool
AlsaAudioSink::open()
{
    return open("default");
}


bool
AlsaAudioSink::open(const std::string& _device)
{
    LOGNS(Omm::AvStream, avstream, debug, Poco::format("%s opening with device: %s", getName(), _device));

    int err = snd_pcm_open(&_pcmPlayback, _device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        LOGNS(Omm::AvStream, avstream, error, Poco::format("%s could not open device: %s", getName(), _device));
        return false;
    }

    LOGNS(Omm::AvStream, avstream, debug, Poco::format("%s opened.", getName()));
    return true;
}


void
AlsaAudioSink::close()
{
    if (_pcmPlayback) {
        snd_pcm_close(_pcmPlayback);
    }

    LOGNS(Omm::AvStream, avstream, debug, Poco::format("%s closed.", getName()));
}


bool
AlsaAudioSink::initDevice()
{
    snd_pcm_hw_params_alloca(&_hwParams);
    if (snd_pcm_hw_params_any(_pcmPlayback, _hwParams) < 0) {
        LOGNS(Omm::AvStream, avstream, error, "can not configure PCM device.");
        return false;
    }
    if (snd_pcm_hw_params_set_access(_pcmPlayback, _hwParams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        LOGNS(Omm::AvStream, avstream, error, "setting PCM device access.");
        return false;
    }
    if (snd_pcm_hw_params_set_format(_pcmPlayback, _hwParams, _format) < 0) {
        LOGNS(Omm::AvStream, avstream, error, "setting PCM device format.");
        return false;
    }
    _rate = sampleRate();
    if (snd_pcm_hw_params_set_rate_near(_pcmPlayback, _hwParams, &_rate, 0) < 0) {
        LOGNS(Omm::AvStream, avstream, error, "setting PCM device rate to: " + Poco::NumberFormatter::format(_rate));
        return false;
    }
    _channels = channels();
    if (snd_pcm_hw_params_set_channels(_pcmPlayback, _hwParams, _channels) < 0) {
        LOGNS(Omm::AvStream, avstream, error, "setting PCM device channels to: " + Poco::NumberFormatter::format(_channels));
        return false;
    }
//     if (snd_pcm_hw_params_set_periods(_pcmPlayback, _hwParams, _periods, 0) < 0) {
//         LOGNS(Omm::AvStream, avstream, error, "setting PCM device periods.");
//         return false;
//     }
    // Set buffer size (in frames). The resulting latency is given by
    // latency = periodSize * periods / (rate * bytes_per_frame)
//     snd_pcm_uframes_t bufferSizeInFrames = (_startPeriodSize * _periods) >> 2;
    snd_pcm_uframes_t bufferSizeInFrames = frameCount(_startPeriodSize * _channels);
    if (int ret = snd_pcm_hw_params_set_buffer_size_near(_pcmPlayback, _hwParams, &bufferSizeInFrames)) {
        LOGNS(Omm::AvStream, avstream, error, getName() + " setting up PCM device buffer to size: " + Poco::NumberFormatter::format(_bufferSize) + " returns: " + Poco::NumberFormatter::format(ret));
        return false;
    }
    if (_buffer) {
        delete _buffer;
    }
    _bufferSize = byteCount(bufferSizeInFrames);
    _buffer = new char[_bufferSize];
    LOGNS(Omm::AvStream, avstream, debug, getName() + " setting up PCM device buffer to number of frames: " + Poco::NumberFormatter::format(bufferSizeInFrames) + ", audio read buffer size in bytes is: " + Poco::NumberFormatter::format(_bufferSize));

    _periodSize = _startPeriodSize;
    if (int ret = snd_pcm_hw_params_set_period_size_near(_pcmPlayback, _hwParams, &_periodSize, 0)) {
        LOGNS(Omm::AvStream, avstream, error, getName() + " setting up PCM device period to size: " + Poco::NumberFormatter::format(_periodSize) + " returns: " + Poco::NumberFormatter::format(ret));
        return false;
    }
    if (snd_pcm_hw_params(_pcmPlayback, _hwParams) < 0) {
        LOGNS(Omm::AvStream, avstream, error, getName() + " initializing device.");
        return false;
    }

    return true;
}


bool
AlsaAudioSink::closeDevice()
{
    return true;
}


void
AlsaAudioSink::startPresentation()
{
    LOGNS(Omm::AvStream, avstream, debug, getName() + " starting write thread ...");

    _writeThread.start(_writeThreadRunnable);
    _writeThread.setPriority(Poco::Thread::PRIO_HIGHEST);
}


void
AlsaAudioSink::stopPresentation()
{
    LOGNS(Omm::AvStream, avstream, debug, getName() + " stopping write thread ...");
    setStopWriting(true);
}


void
AlsaAudioSink::waitPresentationStop()
{
    LOGNS(Omm::AvStream, avstream, debug, getName() + " trying to join write thread ...");
    try {
        if (_writeThread.isRunning()) {
            _writeThread.join(500);
        }
    }
    catch(...) {
        LOGNS(Omm::AvStream, avstream, warning, getName() + " failed to cleanly shutdown alsa audio write thread");
    }
    setStopWriting(false);
    LOGNS(Omm::AvStream, avstream, debug, getName() + " write thread joined.");
    if (_pcmPlayback) {
        snd_pcm_drop(_pcmPlayback);
    }
}


void
AlsaAudioSink::writeThread()
{
    LOGNS(Omm::AvStream, avstream, debug, "alsa audio sink write thread started.");

    const int bufferSizeInFrames = frameCount(_bufferSize);
    while(!getStopWriting()) {
        audioReadBlocking(_buffer, _bufferSize);
        int samplesWritten = 0;
        LOGNS(Omm::AvStream, avstream, trace, "alsa audio sink write thread, trying to write " + Poco::NumberFormatter::format(_bufferSize) + " bytes");
        // last parameter of snd_pcm_writei are the number of frames (not bytes) to write to the pcm ringbuffer
        while ((samplesWritten = snd_pcm_writei(_pcmPlayback, _buffer, bufferSizeInFrames)) < 0) {
            snd_pcm_prepare(_pcmPlayback);
            LOGNS(Omm::AvStream, avstream, warning, "<<<<<<<<<<<<<<< " + getName() + " buffer underrun >>>>>>>>>>>>>>>");
        }
        LOGNS(Omm::AvStream, avstream, trace("alsa audio sink write thread, bytes written: " +\
            Poco::NumberFormatter::format(samplesWritten << 2));
    }

    LOGNS(Omm::AvStream, avstream, debug, "alsa audio sink write thread finished.");
}


void
AlsaAudioSink::setStopWriting(bool stop)
{
    _writeLock.lock();
    _quitWriteThread = stop;
    _writeLock.unlock();
}


bool
AlsaAudioSink::getStopWriting()
{
    bool stop = false;
    _writeLock.lock();
    stop = _quitWriteThread;
    _writeLock.unlock();
    return stop;
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::AvStream::AudioSink)
POCO_EXPORT_CLASS(AlsaAudioSink)
POCO_END_MANIFEST
#endif
