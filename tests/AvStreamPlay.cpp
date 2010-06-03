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
#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <AvStream.h>
// #include "/home/jb/devel/cc/omm/renderer/src/FileSinks/PpmVideoSink.h"
// #include "/home/jb/devel/cc/omm/renderer/src/SdlSink/SdlAudioSink.h"



class AvPlayer : public Poco::Util::ServerApplication
{
public:
    void play(const std::string& uri)
    {
        std::clog << "AvPlayer starts playing " << uri << " ..." << std::endl;
        
        Omm::AvStream::Demuxer demuxer;
        
        demuxer.set(uri);
        
        if (demuxer.firstAudioStream() < 0 && demuxer.firstVideoStream() < 0) {
            std::clog << "no audio or video stream found, exiting" << std::endl;
            return;
        }
            
        //////////// setup engine stream graph ////////////
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
        
        Omm::AvStream::Decoder audioDecoder;
//         SdlAudioSink audioSink;
        
        if (demuxer.firstAudioStream() >= 0) {
            demuxer.attach(&audioDecoder, demuxer.firstAudioStream());
        
        //////////// load and attach audio Sink ////////////
            Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "AlsaSink/libomm-audiosink-alsa.so",
                "AlsaAudioSink");
//              Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "FileSinks/libomm-audiosink-pcm.so",
//                  "PcmAudioSink");
//             Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "SdlSink/libomm-audiosink-sdl.so",
//                  "SdlAudioSink");
            audioDecoder.attach(pAudioSink);
            Omm::AvStream::Clock::instance()->attachAudioSink(pAudioSink);
            
//             audioDecoder.attach(&audioSink);
//             Omm::AvStream::Clock::instance()->attachSink(&audioSink);
        }
        
        //////////// load and attach video sink ////////////
        if (demuxer.firstVideoStream() >= 0) {
            // FIXME: PpmVideoSink crashes while being allocated, when audio sink is loaded, too
            // => fix plugin loader
//             Omm::AvStream::Sink* pVideoSink = Omm::AvStream::Sink::loadPlugin(basePluginDir + "FileSinks/libomm-videosink-ppm.so",
//                 "PpmVideoSink");
            Omm::AvStream::VideoSink* pVideoSink = Omm::AvStream::VideoSink::loadPlugin(basePluginDir + "SdlSink/libomm-videosink-sdl.so",
                "SdlVideoSink");
            demuxer.attach(pVideoSink, demuxer.firstVideoStream());
            Omm::AvStream::Clock::instance()->attachVideoSink(pVideoSink);
        }
        
        std::clog << "<<<<<<<<<<<< ENGINE STARTS ... >>>>>>>>>>>>" << std::endl;
        
        demuxer.start();
        Omm::AvStream::Clock::instance()->start();
        waitForTerminationRequest();
        Omm::AvStream::Clock::instance()->stop();
        demuxer.stop();
        
        std::clog << "<<<<<<<<<<<< ENGINE STOPPED. >>>>>>>>>>>>" << std::endl;
        
        ////////// deallocate meta data and packet queues ////////////
        demuxer.reset();
        
        std::clog << "AvPlayer finished." << std::endl;
    }
};


int main(int argc, char** argv)
{
//     std::ifstream istr("/home/jb/tmp/omm/o1$r1");  // mpgts with 522 frames
    
    // mp3 with 6210 frames of size 627 (= 3893670 bytes, actual size is 3895296 bytes) 118.875 buffers of size 32768
//     std::ifstream istr("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");  // 2048 probe data + 3893248 read later = 3895296 bytes
    
    AvPlayer player;
    
//     player.play("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");
    player.play(std::string(argv[1]));
}

