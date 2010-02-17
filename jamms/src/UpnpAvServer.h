/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/
#ifndef JAMMUPNPAV_SERVER_H
#define JAMMUPNPAV_SERVER_H

#include <Jamm/Upnp.h>
#include <Jamm/UpnpAvTypes.h>
#include <Jamm/UpnpAvDevices.h>

using namespace Jamm;
using namespace Jamm::Av;


class UpnpAvServer : public MediaServer
{
public:
    UpnpAvServer(ContentDirectory* pContentDirectoryImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl);

//      void setServerImplementation(UpnpAvServerInterface* p serverImpl);
    void setRoot(MediaObject* pRoot);
    MediaObject* getRoot();
    void startScanTree();
    void stopScanTree();
    
private:
    MediaObject* m_pRoot;
//     MediaObjectCache* m_pCache;
//      UpnpAvServerInterface* m_pServerInterface;
};


class UpnpAvServerInterface
{
protected:
    
private:
    MediaObject* m_pRoot;
};


class SimpleMediaObjectModel
{
public:
    // filtering is done in ContentDirectoryServiceImplementation::Browse() by using
    // -> std::string propertyFilter(const std::string& object, const std::vector<std::string>& filterString)
    
    // searching is done in ContentDirectoryServiceImplementation::Search() by using
    // -> bool objectFilter(const std::string& object, const std::string& searchString)
    
    // sorting can be implemented in different ways:
    // 1. no sorting: Metadata can be passed from cache without modification
    // 2. sort in memory: store a sorted std::vector<MediaObject>
    // 3. cached sort index: store a sorted std::vector<std::string>
    // -> MediaObjectModel implementation may offer different sort strategies to select from
    
    // caching can be implemented in different ways:
    // 1. no caching: all meta data and object ids are generated dynamically
    // 2. meta data scan: all meta data is scanned a priori and continually updated
    // 3. meta data + indices: aditionally sorted indices are cached
    // -> MediaObjectModel implementation may offer different caching strategies to select from
    
    virtual int numChildren(const std::string& objectId) = 0;

    // return meta data of object
    // may be cached on disk or dynamically generated
    virtual std::string object(const std::string& objectId) = 0;
    virtual std::string child(const std::string& containerId, 
                              Jamm::ui4 childNumber,
                              const std::vector<std::string>& propertySortList) = 0;
};


class FastMediaObjectModel
{
public:
    // searching is done in FastMediaObjectModelImplementation::children() by using
    // -> bool objectFilter(const std::string& object, const std::string& searchString)
    
    virtual int numChildren(const std::string& objectId) = 0;
    
    // return meta data of object
    // may be cached on disk or dynamically generated
    virtual std::string object(const std::string& objectId) = 0;
    virtual std::string children(const std::string& containerId, 
                                 Jamm::ui4 childNumber,
                                 Jamm::ui4 childCount,
                                 const std::vector<std::string>& propertySortList,
                                 const std::string& searchString) = 0;
};

#endif
