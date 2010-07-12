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


class AbstractProperty
{
public:
};


class AbstractResource : public AbstractProperty
{
public:
    virtual ~AbstractResource();
    
    virtual void setUri(const std::string& uri) = 0;
    virtual void setProtInfo(const std::string& protInfo) = 0;
    virtual void setSize(ui4 size) = 0;
    
    virtual std::string getId() = 0;
    virtual std::string getUri() = 0;                           // server object, write meta data
    virtual std::string getProtInfo() = 0;                      // server object, write meta data
    virtual ui4 getSize() = 0;                                  // server object, write meta data
};



// class AbstractReadableMediaObject : AbstractMediaObject
// {
//     public:
// 
// };
// 
// 
// class AbstractWriteableMediaObject : AbstractMediaObject
// {
//     public:
//  
// };


class AbstractMediaObject
{
public:
    virtual ~AbstractMediaObject() {};
    
    virtual AbstractMediaObject* createObject() {};
    virtual AbstractResource* createResource() {};
    
    /*------------- write interface -------------*/
    virtual void setParent(AbstractMediaObject* pParent) {}
    
    virtual void setObjectId(const std::string& objectId) {}                                    // controller object, read from xml into memory
    virtual void setParentObjectId(const std::string& objectId) {}                              // controller object, read from xml into memory
    virtual void appendChild(AbstractMediaObject* pChild) {}                                    // controller object, read from xml into memory
    virtual void setIsContainer(bool isContainer) {}                                            // controller object, read from xml into memory
    virtual void setIsRestricted(bool isRestricted) {}                                          // controller object, read from xml into memory
    virtual void setChildCount(ui4 childCount) {}                                               // controller object, read from xml into memory
    virtual void setProperty(const std::string& property, const std::string& value) {}          // controller object, read from xml into memory
    virtual void addResource(AbstractResource* pResource) {}                                    // controller object, read from xml into memory
    
    /*------------- read interface --------------*/
    virtual AbstractMediaObject* getChild(const std::string& objectId) { return 0; }
    virtual ui4 getTotalChildCount();
    
    virtual AbstractMediaObject* getParent() { return 0; }                                      // controller object, browse
    virtual AbstractResource* getResource(const std::string& resourceId) { return 0; }          // controller object, transport
    
    std::string getTitle();                                                                     // controller object, browse
    
    virtual int fetchChildren() { return 0; }                                                   // controller object, lazy browse
    bool fetchedAllChildren();                                                                  // controller object, lazy browse
    
    virtual AbstractMediaObject* getObject(const std::string& objectId);                        // server object, cds browse
    virtual ui4 getChildCount() = 0;                                                            // server object, cds browse / write meta data
                                                                                                // controller object, browse
    virtual bool isContainer() = 0;                                                             // server object, write meta data
                                                                                                // controller object, browse
    virtual AbstractMediaObject* getChild(ui4 numChild) = 0;                                    // server object, write meta data
                                                                                                // controller object, browse

    virtual std::string getParentObjectId();                                                    // server object, write meta data
    virtual std::string getObjectId() = 0;                                                      // server object, write meta data
    
    virtual bool isRestricted() = 0;                                                            // server object, write meta data

// begin depricated property API
    virtual int getPropertyCount() = 0;                                                         // server object, write meta data
    virtual int getResourceCount() = 0;                                                         // server object, write meta data
    virtual std::string getPropertyName(int numProperty) = 0;                                   // server object, write meta data
    virtual std::string getProperty(const std::string& property) = 0;                           // server object, write meta data
    virtual AbstractResource* getResource(int numResource) = 0;                                 // server object, write meta data
// end depricated property API
    
// begin new property API
// //     virtual AbstractProperty* getProperty(const std::string& name) = 0;                         // server object, write meta data
    virtual AbstractProperty* getFirstProperty() = 0;                                           // server object, write meta data
    virtual AbstractProperty* getNextProperty(AbstractProperty*) = 0;                           // server object, write meta data
// end new property API
};


// implements a memory based media object
class MemoryMediaObject : public AbstractMediaObject
{
public:
    virtual void setParent(AbstractMediaObject* pParent);

    virtual void appendChild(AbstractMediaObject* pChild);                      // controller object, read from xml into memory
    virtual void addResource(AbstractResource* pResource);                      // controller object, read from xml into memory

    /*------------- read interface --------------*/
    virtual AbstractMediaObject* getChild(const std::string& objectId);

//     virtual AbstractMediaObject* getObject(const std::string& objectId);        // server object, cds browse
    virtual ui4 getChildCount();                                                // server object, cds browse / write meta data
                                                                                // controller object, browse
    virtual bool isContainer();                                                 // server object, write meta data
                                                                                // controller object, browse
    virtual AbstractMediaObject* getChild(ui4 numChild);                        // server object, write meta data
                                                                                // controller object, browse

    virtual std::string getObjectId();                                          // server object, write meta data
    virtual int getPropertyCount();                                             // server object, write meta data
    virtual int getResourceCount();                                             // server object, write meta data
    virtual bool isRestricted();                                                // server object, write meta data
    virtual std::string getPropertyName(int numProperty);                       // server object, write meta data
    virtual std::string getProperty(const std::string& property);               // server object, write meta data
    virtual AbstractResource* getResource(int numResource);                     // server object, write meta data
    
private:
    bool                                                _restricted;
    bool                                                _isContainer;
    AbstractMediaObject*                                _parent;
    std::vector<AbstractMediaObject*>                   _childVec;
    std::map<std::string,AbstractMediaObject*>          _childMap;
    std::map<std::string,std::string>                   _propertyMap;
    std::map<std::string,AbstractResource*>             _resourceMap;
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
