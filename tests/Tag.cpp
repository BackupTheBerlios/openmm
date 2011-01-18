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

#include <AvStream.h>
#include <Util.h>


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
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream tagger plugin: " + taggerPlugin);
        return 1;
    }
    Omm::AvStream::Meta* pMeta = pTagger->tag(std::string(argv[1]));
    if (pMeta) {
        pMeta->print();
    }
    std::clog << std::endl;
    std::clog << "Artist: " << pMeta->getProperty(Omm::AvStream::Meta::TK_ARTIST) << std::endl;
    std::clog << "Album: " << pMeta->getProperty(Omm::AvStream::Meta::TK_ALBUM) << std::endl;
    std::clog << "Title: " << pMeta->getProperty(Omm::AvStream::Meta::TK_TITLE) << std::endl;
    std::clog << "Track: " << pMeta->getProperty(Omm::AvStream::Meta::TK_TRACK) << std::endl;
    std::clog << "Genre: " << pMeta->getProperty(Omm::AvStream::Meta::TK_GENRE) << std::endl;
    std::clog << std::endl;
    
    delete pMeta;
    delete pTagger;
    return 0;
}
