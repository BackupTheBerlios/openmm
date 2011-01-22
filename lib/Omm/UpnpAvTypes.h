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
#include "Util.h"

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


// TODO: generate const strings for allowed values for action arguments with ommgen stub generator.

class AvTransportArgument
{
public:
    const static std::string TRANSPORT_STATE_STOPPED;
    const static std::string TRANSPORT_STATE_PLAYING;
    const static std::string TRANSPORT_STATE_TRANSITIONING;
    const static std::string TRANSPORT_STATE_PAUSED_PLAYBACK;
    const static std::string TRANSPORT_STATE_PAUSED_RECORDING;
    const static std::string TRANSPORT_STATE_RECORDING;
    const static std::string TRANSPORT_STATE_NO_MEDIA_PRESENT;
    const static std::string TRANSPORT_STATUS_OK;
    const static std::string TRANSPORT_STATUS_ERROR_OCCURRED;
    const static std::string PLAYBACK_STORAGE_MEDIUM_UNKNOWN;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MINI_DV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_W_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_S_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_D_VHS;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VHSC;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VIDEO8;
    const static std::string PLAYBACK_STORAGE_MEDIUM_HI8;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_ROM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_DA;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_R;
    const static std::string PLAYBACK_STORAGE_MEDIUM_CD_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_VIDEO_CD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_SACD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MD_AUDIO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MD_PICTURE;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_ROM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_VIDEO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_R;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_PLUS_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_RW;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_RAM;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DVD_AUDIO;
    const static std::string PLAYBACK_STORAGE_MEDIUM_DAT;
    const static std::string PLAYBACK_STORAGE_MEDIUM_LD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_HDD;
    const static std::string PLAYBACK_STORAGE_MEDIUM_MICRO_MV;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NETWORK;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NONE;
    const static std::string PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED;
    const static std::string CURRENT_PLAY_MODE_NORMAL;
    const static std::string CURRENT_PLAY_MODE_SHUFFLE;
    const static std::string CURRENT_PLAY_MODE_REPEAT_ONE;
    const static std::string CURRENT_PLAY_MODE_REPEAT_ALL;
    const static std::string CURRENT_PLAY_MODE_RANDOM;
    const static std::string CURRENT_PLAY_MODE_DIRECT_1;
    const static std::string CURRENT_PLAY_MODE_INTRO;
    const static std::string TRANSPORT_PLAY_SPEED_1;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_WRITABLE;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_PROTECTED;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_NOT_WRITABLE;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_UNKOWN;
    const static std::string RECORD_MEDIUM_WRITE_STATUS_NOT_IMPLEMENTED;
    const static std::string CURRENT_RECORD_QUALITY_MODE_0_EP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_1_LP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_2_SP;
    const static std::string CURRENT_RECORD_QUALITY_MODE_0_BASIC;
    const static std::string CURRENT_RECORD_QUALITY_MODE_1_MEDIUM;
    const static std::string CURRENT_RECORD_QUALITY_MODE_2_HIGH;
    const static std::string CURRENT_RECORD_QUALITY_MODE_NOT_IMPLEMENTED;
    const static std::string CURRENT_TRACK_DURATION_0;
    const static std::string CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED;
    const static std::string RELATIVE_TIME_POSITION_NOT_IMPLEMENTED;
    const static std::string CURRENT_TRANSPORT_ACTIONS_NOT_IMPLEMENTED;
    const static std::string SEEK_MODE_TRACK_NR;
    const static std::string SEEK_MODE_ABS_TIME;
    const static std::string SEEK_MODE_REL_TIME;
    const static std::string SEEK_MODE_ABS_COUNT;
    const static std::string SEEK_MODE_REL_COUNT;
    const static std::string SEEK_MODE_CHANNEL_FREQ;
    const static std::string SEEK_MODE_TAPE_INDEX;
    const static std::string SEEK_MODE_FRAME;
    const static i4 RELATIVE_COUNTER_POSITION_UNDEFINED;
};


class AvClass
{
public:
    const static std::string OBJECT;
    const static std::string ITEM;
    const static std::string IMAGE_ITEM;
    const static std::string AUDIO_ITEM;
    const static std::string VIDEO_ITEM;
    const static std::string PLAYLIST_ITEM;
    const static std::string TEXT_ITEM;
    const static std::string PHOTO;
    const static std::string MUSIC_TRACK;
    const static std::string AUDIO_BROADCAST;
    const static std::string AUDIO_BOOK;
    const static std::string MOVIE;
    const static std::string VIDEO_BROADCAST;
    const static std::string MUSIC_VIDEO_CLIP;
    const static std::string CONTAINER;
    const static std::string PERSON;
    const static std::string PLAYLIST_CONTAINER;
    const static std::string ALBUM;
    const static std::string GENRE;
    const static std::string STORAGE_SYSTEM;
    const static std::string STORAGE_VOLUME;
    const static std::string STORAGE_FOLDER;
    const static std::string MUSIC_ARTIST;
    const static std::string MUSIC_ALBUM;
    const static std::string PHOTO_ALBUM;
    const static std::string MUSIC_GENRE;
    const static std::string MOVIE_GENRE;
    
    static std::string className(const std::string& c1, const std::string& c2 = "", const std::string& c3 = "", const std::string& c4 = "", const std::string& c5 = "");
};


class AvProperty
{
public:
    const static std::string ID;
    const static std::string TITLE;
    const static std::string CREATOR;
    const static std::string RES;
    const static std::string CLASS;
    const static std::string CLASS_NAME;
    const static std::string CONTAINER_SEARCHABLE;
    const static std::string SEARCH_CLASS;
    const static std::string SEARCH_CLASS_INCLUDE_DERIVED;
    const static std::string SEARCH_CLASS_NAME;
    const static std::string CREATE_CLASS;
    const static std::string CREATE_CLASS_INCLUDE_DERIVED;
    const static std::string CREATE_CLASS_NAME;
    const static std::string PARENT_ID;
    const static std::string REF_ID;
    const static std::string RESTRICTED;
    const static std::string WRITE_STATUS;
    const static std::string CHILD_COUNT;
    const static std::string ARTIST;
    const static std::string ARTIST_ROLE;
    const static std::string ACTOR;
    const static std::string ACTOR_ROLE;
    const static std::string AUTHOR;
    const static std::string AUTHOR_ROLE;
    const static std::string PRODUCER;
    const static std::string DIRECTOR;
    const static std::string PUBLISHER;
    const static std::string CONTRIBUTOR;
    const static std::string GENRE;
    const static std::string ALBUM;
    const static std::string PLAYLIST;
    const static std::string SIZE;
    const static std::string DURATION;
    const static std::string BITRATE;
    const static std::string SAMPLE_FREQUENCY;
    const static std::string BITS_PER_SAMPLE;
    const static std::string NR_AUDIO_CHANNELS;
    const static std::string RESOLUTION;
    const static std::string COLOR_DEPTH;
    const static std::string PROTOCOL_INFO;
    const static std::string PROTECTION;
    const static std::string IMPORT_URI;
    const static std::string ALBUM_ART_URI;
    const static std::string ARTIST_DISCOGRAPHY_URI;
    const static std::string LYRICS_URI;
    const static std::string RELATION;
    const static std::string STORAGE_TOTAL;
    const static std::string STORAGE_USED;
    const static std::string STORAGE_FREE;
    const static std::string STORAGE_MAX_PARTITION;
    const static std::string STORAGE_MEDIUM;
    const static std::string DESCRIPTION;
    const static std::string LONG_DESCRIPTION;
    const static std::string ICON;
    const static std::string REGION;
    const static std::string RATING;
    const static std::string RIGHTS;
    const static std::string DATE;
    const static std::string LANGUAGE;
    const static std::string RADIO_CALL_SIGN;
    const static std::string RADIO_STATION_ID;
    const static std::string RADIO_BAND;
    const static std::string CHANNEL_NR;
    const static std::string CHANNEL_NAME;
    const static std::string SCHEDULED_START_TIME;
    const static std::string SCHEDULED_END_TIME;
    const static std::string DVD_REGION_CODE;
    const static std::string ORIGINAL_TRACK_NUMBER;
    const static std::string TOC;
    const static std::string USER_ANNOTATION;
};


class AvTypeConverter
{
public:
    static r8 readDuration(const std::string& duration);
    static std::string writeDuration(const r8& duration);
    static time readTime(const std::string& timeString);
    static std::string writeTime(const time& timeVal);
    static void replaceNonUtf8(std::string& str);
};


class PropertyImpl
{
public:
    virtual void setName(const std::string& name) {}
    virtual void setValue(const std::string& value) {}
    virtual void setAttribute(const std::string& name, const std::string& value) {}
    
    virtual std::string getName() = 0;
    virtual std::string getValue() = 0;
    virtual std::string getAttributeName(int index) { return ""; }
    virtual std::string getAttributeValue(int index) { return ""; }
    virtual std::string getAttributeValue(const std::string& name) { return ""; }
    virtual int getAttributeCount() { return 0; }
};


class AbstractProperty
{
public:
    AbstractProperty(PropertyImpl* pPropertyImpl);
    
    virtual void setName(const std::string& name);
    virtual void setValue(const std::string& value);
    virtual void setAttribute(const std::string& name, const std::string& value);
    
    virtual std::string getName();
    virtual std::string getValue();
    virtual std::string getAttributeName(int index);
    virtual std::string getAttributeValue(int index);
    virtual std::string getAttributeValue(const std::string& name);
    virtual int getAttributeCount();
    
private:
    PropertyImpl*       _pPropertyImpl;
};


class AbstractResource : public AbstractProperty
{
public:
//     AbstractResource(const std::string& uri, const std::string& protInfo, ui4 size);
    AbstractResource(PropertyImpl* pPropertyImpl);
    virtual ~AbstractResource() {}
    
    virtual void setName(const std::string& name) {}
    void setUri(const std::string& uri);
    void setProtInfo(const std::string& protInfo);
    void setSize(ui4 size);
    
    virtual std::string getName() { return "res"; }
    virtual std::string getUri();
    virtual std::string getProtInfo();
    virtual ui4 getSize();
};


class AbstractMediaObject : public Util::ConfigurablePlugin
{
public:
    AbstractMediaObject();
    virtual ~AbstractMediaObject() {};
    
    virtual AbstractMediaObject* createChildObject() { return 0; }
    virtual AbstractProperty* createProperty() { return 0; }
    virtual AbstractResource* createResource() { return 0; }
    
    /*------------- write interface -------------*/
    void setTitle(const std::string& title);
    void setClass(const std::string& subclass);
    
    void setObjectNumber(const std::string& objectId);                                          // controller object, read from xml into memory
    void setParentObjectId(const std::string& objectId);                                        // controller object, read from xml into memory
    void setObjectNumber(ui4 id);                                                               // controller object, read from xml into memory
    void setParent(AbstractMediaObject* pParent);
    void appendChild(AbstractMediaObject* pChild);                                              // controller object, read from xml into memory
    void addResource(AbstractResource* pResource);                                              // controller object, read from xml into memory

    virtual void appendChildImpl(AbstractMediaObject* pChild) {}
    // TODO: is this setTotalChildCount() ?
    virtual void setTotalChildCount(ui4 childCount) {}                                          // controller object, read from xml into memory

    virtual void setIsContainer(bool isContainer) {}                                            // controller object, read from xml into memory
    virtual void setIsRestricted(bool isRestricted) {}                                          // controller object, read from xml into memory
    virtual void addProperty(AbstractProperty* pProperty) {}                                    // controller object, read from xml into memory
    
    /*------------- read interface --------------*/
    // generic implementation
    std::string getObjectId();                                                                  // server object, write meta data
    std::string getParentObjectId();                                                            // server object, write meta data
    AbstractMediaObject* getParent();                                                           // controller object, browse
    AbstractMediaObject* getObject(const std::string& objectId);                                // server object, cds browse
    AbstractMediaObject* getChild(const std::string& objectId);
    std::string getTitle();                                                                     // controller object, browse
    std::string getClass();
    bool fetchedAllChildren();                                                                  // controller object, lazy browse
    int getResourceCount();
    void setUniqueProperty(const std::string& name, const std::string& value);

    // API for read interface
    virtual ui4 getObjectNumber(); // default implementation is returning _id
    // TODO: next two methods are only for a special form of lazy browsing
    virtual int fetchChildren();                                                                // controller object, lazy browse
    virtual ui4 getTotalChildCount();

    virtual ui4 getChildCount() { return 0; }                                                   // server object, cds browse / write meta data
                                                                                                // controller object, browse
    virtual bool isContainer() { return false; }                                                // server object, write meta data
                                                                                                // controller object, browse
    virtual AbstractMediaObject* getChild(ui4 numChild) { return 0; }                           // server object, write meta data
                                                                                                // controller object, browse
    virtual bool isRestricted() { return true; }                                                // server object, write meta data
    // TODO: title and class are mandatory properties
    virtual int getPropertyCount(const std::string& name = "") = 0;
    virtual AbstractProperty* getProperty(int index) = 0;
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0) = 0;          // server object, write meta data
    virtual AbstractResource* getResource(int index = 0);                                       // controller object, transport

private:
    // TODO: put these private members in MediaObject (aka ServerObject)
    ui4                         _id;
    AbstractMediaObject*        _pParent;
};


// MemoryProperty implements a memory based property
class MemoryPropertyImpl : public PropertyImpl
{
public:
    virtual void setName(const std::string& name);
    virtual void setValue(const std::string& value);
    virtual void setAttribute(const std::string& name, const std::string& value);
    
    virtual std::string getName();
    virtual std::string getValue();
    virtual std::string getAttributeName(int index);
    virtual std::string getAttributeValue(int index);
    virtual std::string getAttributeValue(const std::string& name);
    virtual int getAttributeCount();
    
private:
    std::string                                         _name;
    std::string                                         _value;
    std::map<std::string,std::string>                   _attrMap;
};


// MemoryMediaObject implements a memory based media object
class MemoryMediaObject : public AbstractMediaObject
{
public:
    MemoryMediaObject();
    virtual ~MemoryMediaObject();

    virtual AbstractMediaObject* createChildObject();
    virtual AbstractProperty* createProperty();
    virtual AbstractResource* createResource();
    
    virtual void setIsContainer(bool isContainer);

    virtual void appendChildImpl(AbstractMediaObject* pChild);                  // controller object, read from xml into memory
    void addProperty(AbstractProperty* pProperty);                              // controller object, read from xml into memory

    /*------------- read interface --------------*/
    virtual ui4 getChildCount();                                                // server object, cds browse / write meta data
                                                                                // controller object, browse
    virtual bool isContainer();                                                 // server object, write meta data
                                                                                // controller object, browse
    virtual AbstractMediaObject* getChild(ui4 numChild);                        // server object, write meta data
                                                                                // controller object, browse
    virtual bool isRestricted();                                                // server object, write meta data
    virtual int getPropertyCount(const std::string& name = "");
    virtual AbstractProperty* getProperty(int index);
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0);             // server object, write meta data
    
private:
    typedef std::multimap<std::string,AbstractProperty*>::iterator      PropertyIterator;
    bool                                                                _restricted;
    bool                                                                _isContainer;
    std::vector<AbstractMediaObject*>                                   _childVec;
    std::vector<AbstractProperty*>                                      _propertyVec;
    std::multimap<std::string,AbstractProperty*>                        _propertyMap;
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


/*--------------- convenience classes ------------------*/

class MemoryProperty : public AbstractProperty
{
public:
    MemoryProperty();
};


class MemoryResource : public AbstractResource
{
public:
    MemoryResource();
};


/*--------------- depricated classes ------------------*/

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

} // namespace Av
} // namespace Omm

#endif
