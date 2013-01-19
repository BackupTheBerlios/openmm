/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef UpnpAvTypes_INCLUDED
#define UpnpAvTypes_INCLUDED

#include <Poco/AutoPtr.h>
#include <Poco/Net/MediaType.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include "Poco/Data/Common.h"

#include "UpnpTypes.h"
#include "UpnpAv.h"
#include "UpnpAvLogger.h"

// NOTE: for media object ids only use non-reserved characters for segments
// (according to RFC 3986), because object ids may be used in resource URIs.
// These characters are alphanumerics, digits, "-", ".", "_", "!", ";", ",", "="

namespace Omm {

class StateVar;

namespace Av {

class AbstractProperty;
class AbstractMediaObject;

// TODO: generate const strings for allowed values for action arguments with ommgen stub generator.


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

    static std::string className(const std::string& c1 = "",
        const std::string& c2 = "",
        const std::string& c3 = "",
        const std::string& c4 = "");
    static bool matchClass(const std::string& name,
        const std::string& c1 = "",
        const std::string& c2 = "",
        const std::string& c3 = "",
        const std::string& c4 = "");
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
    /// duration in seconds
    static time readTime(const std::string& timeString);
    static std::string writeTime(const time& timeVal);
    /// timeVal in micro seconds
};


class Mime : Poco::Net::MediaType
{
public:
    Mime();
    Mime(const Mime& mime);
    Mime(const std::string& mimeString);

    const static std::string TYPE_AUDIO;
    const static std::string TYPE_VIDEO;
    const static std::string TYPE_IMAGE;
    const static std::string AUDIO_MPEG;
    const static std::string VIDEO_MPEG;
    const static std::string VIDEO_QUICKTIME;
    const static std::string VIDEO_AVI;
    const static std::string IMAGE_JPEG;
    const static std::string PLAYLIST;

    bool isAudio();
    bool isVideo();
    bool isImage();
};


class SearchCriteria
{
public:
    SearchCriteria();

    std::string parse(const std::string& searchString);
    /// provide a sublcass of SearchCriteria to convert search string into for example a usefull SQL string
    /// without that, parse returns the original searchString (after parsing it).

protected:
    static const std::string logOpAnd;
    static const std::string logOpOr;
    static const std::string relOpEqual;
    static const std::string relOpUnequal;
    static const std::string relOpLess;
    static const std::string relOpLessEqual;
    static const std::string relOpGreater;
    static const std::string relOpGreaterEqual;
    static const std::string stringOpContains;
    static const std::string stringOpContainsNot;
    static const std::string stringOpDerived;
    static const std::string existsOp;
    static const std::string boolValTrue;
    static const std::string boolValFalse;
    static const char hTab;
    static const char lineFeed;
    static const char vTab;
    static const char formFeed;
    static const char returnChar;
    static const char space;
    static const char dQuote;
    static const char asterisk;
    static const char openingBracket;
    static const char closingBracket;
    static const char escape;

    virtual std::string translateAsterisk();
    virtual std::string translateCompareExp(const std::string& property, const std::string& op, const std::string& val);
    virtual std::string translateStringExp(const std::string& property, const std::string& op, const std::string& val);
    virtual std::string translateExistsExp(const std::string& property, const std::string& op, bool val);
    virtual std::string translateLogOp(const std::string& logOp);
    virtual std::string translateProperty(const std::string& property);

private:
    void searchExp();
    void relExp();

    char peek();
    void step();
    std::string getToken();
    void skipBlanks();
    bool endOfString();

    bool isWChar(char ch);
    bool isOpeningBracket(char ch);
    bool isClosingBracket(char ch);
    bool isBracket(char ch);

    bool isLogOp(const std::string& token);
    bool isBinOp(const std::string& token);
    bool isRelOp(const std::string& token);
    bool isStringOp(const std::string& token);
    bool isExistsOp(const std::string& token);
    bool isQuotedVal(const std::string& token);
    bool isBoolVal(const std::string& token);

    std::string                 _searchString;
    std::string::size_type      _scanPos;
    std::string                 _translatedString;
};


class SqlSearchCriteria : public SearchCriteria
{
    virtual std::string translateAsterisk();
    virtual std::string translateStringExp(const std::string& property, const std::string& op, const std::string& val);
    // TODO: translateExistsExp is not handled in SqlSearchCriteria
    virtual std::string translateExistsExp(const std::string& property, const std::string& op, bool val);
};


class SortCriteria
{
    SortCriteria(const std::string& sortString = "");

    void append(AbstractProperty* pProperty, bool ascending = true);
    void remove(AbstractProperty* pProperty);

    std::string toString();
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

protected:
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
    virtual std::streamsize getSize();
};


class AbstractMediaObject
{
public:
    virtual ~AbstractMediaObject() {};

    // factory methods
    virtual AbstractMediaObject* createChildObject() { return 0; }
    virtual AbstractProperty* createProperty() { return 0; }
    virtual AbstractResource* createResource() { return 0; }

    virtual std::string getId() { return ""; }
    virtual void setId(const std::string& id) {}
    virtual std::string getParentId() { return ""; }

    // attributes
    virtual bool isRestricted() { return true; }
    virtual void setIsRestricted(bool isRestricted) {}
    virtual bool isSearchable() { return false; }
    virtual void setIsSearchable(bool isSearchable = true) {}

    // properties
    std::string getTitle();
    void setTitle(const std::string& title);
    std::string getClass();
    void setClass(const std::string& subclass);
    virtual void addProperty(AbstractProperty* pProperty) {}
    // TODO: title and class are mandatory properties
    virtual int getPropertyCount(const std::string& name = "") = 0;
    virtual AbstractProperty* getProperty(int index) = 0;
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0) = 0;
    virtual void setUniqueProperty(const std::string& name, const std::string& value);

    // resources
    int getResourceCount();
    void addResource(AbstractResource* pResource);
    virtual AbstractResource* getResource(int index = 0);

    // parent and descendants
    virtual bool isContainer() { return false; }
    virtual void setIsContainer(bool isContainer) {}
    virtual ui4 getChildCount() { return 0; }
    virtual void setChildCount(ui4 childCount) {}
    virtual CsvList* getSortCaps() { return 0; }
    virtual CsvList* getSearchCaps() { return 0; }
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
    MemoryMediaObject(const MemoryMediaObject& mediaObject);
    virtual ~MemoryMediaObject();

    // factory methods
    virtual AbstractMediaObject* createChildObject();
    virtual AbstractProperty* createProperty();
    virtual AbstractResource* createResource();

    // attributes
    virtual bool isRestricted();
    virtual bool isSearchable();
    void setIsRestricted(bool restricted = true);
    void setIsSearchable(bool searchable = true);

    // properties
    virtual int getPropertyCount(const std::string& name = "");
    void addProperty(AbstractProperty* pProperty);
    virtual AbstractProperty* getProperty(int index);
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0);

    // descendants
    virtual bool isContainer();
    virtual void setIsContainer(bool isContainer);
    virtual ui4 getFetchedChildCount();
    virtual ui4 getChildCount();
    virtual void setChildCount(ui4 childCount);
    virtual AbstractMediaObject* getChildForRow(ui4 row);

protected:
    typedef std::multimap<std::string,AbstractProperty*>::iterator      PropertyIterator;

    bool                                                                _restricted;
    bool                                                                _isContainer;
    bool                                                                _searchable;
    std::vector<AbstractMediaObject*>                                   _childVec;
    std::vector<AbstractProperty*>                                      _propertyVec;
    std::multimap<std::string,AbstractProperty*>                        _propertyMap;
    ui4                                                                 _childCount;
};


class MediaObjectReader
{
public:
    MediaObjectReader();

    void read(AbstractMediaObject* pObject, const std::string& metaData);
    void readChildren(std::vector<AbstractMediaObject*>& children, const std::string& metaData, AbstractMediaObject* pContainer);

private:
    void readNode(AbstractMediaObject* pObject, Poco::XML::Node* pNode);
};


class MediaObjectWriter
{
public:
    MediaObjectWriter(bool full = true);

    void write(std::string& meta, AbstractMediaObject* pObject, const std::string& filter = "*");
    void writeChildren(std::string& meta, const std::vector<AbstractMediaObject*>& children, const std::string& filter = "*");

    static const std::string& getXmlProlog();

protected:
    void writeMetaDataHeader();
    void writeMetaDataClose(std::string& metaData);
    void writeMetaData(Poco::XML::Element* pDidl, AbstractMediaObject* pObject);

    Poco::AutoPtr<Poco::XML::Document>      _pDoc;
    Poco::AutoPtr<Poco::XML::Element>       _pDidl;
    bool                                    _full;
    static const std::string                _xmlProlog;
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


/*--------------- media object caches ------------------*/


class AbstractMediaObjectCache
{
public:
    AbstractMediaObjectCache(ui4 maxCacheSize = 100);

//    virtual ui4 getTotalCount() { return 0; }
    virtual void clear() {}
    virtual AbstractMediaObject* getMediaObjectForRow(ui4 row) { return 0; }
    virtual AbstractMediaObject* getMediaObjectForIndex(ui4 index) { return 0; }

    void setMaxCacheSize(ui4 size);
    ui4 getMaxCacheSize();

protected:
    virtual ui4 getCacheSize() { return 0; }

private:
    ui4                         _maxCacheSize;
};


class BlockCache : public AbstractMediaObjectCache
/// BlockCache guarantees to have maxCacheSize number of adjacent objects in the
/// cache, if they are all accessed via getMediaObject().
{
public:
    BlockCache(ui4 blockSize = 10);

    virtual AbstractMediaObject* getMediaObjectForRow(ui4 row);
    virtual void erase(std::vector<AbstractMediaObject*>::iterator begin, std::vector<AbstractMediaObject*>::iterator end);
    virtual void clear();

    void setBlockSize(ui4 blockSize);

protected:
    virtual void getBlock(std::vector<AbstractMediaObject*>& block, ui4 offset, ui4 size) {}
    void insertBlock(std::vector<AbstractMediaObject*>& block, bool prepend = false);

private:
    virtual ui4 getCacheSize();

    std::vector<AbstractMediaObject*>   _cache;
    ui4                                 _offset;
    ui4                                 _blockSize;
};


} // namespace Av
} // namespace Omm

#endif
