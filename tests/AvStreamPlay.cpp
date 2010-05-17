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


class AvPlayer : public Omm::Av::AvStream
{
public:
    void stream(std::istream& istr)
    {
        std::clog << "AvPlayer::stream()" << std::endl;
        int i;
        
        set(istr);
        
        //////////// open and attach audio Sink ////////////
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
        Omm::Av::Sink* audioSink = Omm::Av::Sink::loadPlugin(basePluginDir + "AlsaSink/libommavr-alsasink.so", "AlsaSinkPlugin");
        audioSink->open();
        
        //////////// open and attach video Sink ////////////
//         Omm::Av::Sink* videoSink = Omm::Av::Sink::loadPlugin(basePluginDir + "QtSink/libommavr-qtsink.so", "QtSinkPlugin");
        Omm::Av::Sink* videoSink = Omm::Av::Sink::loadPlugin(basePluginDir + "SdlSink/libommavr-sdlsink.so", "SdlSinkPlugin");
        videoSink->open();
        
        //////////// open audio and video stream ////////////
        audioStream()->open();
        audioStream()->attachSink(audioSink);
        
        videoStream()->open();
        videoStream()->attachSink(videoSink);
        
        //////////// start demuxer thread ////////////
        Omm::Av::Demuxer* pDemuxer = new Omm::Av::Demuxer(this);
        Poco::Thread demuxThread;
        demuxThread.setName("demux thread");
        demuxThread.start(*pDemuxer);
        
        //////////// decode and render audio and video frames ////////////
        Poco::Thread audioThread;
        audioThread.setName("audio thread");
        audioThread.start(*audioStream());
        
        Poco::Thread videoThread;
        videoThread.setName("video thread");
        videoThread.start(*videoStream());
        
        //////////// start video presentation timer ////////////
        Omm::Av::PresentationTimer videoTimer(videoSink);
        videoTimer.start();
        
        //////////// wait for events ////////////
        videoSink->eventLoop();
        videoTimer.stop();
        
        demuxThread.tryJoin(10000);
        videoThread.tryJoin(10000);
        audioThread.tryJoin(10000);
        
        //////////// deallocate meta data and packet queues ////////////
        reset();
        
        std::clog << "AvPlayer::stream() finished." << std::endl;
    }
};


class VideoPlayer : public Omm::Av::AvStream
{
public:
    void stream(std::istream& istr)
    {
        std::clog << "VideoPlayer::stream()" << std::endl;
        int i;
        
        set(istr);
        
        //////////// open video Sink ////////////
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
//         Omm::Av::Sink* videoSink = Omm::Av::Sink::loadPlugin(basePluginDir + "QtSink/libommavr-qtsink.so", "QtSinkPlugin");
        Omm::Av::Sink* videoSink = Omm::Av::Sink::loadPlugin(basePluginDir + "SdlSink/libommavr-sdlsink.so", "SdlSinkPlugin");
        
        videoSink->open();
        
        //////////// read frames from input and demux them into single Streams ////////////
        demux();
        
        //////////// decode video frames ////////////
        videoStream()->open();
        
        i = 0;
        Omm::Av::Frame* pFrame;
        std::vector<Omm::Av::Frame*> decodedVideoFrames;
        while (pFrame = videoStream()->get()) {
            std::clog << "decode video frame #" << ++i << " ";
            Omm::Av::Frame* pFrameDecoded = pFrame->decode();
            if (pFrameDecoded) {
//                 decodedVideoFrames.push_back(pFrameDecoded);
                videoSink->writeFrame(pFrameDecoded);
            }
        }
        
        //////////// write decoded video frames to video Sink ////////////
//         i = 0;
//         for(std::vector<Omm::Av::Frame*>::iterator it = decodedVideoFrames.begin(); it != decodedVideoFrames.end(); ++it) {
//             std::clog << "write frame #" << ++i << " ";
//             videoSink->writeFrame(*it);
//             Poco::Thread::sleep(40);
// //             delete *it;
//         }
        
//         videoSink->writeFrame(decodedVideoFrames[100]);
        videoSink->eventLoop();
        
        //////////// deallocate meta data and packet queues ////////////
        reset();
        
        std::clog << "VideoPlayer::stream() finished." << std::endl;
    }
};


class VideoFrameWriter : public Omm::Av::AvStream
{
public:
    void stream(std::istream& istr)
    {
        std::clog << "VideoFrameWriter::stream()" << std::endl;
        int i;
        
        set(istr);
        
        //////////// read frames from input and demux them into single Streams ////////////
        demux();
        
        //////////// decode video frames ////////////
        videoStream()->open();
        
        i = 0;
        Omm::Av::Frame* pFrame;
        std::vector<Omm::Av::Frame*> decodedVideoFrames;
        while (pFrame = videoStream()->get()) {
            std::clog << "decode video frame #" << ++i << " ";
            Omm::Av::Frame* pFrameDecoded = pFrame->decode();
            if (pFrameDecoded) {
                pFrameDecoded->writePpm("frame" + Poco::NumberFormatter::format0(i, 3) + ".ppm");
                // make a copy of decoded frame
//                 Omm::Av::Frame* pFrameDecodedCopy = new Omm::Av::Frame(*pFrameDecoded);
//                 decodedVideoFrames.push_back(pFrameDecodedCopy);
            }
        }
        
        //////////// write decoded video frames to file ////////////
//         i = 0;
//         for(std::vector<Omm::Av::Frame*>::iterator it = decodedVideoFrames.begin(); it != decodedVideoFrames.end(); ++it) {
//             std::clog << "write frame #" << ++i << " ";
//             (*it)->writePpm("frame" + Poco::NumberFormatter::format0(i, 3) + ".ppm");
//         }
        
        //////////// deallocate meta data and packet queues ////////////
        reset();
        
        std::clog << "VideoFrameWriter::stream() finished." << std::endl;
    }
};


class AudioPlayerSyncMemory : public Omm::Av::AvStream
{
public:
    void stream(std::istream& istr)
    {
        std::clog << "AudioPlayer::stream()" << std::endl;
        int i;
        
        set(istr);
        
        //////////// open audio Sink ////////////
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
        Omm::Av::Sink* audioSink = Omm::Av::Sink::loadPlugin(basePluginDir + "AlsaSink/libommavr-alsasink.so", "AlsaSinkPlugin");
        
        audioSink->open();
        
        //////////// read frames from input and demux them into single Streams ////////////
        demux();
        
        //////////// decode audio frames ////////////
        audioStream()->open();
        
        i = 0;
        Omm::Av::Frame* pFrame;
        std::vector<Omm::Av::Frame*> decodedAudioFrames;
        while (pFrame = audioStream()->get()) {
            std::clog << "decode audio frame #" << ++i << " ";
            decodedAudioFrames.push_back(pFrame->decode());
        }
        
        //////////// write decoded audio frames to audio Sink ////////////
        i = 0;
        for(std::vector<Omm::Av::Frame*>::iterator it = decodedAudioFrames.begin(); it != decodedAudioFrames.end(); ++it) {
            std::clog << "write frame #" << ++i << " ";
            audioSink->writeFrame(*it);
            delete *it;
        }
        
        //////////// close audio sink and stream ////////////
        audioStream()->close();
        audioSink->close();
        
        //////////// deallocate meta data and packet queues ////////////
        reset();
        
        std::clog << "AudioPlayer::stream() finished." << std::endl;
    }
    
};


class AudioPlayerSyncFile : public Omm::Av::AvStream
{
public:
    void stream(std::istream& istr)
    {
        std::clog << "AudioPlayer::stream()" << std::endl;
        int i;
        
        set(istr);
        
        //////////// open audio Sink ////////////
        std::string basePluginDir("/home/jb/devel/cc/ommbin/renderer/src/");
        Omm::Av::Sink* audioSink = Omm::Av::Sink::loadPlugin(basePluginDir + "AlsaSink/libommavr-alsasink.so", "AlsaSinkPlugin");
        
        audioSink->open();
        
        //////////// read frames from input and demux them into single Streams ////////////
        demux();
        
        //////////// decode audio frames ////////////
        audioStream()->open();
        
        i = 0;
        Omm::Av::Frame* pFrame;
        std::ofstream outfile("audio.out");
        while (pFrame = audioStream()->get()) {
            std::clog << "decode frame #" << ++i << " ";
            Omm::Av::Frame* pFrameDecoded = pFrame->decode();
            outfile.write((const char*)pFrameDecoded->data(), pFrameDecoded->size());
            delete pFrameDecoded;
        }
        outfile.close();
        
        //////////// write decoded audio frames to audio Sink ////////////
        i = 0;
        int periodsize = 8192;
        char buffer[periodsize];
        std::ifstream infile("audio.out");
        while (infile.read((char*)buffer, periodsize)) {
            Omm::Av::Frame frame(audioStream(), buffer, periodsize);
            std::clog << "write frame #" << ++i << " ";
            audioSink->writeFrame(&frame);
        }
        infile.close();
        
        //////////// close audio sink and stream ////////////
        audioStream()->close();
        audioSink->close();
        
        //////////// deallocate meta data and packet queues ////////////
        reset();
        
        std::clog << "AudioPlayer::stream() finished." << std::endl;
    }
    
};


int main(int argc, char** argv)
{
//     std::ifstream istr("/home/jb/tmp/omm/o1$r1");  // mpgts with 522 frames
    
    // mp3 with 6210 frames of size 627 (= 3893670 bytes, actual size is 3895296 bytes) 118.875 buffers of size 32768
    std::ifstream istr("/home/jb/mp3/current/04_-_Kyuss_-_Hurricane.mp3");  // 2048 probe data + 3893248 read later = 3895296 bytes
    
//     AudioPlayerSyncMemory player;
//     AudioPlayerSyncFile player;
//     VideoFrameWriter player;
//     VideoPlayer player;
    AvPlayer player;
    
    player.stream(istr);
}

