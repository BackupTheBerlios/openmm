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
    friend class MediaObject;
    
public:
    Resource(const std::string& uri, const std::string& protInfo, ui4 size);
    
    const std::string& getUri();
    const std::string& getProtInfo();
    ui4 getSize();
    
    void setUri(const std::string& uri);
    void setProtInfo(const std::string& protInfo);
    
private:
    std::string     _uri;
    std::string     _protInfo;
    ui4             _size;
};


class MediaObject
{
    friend class MediaObjectWriter;
    
public:
    MediaObject();
    
    typedef std::map<std::string,std::string>::iterator PropertyIterator;
    PropertyIterator beginProperty();
    PropertyIterator endProperty();
    
    typedef std::vector<MediaObject*>::iterator ChildIterator;
    ChildIterator beginChildren();
    ChildIterator endChildren();
    
    typedef std::vector<Resource*>::iterator ResourceIterator;
    ResourceIterator beginResource();
    ResourceIterator endResource();
    
    void appendChild(MediaObject* pChild);
    void addResource(Resource* pResource);
    virtual std::string getObjectId() const;
    MediaObject* getChild(ui4 num);
    std::string getParentId();
    ui4 getChildCount();
    
    std::string objectId();
    std::string getTitle();
    bool isContainer();
    bool isRestricted();
    
    void setIsContainer(bool isContainer=true);
    void setObjectId(const std::string& objectId);
    void setTitle(const std::string& title);
    void setProperty(const std::string& name, const std::string& value);
    
protected:
    
    std::string                             _objectId;
    bool                                    _restricted;
    std::map<std::string,std::string>       _properties;
    
    bool                                    _isContainer;
    // is Container:
    MediaObject*                            _parent;
    std::vector<MediaObject*>               _children;
    // is Item:
    std::vector<Resource*>                  _resources;
};



class MediaObjectWriter
{
public:
    MediaObjectWriter(MediaObject* pMediaObject);
    
    void write(std::string& metaData);
    ui4 writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData);
    
private:
    void writeMetaDataHeader();
    void writeMetaDataClose(std::string& metaData);
    void writeMetaData(Poco::XML::Element* pDidl);
    
    MediaObject*                            _pMediaObject;
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Poco::AutoPtr<Poco::XML::Element>       _pDidl;
};


class AbstractResource
{
public:
    virtual ~AbstractResource();
    
    virtual void setUri(const std::string& uri) = 0;
    virtual void setProtInfo(const std::string& protInfo) = 0;
    virtual void setSize(ui4 size) = 0;
    
    virtual std::string getUri() = 0;
    virtual std::string getProtInfo() = 0;
    virtual ui4 getSize() = 0;
};


class AbstractMediaObject
{
public:
    virtual ~AbstractMediaObject();
    
    virtual AbstractMediaObject* createObject() = 0;
    virtual AbstractResource* createResource() = 0;
    
    virtual void setObjectId(const std::string& objectId) = 0;
    virtual void setParentObjectId(const std::string& objectId) = 0;
    virtual void appendChild(AbstractMediaObject* pChild) = 0;
    virtual void setIsContainer(bool isContainer) = 0;
    virtual void setIsRestricted(bool isRestricted) = 0;
    virtual void setChildCount(ui4 childCount) = 0;
    virtual void setProperty(const std::string& property, const std::string& value) = 0;
    virtual void addResource(AbstractResource* pResource) = 0;
    
    virtual std::string getObjectId() = 0;
    virtual std::string getParentObjectId() = 0;
    virtual ui4 getChildCount() = 0;
    virtual int getPropertyCount() = 0;
    virtual int getResourceCount() = 0;
    virtual bool isContainer() = 0;
    virtual bool isRestricted() = 0;
    virtual AbstractMediaObject* getParent() = 0;
    virtual AbstractMediaObject* getChild(ui4 numChild) = 0;
    virtual AbstractMediaObject* getObject(const std::string& objectId) = 0;
    // TODO: handle multiple occurence properties
    virtual std::string getPropertyName(int numProperty) = 0;
    virtual std::string getProperty(const std::string& property) = 0;
    virtual AbstractResource* getResource(int numResource) = 0;
    virtual AbstractResource* getResource(const std::string& resourceId) = 0;
    
    std::string getTitle();
};


class MediaObjectReader
{
public:
    MediaObjectReader(AbstractMediaObject* pMediaObject);
    
    void read(const std::string& metaData);
    void readChildren(const std::string& metaData);
    
private:
    void readNode(AbstractMediaObject* pObject, Poco::XML::Node* pNode);
    
    AbstractMediaObject*                    _pMediaObject;
};


class MediaObjectWriter2
{
public:
    MediaObjectWriter2(AbstractMediaObject* pMediaObject);
    
    void write(std::string& metaData);
    ui4 writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData);
    
private:
    void writeMetaDataHeader();
    void writeMetaDataClose(std::string& metaData);
    void writeMetaData(Poco::XML::Element* pDidl);
    
    AbstractMediaObject*                    _pMediaObject;
    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Poco::AutoPtr<Poco::XML::Element>       _pDidl;
};


} // namespace Av
} // namespace Omm

#endif
