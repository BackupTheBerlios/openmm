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
#include <string>

#include <Poco/Environment.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/Random.h>

#include <Omm/AvStream.h>
#include <Omm/Util.h>

const std::string liveStreamAddress = "http://anubis:8888/";

void
tagUri(std::vector<std::string>& uris)
{
    for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
        std::clog << "tagging uri: " << *it << std::endl;

        Omm::AvStream::Tagger* pTagger;
        std::string taggerPlugin("tagger-ffmpeg");
        Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
        try {
            pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
        }
        catch(Poco::NotFoundException) {
            LOGNS(Omm::AvStream, avstream, error, "could not find avstream tagger plugin: " + taggerPlugin);
            return;
        }
        Omm::AvStream::Meta* pMeta = pTagger->tag(*it);
        if (pMeta) {
            pMeta->print();
        }
        delete pMeta;
        delete pTagger;
    }
}


void
tagStream(std::vector<std::string>& uris)
{
    for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
        std::clog << "tagging stream: " << *it << std::endl;

        Omm::AvStream::Tagger* pTagger;
        std::string taggerPlugin("tagger-ffmpeg");
        Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
        try {
            pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
        }
        catch(Poco::NotFoundException) {
            LOGNS(Omm::AvStream, avstream, error, "could not find avstream tagger plugin: " + taggerPlugin);
            return;
        }
        std::ifstream ifs((*it).c_str());
        Omm::AvStream::Meta* pMeta = pTagger->tag(ifs);
        if (pMeta) {
            pMeta->print();
        }
        delete pMeta;
        delete pTagger;
    }
}


int
main(int argc, char** argv)
{
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

//     tagUri(allStreams);
    tagStream(allStreams);
}
