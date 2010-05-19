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

#include "SdlAudioSink.h"


AudioSinkPlugin::AudioSinkPlugin() /*:
pcm_playback(0),
device("default"),
format(SND_PCM_FORMAT_S16),
rate(44100),
channels(2),
periods(2),
periodsize(8192),
buffer(new char[periodsize]),
bufferPos(buffer),
frames(periodsize >> 2)*/
{
}


AudioSinkPlugin::~AudioSinkPlugin()
{
    delete buffer;
}


void
AudioSinkPlugin::open()
{
    open("default");
}


void
AudioSinkPlugin::open(const std::string& device)
{
    std::clog << "Opening SDL audio sink with device: " << device << std::endl;
    
//     int err = snd_pcm_open(&pcm_playback, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
//     if (err < 0) {
//         std::cerr << "error: could not open alsa device: " << device << std::endl;
//         return;
//     }
    
    initDevice();
    
    std::clog << "SDL audio sink opened." << std::endl;
}


void
AudioSinkPlugin::close()
{
    std::clog << "AudioSinkPlugin::close()" << std::endl;
    
//     if (pcm_playback) {
//         snd_pcm_drop(pcm_playback);
//         snd_pcm_close(pcm_playback);
//     }
}


void
AudioSinkPlugin::initDevice()
{
    std::clog << "AudioSinkPlugin::initDevice()" << std::endl;
    

}


void
AudioSinkPlugin::writeFrame(Omm::Av::Frame* pFrame)
{
    std::clog << "Alsa audio Sink::writeFrame()" << std::endl;
//     if (!pFrame) {
//         std::cerr << "error: no frame to write" << std::endl;
//         return;
//     }
//     
//     int framesWritten;
//     while ((framesWritten = snd_pcm_writei(pcm_playback, pFrame->data(), pFrame->size() >> 2)) < 0) {
//         snd_pcm_prepare(pcm_playback);
//         std::cerr << "<<<<<<<<<<<<<<< buffer underrun >>>>>>>>>>>>>>>" << std::endl;
//     }
//     std::clog << "frames written: " << framesWritten << std::endl;
}


POCO_BEGIN_MANIFEST(Omm::Av::Sink)
POCO_EXPORT_CLASS(AudioSinkPlugin)
POCO_END_MANIFEST
