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
        
        //////////// load and attach audio Sink ////////////
        if (demuxer.firstAudioStream() >= 0) {
            Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "AlsaSink/libomm-audiosink-alsa.so",
                "AlsaAudioSink");
//              Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "FileSinks/libomm-audiosink-pcm.so",
//                  "PcmAudioSink");
//             Omm::AvStream::AudioSink* pAudioSink = Omm::AvStream::AudioSink::loadPlugin(basePluginDir + "SdlSink/libomm-audiosink-sdl.so",
//                  "SdlAudioSink");
            demuxer.attach(pAudioSink, demuxer.firstAudioStream());
            Omm::AvStream::Clock::instance()->attachAudioSink(pAudioSink);
        }
        
        //////////// load and attach video sink ////////////
        if (demuxer.firstVideoStream() >= 0) {
            // FIXME: PpmVideoSink crashes while being allocated, when audio sink is loaded, too
            // => fix plugin loader
//             Omm::AvStream::VideoSink* pVideoSink = Omm::AvStream::VideoSink::loadPlugin(basePluginDir + "FileSinks/libomm-videosink-ppm.so",
//                 "PpmVideoSink");
            Omm::AvStream::VideoSink* pVideoSink = Omm::AvStream::VideoSink::loadPlugin(basePluginDir + "SdlSink/libomm-videosink-sdl.so",
                "SdlVideoSink");
            demuxer.attach(pVideoSink, demuxer.firstVideoStream());
            Omm::AvStream::Clock::instance()->attachVideoSink(pVideoSink);
        }
        
        
        std::clog << "<<<<<<<<<<<< ENGINE START ... >>>>>>>>>>>>" << std::endl;
        
        demuxer.start();
        Omm::AvStream::Clock::instance()->setStartTime();

        std::clog << "<<<<<<<<<<<< ENGINE RUN ... >>>>>>>>>>>>" << std::endl;

        Omm::AvStream::Clock::instance()->start();
        waitForTerminationRequest();
        Omm::AvStream::Clock::instance()->stop();
        
        std::clog << "<<<<<<<<<<<< ENGINE HALT. >>>>>>>>>>>>" << std::endl;
        
        demuxer.stop();
        
        std::clog << "<<<<<<<<<<<< ENGINE STOP. >>>>>>>>>>>>" << std::endl;
        
        ////////// deallocate meta data and packet queues ////////////
        demuxer.reset();
        
        std::clog << "AvPlayer finished." << std::endl;
    }
};


int main(int argc, char** argv)
{
    AvPlayer player;
    
    player.play(std::string(argv[1]));
}

