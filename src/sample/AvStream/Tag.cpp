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
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/Random.h>

#include <Omm/AvStream.h>
#include <Omm/Util.h>


int
main(int argc, char** argv)
{
    std::clog << "tagging uri: " << argv[1] << std::endl;

    Omm::AvStream::Tagger* pTagger;

    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        LOGNS(Omm::AvStream, avstream, warning, "could not find avstream tagger plugin: " + taggerPlugin + " loading vlc tagger ...");
        try {
            pTagger = taggerPluginLoader.load("tagger-vlc");
        }
        catch(Poco::NotFoundException) {
            LOGNS(Omm::AvStream, avstream, warning, "could not find avstream tagger plugin: tagger-vlc loading simple tagger ...");
            pTagger = taggerPluginLoader.load("tagger-simple");
        }
    }

    Omm::AvStream::Meta* pMeta = pTagger->tag(std::string(argv[1]));
    if (pMeta) {
        pMeta->print();
    }
    Omm::AvStream::Meta::ContainerFormat format = pMeta->getContainerFormat();
    std::clog << std::endl;
    switch (format) {
        case Omm::AvStream::Meta::CF_AUDIO:
            std::clog << "Audio stream" << std::endl;
            break;
        case Omm::AvStream::Meta::CF_VIDEO:
            std::clog << "Video stream" << std::endl;
            break;
        case Omm::AvStream::Meta::CF_IMAGE:
            std::clog << "Still image" << std::endl;
            break;
    }
    std::clog << std::endl;
    std::clog << "Artist: " << pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST) << std::endl;
    std::clog << "Album: " << pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM) << std::endl;
    std::clog << "Title: " << pMeta->getTag(Omm::AvStream::Meta::TK_TITLE) << std::endl;
    std::clog << "Track: " << pMeta->getTag(Omm::AvStream::Meta::TK_TRACK) << std::endl;
    std::clog << "Duration: " << pMeta->getTag(Omm::AvStream::Meta::TK_DURATION) << std::endl;
    std::clog << "Genre: " << pMeta->getTag(Omm::AvStream::Meta::TK_GENRE) << std::endl;
    std::clog << std::endl;

    delete pMeta;
    delete pTagger;
    return 0;
}
