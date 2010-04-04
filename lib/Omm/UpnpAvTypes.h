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

#ifndef OMMUPNPAV_TYPES_H
#define OMMUPNPAV_TYPES_H

#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>
#include <Poco/SplitterChannel.h>

#include "Upnp.h"

// NOTE: for media object ids only use non-reserved characters for segments 
// (according to RFC 3986), because object ids may be used in resource URIs.
// These characters are alphanumerics, digits, "-", ".", "_", "!", ";", ",", "="

namespace Omm {
namespace Av {

class Log
{
public:
    static Log* instance();
    
    Poco::Logger& upnpav();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pUpnpAvLogger;
};


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
    std::string     _uri;
    std::string     _protInfo;
    ui4             _size;
};


class MediaObject
{
public:
    // NOTE: instead of this ctor make a MediaObjectWriter
    // MediaObject* createObject(const std::string&)
    // MediaObject* createObject(DatabasePointer)
    // and a MediaObjectReader ...
    
    // NOTE: should be split into a Controller and Device part?
//     MediaObject(const std::string& metaData);
//     MediaObject(Poco::XML::Node* pNode);
    
    MediaObject();
    
    // controller interface
    typedef std::vector<MediaObject*>::iterator childIterator;
    childIterator beginChildren();
    childIterator endChildren();
    int fetchChildren();
    bool fetchedAllChildren();
    bool isContainer() { return _isContainer; }
    std::string getTitle();
    std::string getProperty(const std::string& name);
    std::string getParentId();
    MediaObject* parent();
    MediaObject* getChild(ui4 num);
    ui4 getChildCount();
    ui4 childCount();
    std::string objectId();
    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);
    void setServerController(MediaServerController* _pServer);
    void setFetchedAllChildren(bool fetchedAllChildren);
    
private:
    bool                                    _fetchedAllChildren;
    MediaServerController*                  _server;
    
public:
    // server interface
    void setParentId(const std::string& parentId);
    void setIsContainer(bool isContainer=true);
    void setObjectId(const std::string& objectId);
    void setTitle(const std::string& title);
    void addResource(const std::string& uri, const std::string& protInfo = "", ui4 size = 0);
    Resource* getResource(int num = 0);
    void setProperty(const std::string& name, const std::string& value);
    
    void appendChild(const std::string& objectId, MediaObject* pChild);  // depricated
    void appendChild(MediaObject* pChild);
    MediaObject* getObject(const std::string& objectId);
    
    void writeMetaData(std::string& metaData);
    void writeMetaData(Poco::XML::Element* pDidl);
    ui4 writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData);
    void writeMetaDataHeader();
    void writeMetaDataClose(std::string& metaData);
    
private:
    std::map<std::string,MediaObject*>      _childrenMap;
    
    // TODO: this should be in a seperate class
    //       (sometimes the XML fragement can be fetched from cache)
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Poco::AutoPtr<Poco::XML::Element>       _pDidl;

public:
// common interface
    std::string getObjectId() const;
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
    
protected:
    MediaObject*                            _parent;
    std::string                             _objectId;
    std::string                             _parentId;
    bool                                    _restricted;
    unsigned int                            _childCount;
    Container<Variant>                      _properties;
//     std::multimap<std::string>              _properties;
    bool                                    _isContainer;
    std::vector<Resource*>                  _resources;

private:
    std::vector<MediaObject*>               _children;
};


class MediaContainer : public MediaObject
{
public:
    MediaContainer();
    MediaContainer(const std::string& title, const std::string& subClass = "");
};


class MediaItem : public MediaObject
{
public:
    MediaItem();
//     MediaItem(const std::string& title, const std::string& subClass = "", bool restricted = true);
    
//     void addResource(const std::string& uri, const std::string& profile = "", ui4 size = 0);
    
    MediaItem(const std::string& title, const std::string& uri, const std::string& protInfo = "", ui4 size = 0, const std::string& subClass = "");
};


} // namespace Av
} // namespace Omm

#endif
