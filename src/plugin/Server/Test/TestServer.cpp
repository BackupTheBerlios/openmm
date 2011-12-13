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

#include <Poco/ClassLibrary.h>

#include <Omm/Util.h>

#include "TestServer.h"

TestServer::TestServer() //:
// AbstractMediaObject(new MemoryObjectImpl)    // could also use convenience class MemoryContainer
{
    setTitle("Test Server");
    setIsContainer(true);    // could also use convenience class MemoryContainer
    setClass(Omm::Av::AvClass::CONTAINER);

    /*----------- media object with meta data completely hold in memory, streaming directly from internet ------------*/
    std::string protInfoMp3 = "http-get:*:audio/mpeg:*";
    std::string subClass = "audioItem.audioBroadcast";

    Omm::Av::AbstractMediaObject* pGrooveSalad = createChildObject();
    pGrooveSalad->setIsContainer(false);
    pGrooveSalad->setTitle("SOMA FM - Groove Salad (mp3)");

    Omm::Av::AbstractResource* pGrooveSaladRes = pGrooveSalad->createResource();
    pGrooveSaladRes->setProtInfo(protInfoMp3);
    pGrooveSaladRes->setSize(0);
    pGrooveSaladRes->setUri("http://streamer-dtc-aa04.somafm.com:80/stream/1018");

    Omm::Av::AbstractMediaObject* pLush = createChildObject();
    pLush->setIsContainer(false);
    pLush->setTitle("SOMA FM - Lush (mp3)");

    Omm::Av::AbstractResource* pLushRes = pLush->createResource();
    pLushRes->setProtInfo(protInfoMp3);
    pLushRes->setSize(0);
    pLushRes->setUri("http://streamer-ntc-aa02.somafm.com:80/stream/1073");

    /*----------- media object with meta data completely hold in memory, streaming through local proxy ------------*/
// //     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::StreamingMemoryMediaObject;
// //     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject<MemoryObject,MemoryProperty,WebResource>;
// //     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject(new MemoryObject, new MemoryProperty, new WebResource);
//     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject(new FileDirectory("/base/path"), new MemoryProperty, new FileResource("/base/path");

    /*----------- media object with meta data dynamically loaded for each media item, streaming through local proxy ------------*/
    std::string pluginName("server-file");

    Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> videoFileServerLoader;
    Omm::Av::AbstractMediaObject* pVideos;
    try {
        pVideos = videoFileServerLoader.load(pluginName);
    }
    catch(Poco::NotFoundException) {
        std::cerr << "Error could not find server plugin: " << pluginName << std::endl;
        return;
    }
    std::clog << "server plugin: " << pluginName << " loaded successfully" << std::endl;

    pVideos->setTitle("Videos");
//    pVideos->setOption("basePath", "/Users/jb/Videos");
    pVideos->setOption("basePath", "/home/jb/video");
    appendChildWithAutoIndex(pVideos);


    Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> mp3FileServerLoader;
    Omm::Av::AbstractMediaObject* pMusic;
    try {
        pMusic = mp3FileServerLoader.load(pluginName);
    }
    catch(Poco::NotFoundException) {
        std::cerr << "Error could not find server plugin: " << pluginName << std::endl;
        return;
    }
    std::clog << "server plugin: " << pluginName << " loaded successfully" << std::endl;

    pMusic->setTitle("Music");
    pMusic->setOption("basePath", "/home/jb/music");
//    pMusic->setOption("basePath", "/Users/jb/mp3");
//     pMusic->setOption("basePath", "/media/mydata/mp3");
    appendChildWithAutoIndex(pMusic);


//     pluginName = "server-dvb";
//     Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> DvbServerLoader;
//     Omm::Av::AbstractMediaObject* pDvb;
//     try {
//         pDvb = DvbServerLoader.load(pluginName);
//     }
//     catch(Poco::NotFoundException) {
//         std::cerr << "Error could not find server plugin: " << pluginName << std::endl;
//         return;
//     }
//     std::clog << "server plugin: " << pluginName << " loaded successfully" << std::endl;
//
//     pDvb->setTitle("Digital TV");
//     pDvb->setOption("basePath", "/home/jb/.omm/channels.conf");
//     appendChild(pDvb);
};

POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(TestServer)
POCO_END_MANIFEST
