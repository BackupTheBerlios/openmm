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
#include <Poco/ClassLibrary.h>

#include "WebradioSimple.h"

WebradiosimpleServer::WebradiosimpleServer()
// MediaContainer("Web Radio", "musicContainer")
{
    setTitle("Simple Web Radio");
    setIsContainer(true);
 //   setObjectId("0");
    
    std::string protInfoMp3 = "http-get:*:audio/mpeg:*";
    std::string subClass = "audioItem.audioBroadcast";
    
    
    /*----------- media object with meta data completely hold in memory, streaming directly from internet ------------*/
    
    Omm::Av::AbstractMediaObject* pGrooveSalad = new Omm::Av::MemoryMediaObject;
    pGrooveSalad->setIsContainer(false);
//    pGrooveSalad->setObjectId("1");
    pGrooveSalad->setTitle("SOMA FM - Groove Salad");
    
    Omm::Av::AbstractResource* pGrooveSaladRes = pGrooveSalad->createResource();
    pGrooveSaladRes->setProtInfo(protInfoMp3);
    pGrooveSaladRes->setSize(0);
    pGrooveSaladRes->setUri("http://streamer-dtc-aa04.somafm.com:80/stream/1018");
    pGrooveSalad->addResource(pGrooveSaladRes);
    appendChild(pGrooveSalad);
    
    
    /*----------- media object with meta data completely hold in memory, streaming through local proxy ------------*/

    Omm::Av::AbstractMediaObject* pLush = new Omm::Av::MemoryMediaObject;
//     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject<MemoryObject,MemoryProperty,WebResource>;
//     Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject(new MemoryObject, new MemoryProperty, new WebResource);
//    Omm::Av::AbstractMediaObject* pLush = new Omm::Av::AbstractMediaObject(new FileDirectory("/base/path"), new MemoryProperty, new FileResource("/base/path");
    pLush->setIsContainer(false);
//    pLush->setObjectId("2");
    pLush->setTitle("SOMA FM - Lush");

    Omm::Av::AbstractResource* pLushRes = pLush->createResource();
    pLushRes->setProtInfo(protInfoMp3);
    pLushRes->setSize(0);
    pLushRes->setUri("http://streamer-ntc-aa02.somafm.com:80/stream/1073");
    pLush->addResource(pLushRes);
    appendChild(pLush);
    
    Omm::Av::AbstractMediaObject* pStation = new Omm::Av::MemoryMediaObject;
    pStation->setIsContainer(false);
    Omm::Av::AbstractResource* pStationResource = pStation->createResource();
    pStationResource->setProtInfo(protInfoMp3);
    pStationResource->setSize(0);

    pStation->setTitle("SOMA FM - Drone Zone");
    pStationResource->setUri("http://streamer-dtc-aa01.somafm.com:80/stream/1032");
    pStation->addResource(pStationResource);
    appendChild(pStation);
    
    Omm::Av::AbstractMediaObject* pStation2 = new Omm::Av::MemoryMediaObject;
    pStation2->setIsContainer(false);
    Omm::Av::AbstractResource* pStationResource2 = pStation2->createResource();
    pStationResource2->setProtInfo(protInfoMp3);
    pStationResource2->setSize(0);
    pStation2->setTitle("Digitally Imported - Chillout");
    pStationResource2->setUri("http://scfire-ntc-aa01.stream.aol.com:80/stream/1035");
    pStation2->addResource(pStationResource2);
    appendChild(pStation2);
};

POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(WebradiosimpleServer)
POCO_END_MANIFEST
