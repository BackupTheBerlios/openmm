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

#include <Poco/Thread.h>

#include <Omm/AvStream.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

class AlsaAudioSink : public Omm::AvStream::AudioSink
{
public:
    AlsaAudioSink();
    virtual ~AlsaAudioSink();

private:
    virtual bool initDevice();
    virtual bool closeDevice();
    virtual void startPresentation();
    virtual void stopPresentation();
    virtual void waitPresentationStop();
    
    bool open();
    bool open(const std::string& device);
    void close();
    
    void writeThread();
    void setStopWriting(bool stop);
    bool getStopWriting();
    
    Poco::Thread                            _writeThread;
    Poco::RunnableAdapter<AlsaAudioSink>    _writeThreadRunnable;
    bool                                    _quitWriteThread;
    Poco::FastMutex                         _writeLock;
    
    snd_pcm_t*                              _pcmPlayback;
    snd_pcm_hw_params_t*                    _hwParams;
    std::string                             _device;
    snd_pcm_format_t                        _format;
    unsigned int                            _rate;
    int                                     _channels;
    const int                               _periods;       // number of periods
    const snd_pcm_uframes_t                 _startPeriodSize;    // periodsize (bytes)
    snd_pcm_uframes_t                       _periodSize;    // periodsize (bytes)
    snd_pcm_uframes_t                       _bufferSize;
    char*                                   _buffer;
};

#endif
