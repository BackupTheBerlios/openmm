#include <vector>
#include <string>

#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Random.h>
#include <Poco/Environment.h>
#include <Poco/DirectoryIterator.h>

#include "../renderer/src/AvStreamEngine.h"


void
seqPlay(AvStreamEngine& engine, std::vector<std::string>& uris)
{
    // play all test streams once
    for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
        engine.setUri(*it);
        engine.load();
        Poco::Thread::sleep(1000);
        engine.stop();
    }
}


void
randPlay(AvStreamEngine& engine, std::vector<std::string>& uris)
{
    // play test streams randomly (TODO: with random play time between 0 and 2000 msecs.
    Poco::Random titleNumber;
    for(;;) {
        int i = titleNumber.next(uris.size());
        std::clog << "playing stream number: " << i << ", uri: " << uris[i] << std::endl;
        engine.setUri(uris[i]);
        engine.load();
        Poco::Thread::sleep(1000);
        engine.stop();
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
        liveTv.push_back("http://anubis:8888/" + Poco::NumberFormatter::format(i) + "$0");
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
    
//     seqPlay(engine, videoFiles);
//     randPlay(engine, videoFiles);
//     seqPlay(engine, audioFiles);
//     randPlay(engine, audioFiles);
    seqPlay(engine, liveTv);
//     randPlay(engine, liveTv);
//     seqPlay(engine, allStreams);
//     randPlay(engine, allStreams);
    
    engine.destructPlayer();
}