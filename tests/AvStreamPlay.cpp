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
#include <iostream>
#include <fstream>

#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>

#include <AvStream.h>


class AvPlayer
{
public:
    void play(std::istream& istr)
    {
        std::clog << "AvPlayer starts playing ..." << std::endl;
        
        Omm::AvStream::Demuxer demuxer;
        
        demuxer.set(istr);
        
        //////////// setup engine stream graph ////////////
        Omm::AvStream::Decoder audioDecoder;
        if (demuxer.firstAudioStream() >= 0) {
            demuxer.attach(&audioDecoder, demuxer.firstAudioStream());
        }
        
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
        //////////// load and attach audio Sink ////////////
        Omm::AvStream::Sink* pAudioSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "AlsaSink/libomm-audiosink-alsa.so",
            "AlsaAudioSink");
//         Omm::AvStream::Sink* pAudioSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "FileSinks/libomm-audiosink-pcm.so",
//             "PcmAudioSink");
//         Omm::AvStream::Sink* pAudioSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "SdlSink/libomm-audiosink-sdl.so",
//             "SdlAudioSink");
        audioDecoder.attach(pAudioSink);
        
        //////////// load and attach video Sink ////////////
        if (demuxer.firstVideoStream() >= 0) {
//             Omm::AvStream::Sink* pVideoSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "FileSinks/libomm-videosink-ppm.so",
//                 "PpmVideoSink");
            Omm::AvStream::Sink* pVideoSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "SdlSink/libomm-videosink-sdl.so",
                "SdlVideoSink");
            demuxer.attach(pVideoSink, demuxer.firstVideoStream());
        }
        
        std::clog << "<<<<<<<<<<<< ENGINE STARTS ... >>>>>>>>>>>>" << std::endl;
        
        demuxer.start();
        Omm::AvStream::Clock::instance()->start(10);
        Poco::Thread::sleep(10000);
        Omm::AvStream::Clock::instance()->stop();
        demuxer.stop();
        
        std::clog << "<<<<<<<<<<<< ENGINE STOPPED. >>>>>>>>>>>>" << std::endl;
        
        //////////// deallocate meta data and packet queues ////////////
        demuxer.reset();
        
        std::clog << "AvPlayer finished." << std::endl;
    }
};


int main(int argc, char** argv)
{
//     std::ifstream istr("/home/jb/tmp/omm/o1$r1");  // mpgts with 522 frames
    
    // mp3 with 6210 frames of size 627 (= 3893670 bytes, actual size is 3895296 bytes) 118.875 buffers of size 32768
    std::ifstream istr("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");  // 2048 probe data + 3893248 read later = 3895296 bytes
    
    AvPlayer player;
    
    player.play(istr);
}
