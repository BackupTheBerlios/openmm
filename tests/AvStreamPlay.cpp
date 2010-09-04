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
#include <Util.h>


class AvPlayer : public Poco::Util::ServerApplication
{
public:
    void play(const std::string& uri)
    {
        std::clog << "AvPlayer starts playing " << uri << " ..." << std::endl;
        
//         std::ifstream fileStream(uri.c_str());
        
        Omm::AvStream::Clock clock;
        Omm::AvStream::Demuxer demuxer;
        
        demuxer.set(uri);
//         demuxer.set(fileStream);
        
        if (demuxer.firstAudioStream() < 0 && demuxer.firstVideoStream() < 0) {
            std::clog << "no audio or video stream found, exiting" << std::endl;
            return;
        }
            
        //////////// load and attach audio Sink ////////////
        if (demuxer.firstAudioStream() >= 0) {
            Omm::Util::PluginLoader<Omm::AvStream::AudioSink> audioPluginLoader;
//             Omm::AvStream::AudioSink* pAudioSink = audioPluginLoader.load("audiosink-pcm", "AudioSink");
//             Omm::AvStream::AudioSink* pAudioSink = audioPluginLoader.load("audiosink-sdl", "AudioSink");
            Omm::AvStream::AudioSink* pAudioSink = audioPluginLoader.load("audiosink-alsa", "AudioSink");
            demuxer.attach(pAudioSink, demuxer.firstAudioStream());
            clock.attachAudioSink(pAudioSink);
        }
        
        //////////// load and attach video sink ////////////
        if (demuxer.firstVideoStream() >= 0) {
            Omm::Util::PluginLoader<Omm::AvStream::VideoSink> videoPluginLoader;
//             Omm::AvStream::VideoSink* pVideoSink = videoPluginLoader.load("videosink-ppm", "VideoSink");
//             Omm::AvStream::VideoSink* pVideoSink = videoPluginLoader.load("videosink-qt", "VideoSink");
            Omm::AvStream::VideoSink* pVideoSink = videoPluginLoader.load("videosink-sdl", "VideoSink");
            demuxer.attach(pVideoSink, demuxer.firstVideoStream());
            clock.attachVideoSink(pVideoSink);
        }
        
        std::clog << "<<<<<<<<<<<< ENGINE START ... >>>>>>>>>>>>" << std::endl;
        
        demuxer.start();
        clock.setStartTime();

        std::clog << "<<<<<<<<<<<< ENGINE RUN ... >>>>>>>>>>>>" << std::endl;

        clock.start();
        waitForTerminationRequest();
        clock.stop();
        
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

