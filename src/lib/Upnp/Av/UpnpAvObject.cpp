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

#include <cmath>
#include <sstream>
#include <iostream>

#ifdef POCO_VERSION_HEADER_FOUND
#include <Poco/Version.h>
#endif
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/Data/Session.h>
#include "Poco/Data/SQLite/Connector.h"
#include "Poco/Data/RecordSet.h"

#include "UpnpAvObject.h"

#include "utf8.h"


namespace Omm {
namespace Av {


const std::string AvClass::OBJECT = "object";

const std::string AvClass::ITEM = "item";
const std::string AvClass::IMAGE_ITEM = "imageItem";
const std::string AvClass::AUDIO_ITEM = "audioItem";
const std::string AvClass::VIDEO_ITEM = "videoItem";
const std::string AvClass::PLAYLIST_ITEM = "playlistItem";
const std::string AvClass::TEXT_ITEM = "textItem";
const std::string AvClass::PHOTO = "photo";
const std::string AvClass::MUSIC_TRACK = "musicTrack";
const std::string AvClass::AUDIO_BROADCAST = "audioBroadcast";
const std::string AvClass::AUDIO_BOOK = "audioBook";
const std::string AvClass::MOVIE = "movie";
const std::string AvClass::VIDEO_BROADCAST = "videoBroadcast";
const std::string AvClass::MUSIC_VIDEO_CLIP = "musicVideoClip";

const std::string AvClass::CONTAINER = "container";
const std::string AvClass::PERSON = "person";
const std::string AvClass::PLAYLIST_CONTAINER = "playlistContainer";
const std::string AvClass::ALBUM = "album";
const std::string AvClass::GENRE = "genre";
const std::string AvClass::STORAGE_SYSTEM = "storageSystem";
const std::string AvClass::STORAGE_VOLUME = "storageVolume";
const std::string AvClass::STORAGE_FOLDER = "storageFolder";
const std::string AvClass::MUSIC_ARTIST = "musicArtist";
const std::string AvClass::MUSIC_ALBUM = "musicAlbum";
const std::string AvClass::PHOTO_ALBUM = "photoAlbum";
const std::string AvClass::MUSIC_GENRE = "musicGenre";
const std::string AvClass::MOVIE_GENRE = "movieGenre";


const std::string AvProperty::ID = "id";
const std::string AvProperty::TITLE = "dc:title";
const std::string AvProperty::CREATOR = "dc:creator";
const std::string AvProperty::RES = "res";
const std::string AvProperty::CLASS = "upnp:class";
const std::string AvProperty::CLASS_NAME = "upnp:name";
const std::string AvProperty::CONTAINER_SEARCHABLE = "searchable";
const std::string AvProperty::SEARCH_CLASS = "upnp:searchClass";
const std::string AvProperty::SEARCH_CLASS_INCLUDE_DERIVED = "upnp:includeDerived";
const std::string AvProperty::SEARCH_CLASS_NAME = "upnp:name";
const std::string AvProperty::CREATE_CLASS = "upnp:createClass";
const std::string AvProperty::CREATE_CLASS_INCLUDE_DERIVED = "upnp:includeDerived";
const std::string AvProperty::CREATE_CLASS_NAME = "upnp:name";
const std::string AvProperty::PARENT_ID = "parentID";
const std::string AvProperty::REF_ID = "refID";
const std::string AvProperty::RESTRICTED = "restricted";
const std::string AvProperty::WRITE_STATUS = "upnp:writeStatus";
const std::string AvProperty::CHILD_COUNT = "childCount";
const std::string AvProperty::ARTIST = "upnp:artist";
const std::string AvProperty::ARTIST_ROLE = "upnp:role";
const std::string AvProperty::ACTOR = "upnp:actor";
const std::string AvProperty::ACTOR_ROLE = "upnp:role";
const std::string AvProperty::AUTHOR = "upnp:author";
const std::string AvProperty::AUTHOR_ROLE = "upnp:role";
const std::string AvProperty::PRODUCER = "upnp:producer";
const std::string AvProperty::DIRECTOR = "upnp:director";
const std::string AvProperty::PUBLISHER = "publisher";
const std::string AvProperty::CONTRIBUTOR = "contributor";
const std::string AvProperty::GENRE = "upnp:genre";
const std::string AvProperty::ALBUM = "upnp:album";
const std::string AvProperty::PLAYLIST = "upnp:playlist";
const std::string AvProperty::SIZE = "size";
const std::string AvProperty::DURATION = "duration";
const std::string AvProperty::BITRATE = "bitrate";
const std::string AvProperty::SAMPLE_FREQUENCY = "sampleFrequency";
const std::string AvProperty::BITS_PER_SAMPLE = "bitsPerSample";
const std::string AvProperty::NR_AUDIO_CHANNELS = "nrAudioChannels";
const std::string AvProperty::RESOLUTION = "resolution";
const std::string AvProperty::COLOR_DEPTH = "colorDepth";
const std::string AvProperty::PROTOCOL_INFO = "protocolInfo";
const std::string AvProperty::PROTECTION = "protection";
const std::string AvProperty::IMPORT_URI = "importUri";
const std::string AvProperty::ALBUM_ART_URI = "upnp:albumArtURI";
const std::string AvProperty::ARTIST_DISCOGRAPHY_URI = "upnp:artistDiscographyURI";
const std::string AvProperty::LYRICS_URI = "upnp:lyricsURI";
const std::string AvProperty::RELATION = "relation";
const std::string AvProperty::STORAGE_TOTAL = "upnp:storageTotal";
const std::string AvProperty::STORAGE_USED = "upnp:storageUsed";
const std::string AvProperty::STORAGE_FREE = "upnp:storageFree";
const std::string AvProperty::STORAGE_MAX_PARTITION = "upnp:storageMaxPartition";
const std::string AvProperty::STORAGE_MEDIUM = "upnp:storageMedium";
const std::string AvProperty::DESCRIPTION = "description";
const std::string AvProperty::LONG_DESCRIPTION = "upnp:longDescription";
const std::string AvProperty::ICON = "upnp:icon";
const std::string AvProperty::REGION = "upnp:region";
const std::string AvProperty::RATING = "upnp:rating";
const std::string AvProperty::RIGHTS = "rights";
const std::string AvProperty::DATE = "date";
const std::string AvProperty::LANGUAGE = "language";
const std::string AvProperty::RADIO_CALL_SIGN = "upnp:radioCallSign";
const std::string AvProperty::RADIO_STATION_ID = "upnp:radioStationID";
const std::string AvProperty::RADIO_BAND = "upnp:radioBand";
const std::string AvProperty::CHANNEL_NR = "upnp:channelNr";
const std::string AvProperty::CHANNEL_NAME = "upnp:channelName";
const std::string AvProperty::SCHEDULED_START_TIME = "upnp:scheduledStartTime";
const std::string AvProperty::SCHEDULED_END_TIME = "upnp:scheduledEndTime";
const std::string AvProperty::DVD_REGION_CODE = "upnp:DVDRegionCode";
const std::string AvProperty::ORIGINAL_TRACK_NUMBER = "upnp:originalTrackNumber";
const std::string AvProperty::TOC = "upnp:toc";
const std::string AvProperty::USER_ANNOTATION = "upnp:userAnnotation";

const std::string Mime::TYPE_AUDIO = "audio";
const std::string Mime::TYPE_VIDEO = "video";
const std::string Mime::TYPE_IMAGE = "image";
const std::string Mime::AUDIO_MPEG = Mime::TYPE_AUDIO + "/mpeg";
const std::string Mime::VIDEO_MPEG = Mime::TYPE_VIDEO + "/mpeg";
const std::string Mime::VIDEO_QUICKTIME = Mime::TYPE_VIDEO + "/quicktime";
const std::string Mime::VIDEO_AVI = Mime::TYPE_VIDEO + "/avi";
const std::string Mime::IMAGE_JPEG = Mime::TYPE_IMAGE + "/jpeg";

std::string
AvClass::className(const std::string& c1, const std::string& c2, const std::string& c3, const std::string& c4)
{
    if (c1 == "") {
        return "object";
    }
    else if (c2 == "") {
        return "object." + c1;
    }
    else if (c3 == "") {
        return "object." + c1 + "." + c2;
    }
    else if (c4 == "") {
        return "object." + c1 + "." + c2 + "." + c3;
    }
    else {
        return "object." + c1 + "." + c2 + "." + c3 + "." + c4;
    }
}


bool
AvClass::matchClass(const std::string& name, const std::string& c1, const std::string& c2 , const std::string& c3, const std::string& c4)
{
    std::string matchName = className(c1, c2, c3, c4);
 //   Log::instance()->upnpav().debug("matching class: " + name + " with class: " + matchName);

    return (name.substr(0, matchName.length()) == matchName);
}


r8
AvTypeConverter::readDuration(const std::string& duration)
{
    if (duration == "NOT_IMPLEMENTED") {
        return 0.0;
    }
    Poco::StringTokenizer tok(duration, ":./");
    r8 res = 3600.0 * Poco::NumberParser::parse(tok[0])
             + 60.0 * Poco::NumberParser::parse(tok[1])
             + Poco::NumberParser::parse(tok[2]);

    if (tok.count() == 4) {
        res += Poco::NumberParser::parseFloat(tok[3]) / pow(10, tok[3].length());
    }
    else if (tok.count() == 5) {
        res += Poco::NumberParser::parse(tok[3]) / Poco::NumberParser::parse(tok[4]);
    }
    return res;
}


std::string
AvTypeConverter::writeDuration(const r8& duration)
{
    int hours = duration / 3600.0;
    int minutes = (duration - hours * 3600) / 60.0;
    r8 seconds = duration - hours * 3600 - minutes * 60;
    return
        Poco::NumberFormatter::format(hours) + ":" +
        Poco::NumberFormatter::format0(minutes, 2) + ":" +
        Poco::NumberFormatter::format(seconds, 2);
}


time
AvTypeConverter::readTime(const std::string& timeString)
{
    Poco::DateTime res;
    int timeZoneDifferential;
    // fill date part of ISO8601 date format with epoch date
    try {
        Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FORMAT, "1970-01-01T" + timeString, res, timeZoneDifferential);
    }
    catch (Poco::SyntaxException& e) {
        Log::instance()->upnpav().debug("parsing time failed: " + e.message());
        return 0;
    }
    return res.timestamp();
}


std::string
AvTypeConverter::writeTime(const time& timeVal)
{
    return Poco::DateTimeFormatter::format(timeVal, Poco::DateTimeFormat::ISO8601_FORMAT, 0).substr(11);
}


void
AvTypeConverter::replaceNonUtf8(std::string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), std::back_inserter(temp));
    str = temp;
}


Mime::Mime() :
Poco::Net::MediaType("")
{

}


Mime::Mime(const Mime& mime) :
Poco::Net::MediaType(mime)
{

}


Mime::Mime(const std::string& mimeString) :
Poco::Net::MediaType(mimeString)
{

}


bool
Mime::isAudio()
{
    Log::instance()->upnpav().debug("Mime type: " + getType());
    return getType() == TYPE_AUDIO;
}


bool
Mime::isVideo()
{
    Log::instance()->upnpav().debug("Mime type: " + getType());
    return getType() == TYPE_VIDEO;
}


bool
Mime::isImage()
{
    Log::instance()->upnpav().debug("Mime type: " + getType());
    return getType() == TYPE_IMAGE;
}


ProtocolInfo::ProtocolInfo() :
_mime(""),
_dlna("")
{
}


ProtocolInfo::ProtocolInfo(const ProtocolInfo& protInfo) :
_mime(protInfo._mime),
_dlna(protInfo._dlna)
{
}


ProtocolInfo::ProtocolInfo(const std::string& infoString)
{
    Log::instance()->upnpav().debug("Protocol Info: " + infoString);

    Poco::StringTokenizer infoTokens(infoString, ":");
    try {
        _mime = infoTokens[2];
        _dlna = infoTokens[3];
    }
    catch (Poco::RangeException) {
    }
    Log::instance()->upnpav().debug("Protocol Info mime: " + _mime + ", dlna: " + _dlna);
}


std::string
ProtocolInfo::getMimeString() const
{
    return _mime;
}


std::string
ProtocolInfo::getDlnaString() const
{
    return _dlna;
}


AbstractResource::AbstractResource(PropertyImpl* pPropertyImpl) :
AbstractProperty(pPropertyImpl)
{
}


std::string
AbstractResource::getUri()
{
    return getValue();
}


std::string
AbstractResource::getProtInfo()
{
    return getAttributeValue(AvProperty::PROTOCOL_INFO);
}


std::streamsize
AbstractResource::getSize()
{
    return Poco::NumberParser::parse(getAttributeValue(AvProperty::SIZE));
}


void
AbstractResource::setUri(const std::string& uri)
{
    setValue(uri);
}


void
AbstractResource::setProtInfo(const std::string& protInfo)
{
    setAttribute(AvProperty::PROTOCOL_INFO, protInfo);
}


void
AbstractResource::setSize(ui4 size)
{
    Log::instance()->upnpav().debug("abstract resource set size: " + Poco::NumberFormatter::format(size));

    setAttribute(AvProperty::SIZE, Poco::NumberFormatter::format(size));
}


AbstractProperty::AbstractProperty(PropertyImpl* pPropertyImpl) :
_pPropertyImpl(pPropertyImpl)
{
}


void
AbstractProperty::setName(const std::string& name)
{
    _pPropertyImpl->setName(name);
}


void
AbstractProperty::setValue(const std::string& value)
{
    _pPropertyImpl->setValue(value);
}


void
AbstractProperty::setAttribute(const std::string& name, const std::string& value)
{
    _pPropertyImpl->setAttribute(name, value);
}


std::string
AbstractProperty::getName()
{
    return _pPropertyImpl->getName();
}


std::string
AbstractProperty::getValue()
{
    return _pPropertyImpl->getValue();
}


std::string
AbstractProperty::getAttributeName(int index)
{
    return _pPropertyImpl->getAttributeName(index);
}


std::string
AbstractProperty::getAttributeValue(int index)
{
    return _pPropertyImpl->getAttributeValue(index);
}


std::string
AbstractProperty::getAttributeValue(const std::string& name)
{
    return _pPropertyImpl->getAttributeValue(name);
}


int
AbstractProperty::getAttributeCount()
{
    return _pPropertyImpl->getAttributeCount();
}


AbstractMediaObjectCache::AbstractMediaObjectCache(ui4 maxCacheSize) :
_maxCacheSize(maxCacheSize),
_scan(false)
{

}


void
AbstractMediaObjectCache::setMaxCacheSize(ui4 size)
{
    _maxCacheSize = size;
}


ui4
AbstractMediaObjectCache::getMaxCacheSize()
{
    return _maxCacheSize;
}


void
AbstractMediaObjectCache::scan(bool on)
{
    if (on && _scan) {
        return;
    }
    _scan = on;
    if (on && (_maxCacheSize < getCacheSize())) {
        doScan();
    }
}


BlockCache::BlockCache(ui4 blockSize) :
_offset(0),
_blockSize(blockSize)
{
}


AbstractMediaObject*
BlockCache::getMediaObjectForRow(ui4 row)
{
    Log::instance()->upnpav().debug("block cache get media object in row: " + Poco::NumberFormatter::format(row) + " ...");

    if (_offset <= row && row < _offset + getCacheSize()) {
        // cache hit, do nothing
        Log::instance()->upnpav().debug("block cache get media object cache hit");
    }
    else if (_offset - row < _blockSize && _offset - row > 0) {
        // near cache miss, insert block before current cache
        int blockSize = std::min(_blockSize, _offset);
        Log::instance()->upnpav().debug("block cache get media object cache near miss before");
        if (getCacheSize() + blockSize > getMaxCacheSize()) {
            Log::instance()->upnpav().debug("block cache get media object deleting block at back");
            erase(_cache.end() - blockSize, _cache.end());
        }
        std::vector<AbstractMediaObject*> block;
        getBlock(block, _offset - blockSize, blockSize);
        insertBlock(block, true);
        _offset -= blockSize;
    }
    else if (row - (_offset + getCacheSize()) < _blockSize && row - (_offset + getCacheSize()) >= 0) {
        // near cache miss, insert block after current cache
        Log::instance()->upnpav().debug("block cache get media object cache near miss after");
        if (getCacheSize() + _blockSize > getMaxCacheSize()) {
            Log::instance()->upnpav().debug("block cache get media object deleting block at front");
            erase(_cache.begin(), _cache.begin() + _blockSize);
            _offset += _blockSize;
        }
        std::vector<AbstractMediaObject*> block;
        getBlock(block, _offset + getCacheSize(), _blockSize);
        insertBlock(block, false);
    }
    else {
        // far cache miss, rebuild whole cache
        Log::instance()->upnpav().debug("block cache get media object cache far miss");
        clear();
        std::vector<AbstractMediaObject*> block;
        getBlock(block, row, _blockSize);
        insertBlock(block);
        _offset = row;
//      scan(on);
    }
    Log::instance()->upnpav().debug("block cache get media object finished.");
    if (_cache.size() > row - _offset) {
        return _cache[row - _offset];
    }
    else {
        Log::instance()->upnpav().error("block cache get media object failed, cache not large enough.");
        return 0;
    }
}


void
BlockCache::erase(std::vector<AbstractMediaObject*>::iterator begin, std::vector<AbstractMediaObject*>::iterator end)
{
    for (std::vector<AbstractMediaObject*>::iterator it = begin; it != end; ++it) {
        delete *it;
    }
    _cache.erase(begin, end);
}


void
BlockCache::clear()
{
    for (std::vector<AbstractMediaObject*>::iterator it = _cache.begin(); it != _cache.end(); ++it) {
        delete *it;
    }
    _cache.clear();
}


void
BlockCache::setBlockSize(ui4 blockSize)
{
    _blockSize = blockSize;
}


void
BlockCache::insertBlock(std::vector<AbstractMediaObject*>& block, bool prepend)
{
    Log::instance()->upnpav().debug(std::string("block cache inserting block at ") + (prepend ? "front" : "back") + "...");
    if (prepend) {
        _cache.insert(_cache.begin(), block.begin(), block.begin() + block.size());
    }
    else {
        _cache.insert(_cache.end(), block.begin(), block.begin() + block.size());
    }
    Log::instance()->upnpav().debug("block cache inserting block finished.");
}


ui4
BlockCache::getCacheSize()
{
    return _cache.size();
}


void
BlockCache::doScan(bool on)
{

}


DatabaseCache::DatabaseCache(const std::string& databaseFile)
{
    Poco::Data::SQLite::Connector::registerConnector();
    _pSession = new Poco::Data::Session("SQLite", databaseFile);
    try {
        *_pSession << "CREATE TABLE objcache (idx INTEGER(4), class VARCHAR(30), title VARCHAR, artist VARCHAR, album VARCHAR, xml VARCHAR)",
                Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating object cache table failed: " + e.displayText());
    }
    try {
        *_pSession << "CREATE UNIQUE INDEX idx ON objcache (idx)", Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache creating index on object cache table failed: " + e.displayText());
    }
}


DatabaseCache::~DatabaseCache()
{
    delete _pSession;
    Poco::Data::SQLite::Connector::unregisterConnector();
}


AbstractMediaObject*
DatabaseCache::getMediaObjectForRow(ui4 row)
{
    Log::instance()->upnpav().debug("database cache get object for row: " + Poco::NumberFormatter::format(row));

    ui4 index;
    std::string xml;
    try {
        Poco::Data::Statement select(*_pSession);
        select << "SELECT idx, xml FROM objcache";
        select.execute();
        Poco::Data::RecordSet recordSet(select);
        recordSet.moveFirst();
        for (ui4 r = 0; r < row; r++) {
            recordSet.moveNext();
        }
        index = recordSet["idx"].convert<ui4>();
        xml = recordSet["xml"].convert<std::string>();
        // NOTE: use recordSet.rowCount() for number of rows in query result
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache get object for row failed: " + e.displayText());
    }
    AbstractMediaObject* pObject = createMediaObject();
    MediaObjectReader xmlReader(pObject);
    xmlReader.read(xml);
    pObject->setIndex(index);
    return pObject;
}


AbstractMediaObject*
DatabaseCache::getMediaObjectForIndex(ui4 index)
{
    Log::instance()->upnpav().debug("database cache get object for index: " + Poco::NumberFormatter::format(index));

    std::vector<std::string> xml;
    try {
        *_pSession << "SELECT xml FROM objcache WHERE idx = :index", Poco::Data::use(index), Poco::Data::into(xml), Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().warning("database cache get object for index failed: " + e.displayText());
    }
    if (xml.size() == 1) {
        AbstractMediaObject* pObject = createMediaObject();
        MediaObjectReader xmlReader(pObject);
        xmlReader.read(xml[0]);
        pObject->setIndex(index);
        return pObject;
    }
    else {
        Log::instance()->upnpav().warning("database cache get object for index reading meta data failed.");
        return 0;
    }
}


void
DatabaseCache::doScan(bool on)
{

}


void
DatabaseCache::insertMediaObject(AbstractMediaObject* pObject)
{
    Log::instance()->upnpav().debug("database cache inserting media object with index: " + Poco::NumberFormatter::format(pObject->getIndex()));
    std::string xml;
    MediaObjectWriter2 xmlWriter(pObject);
    xmlWriter.write(xml);
    try {
//        *_pSession << "INSERT INTO objcache (idx, class, title, artist, album, xml) VALUES(:idx, :class, :title, :artist, :album, :xml)",
        *_pSession << "INSERT INTO objcache (idx, class, title, xml) VALUES(:idx, :class, :title, :xml)",
                Poco::Data::use(pObject->getIndex()),
                Poco::Data::use(pObject->getClass()),
                Poco::Data::use(pObject->getTitle()),
//                Poco::Data::use(pObject->getProperty(AvProperty::ARTIST)->getValue()),
//                Poco::Data::use(pObject->getProperty(AvProperty::ALBUM)->getValue()),
                Poco::Data::use(xml),
                Poco::Data::now;
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().debug("database cache inserting media object failed: " + e.displayText());
    }
    delete pObject;
}


AbstractMediaObject::AbstractMediaObject() :
_index(0),
_pParent(0)
{
}


void
AbstractMediaObject::setIndex(ui4 index)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setObjectNumber() objectId: " + Poco::NumberFormatter::format(id));

    _index = index;
}


void
AbstractMediaObject::setIndex(const std::string& index)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setObjectId() from string: " + id);

    _index = Poco::NumberParser::parseUnsigned(index);
}


void
AbstractMediaObject::setParent(AbstractMediaObject* pParent)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setParent()");

    _pParent = pParent;
}


void
AbstractMediaObject::appendChild(AbstractMediaObject* pChild)
{
//    pChild->setIndex(getChildCount());
//    pChild->setIndex(index);
    pChild->setParent(this);
    appendChildImpl(pChild);
}


void
AbstractMediaObject::appendChildWithAutoIndex(AbstractMediaObject* pChild)
{
    pChild->setIndex(getChildCount());
    appendChild(pChild);
}


void
AbstractMediaObject::setUniqueProperty(const std::string& name, const std::string& value)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setUniqueProperty() name: " +  name + ", value: " + value);

    // TODO: check if property title is already set
    AbstractProperty* pProp = createProperty();
    pProp->setName(name);
    pProp->setValue(value);
    addProperty(pProp);
}


void
AbstractMediaObject::setTitle(const std::string& title)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setTitle() title: " + title);

    setUniqueProperty(AvProperty::TITLE, title);
}


void
AbstractMediaObject::setClass(const std::string& subclass)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setClass() subclass: " + subclass);

    setUniqueProperty(AvProperty::CLASS, subclass);
}


std::string
AbstractMediaObject::getTitle()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getTitle()");

    return getProperty(AvProperty::TITLE)->getValue();
}


std::string
AbstractMediaObject::getClass()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getClass()");

    return getProperty(AvProperty::CLASS)->getValue();
}


AbstractMediaObject*
AbstractMediaObject::getChildForIndex(const std::string& index)
{
    return getChildForIndex(Poco::NumberParser::parseUnsigned(index));
}


AbstractMediaObject*
AbstractMediaObject::getDescendant(const std::string& objectId)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::setObject() objectId: " + objectId);

    // TODO: what about this object, not only child objects?

    std::string::size_type slashPos = objectId.find('/');
    AbstractMediaObject* pChild;
    if (slashPos != std::string::npos) {
        // child is a container
        pChild = getChildForIndex(objectId.substr(0, slashPos));
        if (pChild == 0) {
            // child container is not a child of this container, we try the full path
            pChild = getChildForIndex(objectId);
            if (pChild == 0) {
                // child container is not a child of this container
                Log::instance()->upnpav().error("retrieving child objectId of container, but no child container found");
                return 0;
            }
            else {
                // object id of child contains slashes (subtree's implementation is a faked tree with only item objects).
                return pChild;
            }
        }
        else {
            // recurse into child container
            return pChild->getDescendant(objectId.substr(slashPos + 1));
        }
    }
    else {
        // child is an item
        pChild = getChildForIndex(objectId);
        if (pChild == 0) {
            // child item is not a child of this container
            Log::instance()->upnpav().error("no child item found");
            return 0;
        }
        else {
            // return child item and stop recursion
            return pChild;
        }
    }
}


ui4
AbstractMediaObject::getIndex()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getIndex index: " + Poco::NumberFormatter::format(_index));

    return _index;
}


std::string
AbstractMediaObject::getId()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getObjectId()");

    AbstractMediaObject* pParent = getParent();
    if (pParent == 0) {
        return "0";
    }
    else {
        return pParent->getId() + "/" + Poco::NumberFormatter::format(getIndex());
    }
}


bool
AbstractMediaObject::fetchedAllChildren()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::fetchedAllChildren()");

    return getChildCount() >= getTotalChildCount();
}


AbstractMediaObject*
AbstractMediaObject::getParent()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getParent()");

    return _pParent;
}


std::string
AbstractMediaObject::getParentId()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getParentObjectId()");

    AbstractMediaObject* pParent = getParent();
    if (pParent) {
        return pParent->getId();
    }
    else {
        return "";
    }
}


int
AbstractMediaObject::fetchChildren()
{
    return 0;
}


ui4
AbstractMediaObject::getTotalChildCount()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getTotalChildCount()");

    return getChildCount();
}


AbstractResource*
AbstractMediaObject::getResource(int index)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getResource() index: " + Poco::NumberFormatter::format(index));

    return static_cast<AbstractResource*>(getProperty(AvProperty::RES, index));
}


void
AbstractMediaObject::addResource(AbstractResource* pResource)
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::addResource()");

    addProperty(pResource);
}


int
AbstractMediaObject::getResourceCount()
{
//    Log::instance()->upnpav().debug("AbstractMediaObject::getResourceCount()");

    return getPropertyCount(AvProperty::RES);
}


void
MemoryPropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
MemoryPropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


void
MemoryPropertyImpl::setAttribute(const std::string& name, const std::string& value)
{
    _attrMap[name] = value;
}


std::string
MemoryPropertyImpl::getName()
{
    return _name;
}


std::string
MemoryPropertyImpl::getValue()
{
    return _value;
}


std::string
MemoryPropertyImpl::getAttributeName(int index)
{
    int i = 0;
    std::map<std::string,std::string>::iterator it;
    for (it = _attrMap.begin(); (it != _attrMap.end()) && (i < index); ++it, ++i) {
    }
    return (*it).first;
}


std::string
MemoryPropertyImpl::getAttributeValue(int index)
{
    int i = 0;
    std::map<std::string,std::string>::iterator it;
    for (it = _attrMap.begin(); (it != _attrMap.end()) && (i < index); ++it, ++i) {
    }
    return (*it).second;
}


std::string
MemoryPropertyImpl::getAttributeValue(const std::string& name)
{
    return _attrMap[name];
}


int
MemoryPropertyImpl::getAttributeCount()
{
    return _attrMap.size();
}


MemoryMediaObject::MemoryMediaObject() :
_restricted(true),
_isContainer(false),
_totalChildCount(0)
//_rowOffset(0)
{
}


MemoryMediaObject::~MemoryMediaObject()
{
}


AbstractMediaObject*
MemoryMediaObject::createChildObject()
{
    return new MemoryMediaObject;
}


AbstractProperty*
MemoryMediaObject::createProperty()
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::createProperty()");

    return new AbstractProperty(new MemoryPropertyImpl);
}


AbstractResource*
MemoryMediaObject::createResource()
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::createResource()");

    AbstractResource* pResource = new MemoryResource;
    // FIXME: createResouce() should only create a resource and not add it.
    addResource(pResource);
    return pResource;
}


void
MemoryMediaObject::setIsContainer(bool isContainer)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::setIsContainer()");

    _isContainer = isContainer;
}


void
MemoryMediaObject::setTotalChildCount(ui4 childCount)
{
    _totalChildCount = childCount;
}


void
MemoryMediaObject::appendChildImpl(AbstractMediaObject* pChild)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::appendChildImpl()");

    _childVec.push_back(pChild);
}


ui4
MemoryMediaObject::getChildCount()
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::getChildCount()");

    return _childVec.size();
//    return _rowOffset + _childVec.size();
}


ui4
MemoryMediaObject::getTotalChildCount()
{
    return _totalChildCount;
}


bool
MemoryMediaObject::isContainer()
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::isContainer()");

    return _isContainer;
}


AbstractMediaObject*
MemoryMediaObject::getChildForRow(ui4 row)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::getChild() number: " + Poco::NumberFormatter::format(numChild));

//    ui4 childRow = row - _rowOffset;
    ui4 childRow = row;
    if (childRow < _childVec.size() && childRow >= 0) {
        return _childVec[childRow];
    }
    else {
        Log::instance()->upnpav().error("MemoryMediaObject::getChildForRow() row out of range: " + Poco::NumberFormatter::format(row));
    }
}


bool
MemoryMediaObject::isRestricted()
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::isRestricted()");

    return _restricted;
}


void
MemoryMediaObject::addProperty(AbstractProperty* pProperty)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::addProperty()");

    _propertyVec.push_back(pProperty);
    _propertyMap.insert(make_pair(pProperty->getName(), pProperty));
}


AbstractProperty*
MemoryMediaObject::getProperty(int index)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::getProperty() index: " + Poco::NumberFormatter::format(index));

    return _propertyVec[index];
}


int
MemoryMediaObject::getPropertyCount(const std::string& name)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::getPropertyCount() name: " + name);

    if (name == "") {
        return _propertyMap.size();
    }
    else {
        return _propertyMap.count(name);
    }
}


AbstractProperty*
MemoryMediaObject::getProperty(const std::string& name, int index)
{
//    Log::instance()->upnpav().debug("MemoryMediaObject::getProperty() name: " + name + ", index: " + Poco::NumberFormatter::format(index));

    // FIXME: workaround, seeking std::multimap returns null-pointer on iphone (but not iphone-simulator or any other system)
    // so we just search the first one (for now, this is ok as we provide only one resource in our servers).
    PropertyIterator it = _propertyMap.find(name);
    if (it != _propertyMap.end()) {
        return (*it).second;
    }
    else {
        return 0;
    }

//    std::pair<PropertyIterator,PropertyIterator> range = _propertyMap.equal_range(name);
//    int i = 0;
//    for (PropertyIterator it = range.first; it != range.second; ++it, ++i) {
//        if (i == index) {
//            return (*it).second;
//        }
//    }
}


MediaObjectReader::MediaObjectReader(AbstractMediaObject* pMediaObject) :
_pMediaObject(pMediaObject)
{
}


void
MediaObjectReader::readChildren(const std::string& metaData, std::vector<AbstractMediaObject*>* pChildren)
{
    Log::instance()->upnpav().debug("read children of media object ...");
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            AbstractMediaObject* pChildObject = _pMediaObject->createChildObject();
            readNode(pChildObject, pObjectNode);
            // append child at end of already fetched children
            if (pChildren) {
                Log::instance()->upnpav().debug("push child to back of vector");
                pChildren->push_back(pChildObject);
            }
            else {
                Log::instance()->upnpav().debug("append child to media object");
                _pMediaObject->appendChild(pChildObject);
            }
        }
        pObjectNode = pObjectNode->nextSibling();
    }
    Log::instance()->upnpav().debug("read children of media object finished.");
}


void
MediaObjectReader::read(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    readNode(_pMediaObject, pDidl);
}


void
MediaObjectReader::readNode(AbstractMediaObject* pObject, Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        try {
            // FIXME: object number is the full object path, take only the last segment here ...? Otherwise, Poco::NumberParser will crash.
//            pObject->setIndex(attr->getNamedItem(AvProperty::ID)->nodeValue());
            pObject->setId(attr->getNamedItem(AvProperty::ID)->nodeValue());
        }
        catch (...) {
            Log::instance()->upnpav().error("setting object number in media object reader failed");
        }
//        pObject->setObjectId(attr->getNamedItem(AvProperty::ID)->nodeValue());
    }
    if (pNode->nodeName() == AvClass::CONTAINER) {
        pObject->setIsContainer(true);
        if (attr != 0) {
            ui4 childCount = 0;
            try {
                childCount = Poco::NumberParser::parseUnsigned(attr->getNamedItem(AvProperty::CHILD_COUNT)->nodeValue());
            }
            catch (Poco::Exception &e) {
                Log::instance()->upnpav().error("parsing child count in media object reader failed");
            }
            pObject->setTotalChildCount(childCount);
        }
    }
    if (attr != 0) {
        attr->release();
    }

    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
            // TODO: special treatment of resources shouldn't be necessary
            if (childNode->nodeName() == AvProperty::RES) {
                Poco::XML::NamedNodeMap* attr = 0;
                std::string protInfo = "";
                std::streamsize size = 0;
                if (childNode->hasAttributes()) {
                    attr = childNode->attributes();
                    Poco::XML::Node* attrNode = attr->getNamedItem(AvProperty::PROTOCOL_INFO);
                    if (attrNode) {
                        protInfo = attrNode->nodeValue();
                    }
                    attrNode = attr->getNamedItem(AvProperty::SIZE);
                    if (attrNode) {
                        try {
                            size = Poco::NumberParser::parse(attrNode->nodeValue());
                        }
                        catch (Poco::Exception &e) {
                            Log::instance()->upnpav().error("parsing size in media object reader failed");
                        }
                    }
                }
                AbstractResource* pResource = pObject->createResource();
                pResource->setUri(childNode->innerText());
                pResource->setProtInfo(protInfo);
                pResource->setSize(size);
                pObject->addResource(pResource);
                if (attr != 0) {
                    attr->release();
                }
            }
            else {
                AbstractProperty* pProp = pObject->createProperty();
                pProp->setName(childNode->nodeName());
                pProp->setValue(childNode->innerText());
                pObject->addProperty(pProp);
            }
            childNode = childNode->nextSibling();
        }
    }
}


MediaObjectWriter2::MediaObjectWriter2(AbstractMediaObject* pMediaObject) :
_pMediaObject(pMediaObject),
_pDoc(0),
_pDidl(0)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::MediaObjectWriter2()");
}


void
MediaObjectWriter2::write(std::string& metaData)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::write()");
    writeMetaDataHeader();
    writeMetaData(_pDidl);
    writeMetaDataClose(metaData);
}


ui4
MediaObjectWriter2::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeChildren()");
    writeMetaDataHeader();

    ui4 c;
    ui4 childCount = _pMediaObject->getChildCount();
    for (c = 0; (c < requestedCount) && (c < childCount - startingIndex); ++c) {
        MediaObjectWriter2 writer(_pMediaObject->getChildForRow(startingIndex + c));
        writer.writeMetaData(_pDidl);
    }

    writeMetaDataClose(metaData);
    return c;
}


void
MediaObjectWriter2::writeMetaDataHeader()
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataHeader()");
    _pDoc = new Poco::XML::Document;

    _pDidl = _pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");

    _pDoc->appendChild(_pDidl);
}


void
MediaObjectWriter2::writeMetaDataClose(std::string& metaData)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataClose() ...");
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaDataClose() returns: \n" + metaData);
}


void
MediaObjectWriter2::writeMetaData(Poco::XML::Element* pDidl)
{
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData()");

    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (_pMediaObject->isContainer()) {
        pObject = pDoc->createElement(AvClass::CONTAINER);
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
        pChildCount->setValue(Poco::NumberFormatter::format(_pMediaObject->getChildCount()));
        pObject->setAttributeNode(pChildCount);
    }
    else {
        pObject = pDoc->createElement(AvClass::ITEM);
    }

    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() writing attributes ...");
    // write attributes:
    // id (String, required)
    std::string parentId = _pMediaObject->getParentId();
    std::string objectId = (parentId == "") ? "0" : (parentId + "/" + Poco::NumberFormatter::format(_pMediaObject->getIndex()));
    pObject->setAttribute(AvProperty::ID, objectId);
    // parentID (String, required)
    pObject->setAttribute(AvProperty::PARENT_ID, parentId);
    // restricted (Boolean, required)
    pObject->setAttribute(AvProperty::RESTRICTED, (_pMediaObject->isRestricted() ? "1" : "0"));

    // searchable (Boolean)
    // refID (String)

    // properties
    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() writing properties ...");
    // write properties
    int propCount = _pMediaObject->getPropertyCount();
    for (int propNum = 0; propNum < propCount; ++propNum) {
        AbstractProperty* pProp = _pMediaObject->getProperty(propNum);
        std::string name = pProp->getName();
        std::string value = pProp->getValue();
        // non UTF-8 characters cause the DOM writer to stop and leave an unfinished XML fragment.
        AvTypeConverter::replaceNonUtf8(value);
        Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() property: " + name + ", " + value);
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement(name);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode(value);
        pProperty->appendChild(pPropertyValue);

        // write property attributes
        int attrCount = pProp->getAttributeCount();
        for (int attrNum = 0; attrNum < attrCount; ++attrNum) {
            pProperty->setAttribute(pProp->getAttributeName(attrNum), pProp->getAttributeValue(attrNum));
        }

        pObject->appendChild(pProperty);
    }

    pDidl->appendChild(pObject);
    // check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)

    Log::instance()->upnpav().debug("MediaObjectWriter2::writeMetaData() finished.");
}


/*--------------- convenience classes ------------------*/

MemoryProperty::MemoryProperty() :
AbstractProperty(new MemoryPropertyImpl)
{
}


MemoryResource::MemoryResource() :
AbstractResource(new MemoryPropertyImpl)
{
}



/*--------------- depricated classes ------------------*/

Resource::Resource(const std::string& uri, const std::string& protInfo, ui4 size) :
_uri(uri),
_protInfo(protInfo),
_size(size)
{
}


const std::string&
Resource::getUri()
{
    return _uri;
}


const std::string&
Resource::getProtInfo()
{
    return _protInfo;
}


std::streamsize
Resource::getSize()
{
    return _size;
}


void
Resource::setUri(const std::string& uri)
{
    _uri = uri;
}


void
Resource::setProtInfo(const std::string& protInfo)
{
    _protInfo = protInfo;
}


MediaObjectOld::MediaObjectOld() :
_parent(0),
_isContainer(false),
_restricted(true)
{
}


bool
MediaObjectOld::isContainer()
{
    return _isContainer;
}


bool
MediaObjectOld::isRestricted()
{
    return _restricted;
}

void
MediaObjectOld::setIsContainer(bool isContainer)
{
    _isContainer = isContainer;
}


void
MediaObjectOld::appendChild(MediaObjectOld* pChild)
{
//     std::clog << "MediaObject::appendChild() with objectId: " << pChild->_objectId << std::endl;

    _children.push_back(pChild);
//     _childrenMap[pChild->_objectId] = pChild;
//     _childCount++;
//     pChild->_objectId = objectId;
//     pChild->_parentId = _objectId;
    pChild->_parent = this;

//     std::clog << "MediaObject::appendChild() finished" << std::endl;
}


void
MediaObjectOld::addResource(Resource* pResource)
{
    _resources.push_back(pResource);
}


std::string
MediaObjectOld::getObjectId() const
{
    if (_objectId == "0") {
        return _objectId;
    }
    else {
        if (_parent) {
            return _parent->getObjectId() + "/" + _objectId;
        }
        else {
            return "-1";
        }
    }
}


MediaObjectOld::PropertyIterator
MediaObjectOld::beginProperty()
{
    return _properties.begin();
}


MediaObjectOld::PropertyIterator
MediaObjectOld::endProperty()
{
    return _properties.end();
}


MediaObjectOld::ChildIterator
MediaObjectOld::beginChildren()
{
    return _children.begin();
}


MediaObjectOld::ChildIterator
MediaObjectOld::endChildren()
{
    return _children.end();
}


MediaObjectOld::ResourceIterator
MediaObjectOld::beginResource()
{
    return _resources.begin();
}

MediaObjectOld::ResourceIterator
MediaObjectOld::endResource()
{
    return _resources.end();
}


ui4
MediaObjectOld::getChildCount()
{
    return _children.size();
}


std::string
MediaObjectOld::getParentId()
{
    if (_parent) {
        return _parent->getObjectId();
    }
    else {
        return "-1";
    }
}


MediaObjectOld*
MediaObjectOld::getChild(ui4 num)
{
    return _children[num];
}


std::string
MediaObjectOld::objectId()
{
    return _objectId;
}


void
MediaObjectOld::setObjectId(const std::string& objectId)
{
    _objectId = objectId;
}


// void
// MediaObject::setParentId(const std::string& parentId)
// {
//     _parentId = parentId;
// }


void
MediaObjectOld::setTitle(const std::string& title)
{
//     std::clog << "MediaObject::setTitle() title: " << title << std::endl;
    Log::instance()->upnpav().debug("setting object title: " + title);

//     _properties.append("dc:title", new Omm::Variant(title));
    _properties[AvProperty::TITLE] = title;
//     std::clog << "MediaObject::setTitle() finished" << std::endl;
}


std::string
MediaObjectOld::getTitle()
{
//     std::clog << "MediaObject::getTitle()" << std::endl;

//     std::string res = _properties.getValue<std::string>("dc:title");
//     if (res == "") {
//         return "foo";
//     }
//     return res;
    return _properties[AvProperty::TITLE];
}


void
MediaObjectOld::setProperty(const std::string& name, const std::string& value)
{
//     std::clog << "MediaObject::setProperty() name : " << name << ", value: " << value << std::endl;
//     _properties.append(name, new Omm::Variant(value));
    _properties[name] = value;
}


// void
// MediaObject::addResource(const std::string& uri, const std::string& protInfo, ui4 size)
// {
//     Resource* pRes = new Resource;
//     pRes->_uri = uri;
//     pRes->_protInfo = protInfo;
//     pRes->_size = size;
//     _resources.push_back(pRes);
// }


// void
// MediaObject::addResource(Resource* pResource)
// {
//     pResource->_uri = _objectId + "$" + pResource->_resourceId;
//     // FIXME: transfer protocol (http-get) should be added in ItemServer
//     pResource->_protInfo = "http-get:*:" + pResource->_protInfo;
//
//     _resources.push_back(pResource);
//     _resourceMap[pResource->_resourceId] = pResource;
// }


// void
// MediaObject::addResource(const std::string& resourceId, const std::string& privateUri, const std::string& protInfo, ui4 size)
// {
//     Resource* pRes = new Resource;
//     pRes->_uri = _objectId + "$" + resourceId;
//     // FIXME: transfer protocol (http-get) should be added in ItemServer
//     pRes->_protInfo = "http-get:*:" + protInfo;
//     pRes->_size = size;
//     pRes->_resourceId = resourceId;
//     pRes->_privateUri = privateUri;
//     _resources.push_back(pRes);
//     _resourceMap[resourceId] = pRes;
// }


// Resource*
// MediaObject::getResource(int num)
// {
//     return _resources[num];
// }


// Resource*
// MediaObject::getResource(const std::string& resourceId)
// {
//     return _resourceMap[resourceId];
// }


MediaObjectWriter::MediaObjectWriter(MediaObjectOld* pMediaObject) :
_pMediaObject(pMediaObject),
_pDoc(0),
_pDidl(0)
// _pDoc(new Poco::XML::Document),
// _pDidl(_pDoc->createElement("DIDL-Lite"))
{
}


void
MediaObjectWriter::write(std::string& metaData)
{
//     std::clog << "MediaObjectWriter::write()" << std::endl;
    writeMetaDataHeader();
    writeMetaData(_pDidl);
    writeMetaDataClose(metaData);
//     std::clog << "MediaObjectWriter::write() finished" << std::endl;
}


ui4
MediaObjectWriter::writeChildren(ui4 startingIndex, ui4 requestedCount, std::string& metaData)
{
//     std::clog << "MediaObjectWriter::writeChildren()" << std::endl;
    writeMetaDataHeader();

    ui4 c;
    for (c = 0; (c < requestedCount) && (c < _pMediaObject->getChildCount() - startingIndex); ++c) {
//         std::clog << "MediaObject::writeChildren() child title: " << _children[startingIndex + c]->getTitle() << std::endl;
//         _pMediaObject->getChild(startingIndex + c)->writeMetaData(_pDidl);
        MediaObjectWriter writer(_pMediaObject->getChild(startingIndex + c));
        writer.writeMetaData(_pDidl);
    }

    writeMetaDataClose(metaData);
//     std::clog << "MediaObjectWriter::writeChildren() finished" << std::endl;
    return c;
}


void
MediaObjectWriter::writeMetaDataHeader()
{
//     std::clog << "MediaObjectWriter::writeMetaDataHeader()" << std::endl;
//     Poco::XML::Document* _pDoc = new Poco::XML::Document;
    _pDoc = new Poco::XML::Document;

    _pDidl = _pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");

    _pDoc->appendChild(_pDidl);

//     std::clog << "MediaObjectWriter::writeMetaDataHeader() finished" << std::endl;

//     _pDidl = _pDoc->createElementNS("urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/", "DIDL-Lite");
//     Poco::AutoPtr<Poco::XML::Attr> pUpnpNs = _pDoc->createAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp");
//     _pDidl->setAttributeNode(pUpnpNs);
//     Poco::AutoPtr<Poco::XML::Attr> pDcNs = _pDoc->createAttributeNS("http://purl.org/dc/elements/1.1/", "dc");
//     _pDidl->setAttributeNode(pDcNs);
//     _pDidl->setAttributeNS("urn:schemas-upnp-org:metadata-1-0/upnp/", "upnp", "");

}


void MediaObjectWriter::writeMetaDataClose(std::string& metaData)
{
//     std::clog << "MediaObjectWriter::writeMetaDataClose() pDoc: " << _pDoc << std::endl;
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
//     writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION);
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    metaData = ss.str();
//     std::clog << "meta data:" << std::endl << ss.str() << std::endl;
//     std::clog << "MediaObjectWriter::writeMetaDataClose() finished" << std::endl;
}


void
MediaObjectWriter::writeMetaData(Poco::XML::Element* pDidl)
{
//     std::clog << "MediaObjectWriter::writeMetaData() object title: " << _pMediaObject->getTitle() << std::endl;
    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pObject;
    if (_pMediaObject->isContainer()) {
//         std::clog << "MediaObjectWriter::writeMetaData() is container" << std::endl;
        pObject = pDoc->createElement(AvClass::CONTAINER);
        // childCount (Integer)
        Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute(AvProperty::CHILD_COUNT);
        pChildCount->setValue(Poco::NumberFormatter::format(_pMediaObject->getChildCount()));
        pObject->setAttributeNode(pChildCount);
    }
    else {
//         std::clog << "MediaObjectWriter::writeMetaData() is item" << std::endl;
        pObject = pDoc->createElement(AvClass::ITEM);
    }
    // write attributes:
    // id (String, required)
//     std::clog << "MediaObjectWriter::writeMetaData() attributes" << std::endl;
//     std::clog << "MediaObjectWriter::writeMetaData() id: " << _pMediaObject->getObjectId() << std::endl;
    // FIXME: when writing meta data in SetAVTransportURI(), 0/ is prepended
    pObject->setAttribute(AvProperty::ID, _pMediaObject->getObjectId());
    // parentID (String, required)
//     std::clog << "MediaObjectWriter::writeMetaData() parentID: " << _pMediaObject->getParentId() << std::endl;
    pObject->setAttribute(AvProperty::PARENT_ID, _pMediaObject->getParentId());
    // restricted (Boolean, required)
//     std::clog << "MediaObjectWriter::writeMetaData() restricted: " << (_pMediaObject->isRestricted() ? "1" : "0") << std::endl;
    pObject->setAttribute(AvProperty::RESTRICTED, (_pMediaObject->isRestricted() ? "1" : "0"));

    // searchable (Boolean)
    // refID (String)

    // resources
    for (MediaObjectOld::ResourceIterator it = _pMediaObject->beginResource(); it != _pMediaObject->endResource(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pResource = pDoc->createElement(AvProperty::RES);
        Poco::AutoPtr<Poco::XML::Text> pUri = pDoc->createTextNode((*it)->getUri());
        if ((*it)->getProtInfo() != "") {
            pResource->setAttribute(AvProperty::PROTOCOL_INFO, (*it)->getProtInfo());
        }
        if ((*it)->getSize() > 0) {
            pResource->setAttribute(AvProperty::SIZE, Poco::NumberFormatter::format((*it)->getSize()));
        }
        pResource->appendChild(pUri);
        pObject->appendChild(pResource);
    }

    // write properties
//     std::clog << "MediaObjectWriter::writeMetaData() property elements" << std::endl;
    for (MediaObjectOld::PropertyIterator it = _pMediaObject->beginProperty(); it != _pMediaObject->endProperty(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement((*it).first);
//         std::string propVal;
//         (*it).second->getValue(propVal);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode((*it).second);
        pProperty->appendChild(pPropertyValue);
        pObject->appendChild(pProperty);
    }

    pDidl->appendChild(pObject);
// check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)
//     std::clog << "MediaObjectWriter::writeMetaData() finished" << std::endl;
}


} // namespace Av
} // namespace Omm
