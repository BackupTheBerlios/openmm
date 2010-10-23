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
#include <vector>
#include <string>

#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Random.h>
#include <Poco/Environment.h>
#include <Poco/DirectoryIterator.h>

#include "../renderer/src/avstream/AvStreamEngine.h"

const int maxRandomPlayTime = 2000;
const std::string liveStreamAddress = "http://anubis:8888/";

void
seqPlay(AvStreamEngine& engine, std::vector<std::string>& uris, int playTime = -1)
{
    // play all test streams once
    Poco::Random playTimeRandom;
    for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
        engine.setUri(*it);
        engine.load();
        if (playTime >= 0) {
            Poco::Thread::sleep(playTime);
        }
        else {
            int t = playTimeRandom.next(maxRandomPlayTime);
            Poco::Thread::sleep(t);
        }
        engine.stop();
    }
}


void
randPlay(AvStreamEngine& engine, std::vector<std::string>& uris, int maxIterations = -1, int playTime = -1)
{
    // play test streams randomly (TODO: with random play time between 0 and 2000 msecs.
    Poco::Random titleNumber;
    Poco::Random playTimeRandom;
    int iterations = 0;
    for(;;) {
        if (maxIterations >= 0 && iterations >= maxIterations) {
            break;
        }
        int i = titleNumber.next(uris.size());
        std::clog << "playing stream number: " << i << ", uri: " << uris[i] << std::endl;
        engine.setUri(uris[i]);
        engine.load();
        if (playTime >= 0) {
            Poco::Thread::sleep(playTime);
        }
        else {
            int t = playTimeRandom.next(maxRandomPlayTime);
            Poco::Thread::sleep(t);
        }
        engine.stop();
        iterations++;
    }
}


int
main(int argc, char** argv) {
    std::string basePath(Poco::Environment::get("OMMTEST"));
    Poco::DirectoryIterator end;

    std::vector<std::string> videoFiles;
    std::string videoPath = basePath + "/video";
    Poco::DirectoryIterator videoDir(videoPath);
    while(videoDir != end) {
        videoFiles.push_back(videoPath + "/" + videoDir.name());
        ++videoDir;
    }
    
    std::vector<std::string> audioFiles;
    std::string audioPath = basePath + "/audio";
    Poco::DirectoryIterator audioDir(audioPath);
    while(audioDir != end) {
        audioFiles.push_back(audioPath + "/" + audioDir.name());
        ++audioDir;
    }
    
    std::vector<std::string> liveTv;
    for (int i = 0; i < 10; ++i) {
        liveTv.push_back(liveStreamAddress + Poco::NumberFormatter::format(i) + "$0");
    }
    
    std::vector<std::string> allStreams;
    for (std::vector<std::string>::iterator it = audioFiles.begin(); it != audioFiles.end(); ++it) {
        allStreams.push_back(*it);
    }
    for (std::vector<std::string>::iterator it = videoFiles.begin(); it != videoFiles.end(); ++it) {
        allStreams.push_back(*it);
    }
    
    AvStreamEngine engine;
    engine.createPlayer();
    
//     seqPlay(engine, audioFiles);
//     randPlay(engine, audioFiles);
    seqPlay(engine, videoFiles);
//     randPlay(engine, videoFiles);
//    seqPlay(engine, liveTv);
//     randPlay(engine, liveTv, 10);
//      seqPlay(engine, allStreams);
//      randPlay(engine, allStreams);
    
    engine.destructPlayer();
}
