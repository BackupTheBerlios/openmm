#include <vector>
#include <string>

#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Random.h>

#include "../renderer/src/AvStreamEngine.h"

int
main(int argc, char** argv) {
    std::string basePath("/home/jb/devel/cc/ommtest/dvb_streams/omm");
    std::vector<std::string> uris;
    
    int numStreams = 10;
    for (int i = 1; i <= numStreams; ++i) {
        uris.push_back(basePath + "/o" + Poco::NumberFormatter::format(i) + "$r1");
    }
    
    AvStreamEngine engine;
    engine.createPlayer();
    
    // play all test streams once
//     for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
//         engine.setUri(*it);
//         engine.load();
//         Poco::Thread::sleep(1000);
//         engine.stop();
//     }
    
    // play test streams randomly (TODO: with random play time between 0 and 2000 msecs.
    Poco::Random rand;
    for(;;) {
        int i = rand.next(uris.size());
        std::clog << "playing stream number: " << i << ", uri: " << uris[i] << std::endl;
        engine.setUri(uris[i]);
        engine.load();
        Poco::Thread::sleep(1000);
        engine.stop();
    }
    
    engine.destructPlayer();
}