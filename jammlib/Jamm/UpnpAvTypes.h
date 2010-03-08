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
#ifndef JAMMUPNPAV_TYPES_H
#define JAMMUPNPAV_TYPES_H

#include "Upnp.h"

// NOTE: for media object ids only use non-reserved characters for segments 
// (according to RFC 3986), because object ids may be used in resource URIs.
// These characters are alphanumerics, digits, "-", ".", "_", "!", ";", ",", "="

namespace Jamm {
namespace Av {

class MediaServerController;

class AvTypeConverter
{
public:
    static r8 readDuration(const std::string& duration);
    static std::string writeDuration(const r8& duration);
};


class Resource
{
public:
    std::string     m_uri;
    std::string     m_protInfo;
    ui4             m_size;
};


class MediaObject
{
public:
    // NOTE: instead of this ctor make a MediaObjectWriter
    // MediaObject* createObject(const std::string&)
    // MediaObject* createObject(DatabasePointer)
    // and a MediaObjectReader ...
    
    MediaObject();
    // NOTE: should be split into a Controller and Device part?
//     MediaObject(const std::string& metaData);
//     MediaObject(Poco::XML::Node* pNode);
    
    
    // controller interface
    int fetchChildren();
    bool isContainer() { return m_isContainer; }
    std::string getTitle();
    std::string getProperty(const std::string& name);
    std::string getParentId();
    ui4 getChildCount();
    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);
    
    MediaServerController*             m_server;
    
    // server interface
    void setParentId(const std::string& parentId);
    void setIsContainer(bool isContainer=true);
    void setObjectId(const std::string& objectId);
    void setTitle(const std::string& title);
    void addResource(const std::string& uri, const std::string& protInfo = "", ui4 size = 0);
    Resource* getResource(int num = 0);
    
    void appendChild(const std::string& objectId, MediaObject* pChild);
    MediaObject* getObject(const std::string& objectId);
    void writeMetaData(std::string& metaData);
    void writeMetaData(Poco::XML::Element* pDidl);
    ui4 writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData);
    
    void writeMetaDataHeader();
    void writeMetaDataClose(std::string& metaData);
    
    std::map<std::string,MediaObject*>      m_childrenMap;
    
    // TODO: this should be in a seperate class
    //       (sometimes the XML fragement can be fetched from cache)
    Poco::AutoPtr<Poco::XML::Document>      m_pDoc;
    Poco::AutoPtr<Poco::XML::Element>       m_pDidl;

// common interface
    std::string getObjectId() const;
    
    MediaObject*                            m_parent;
    std::vector<MediaObject*>               m_children;
    std::string                             m_objectId;
    std::string                             m_parentId;
    bool                                    m_restricted;
    unsigned int                            m_childCount;
    bool                                    m_fetchedAllChildren;
    Container<Variant>                      m_properties;
    bool                                    m_isContainer;
    std::vector<Resource*>                  m_resources;
    
    // object properties:
//     const std::string& getId();
//     void setId(const std::string& id);
//     std::string getParentId();
//     void setParentId(const std::string& id);
//     void setTitle(const std::string& id);
//     std::string getRes();
//     void setRes(const std::string& id);
//     std::string getClass();
//     void setClass(const std::string& id);
};


class MediaContainer : public MediaObject
{
public:
    MediaContainer();
    MediaContainer(const std::string& title);
};


class MediaItem : public MediaObject
{
public:
    MediaItem();
    MediaItem(const std::string& title, const std::string& uri, const std::string& protInfo = "", ui4 size = 0);
};


} // namespace Av
} // namespace Jamm

#endif
