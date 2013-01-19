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
#include <Poco/TextConverter.h>
#include <Poco/UTF8Encoding.h>
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
#include <list>

#include "UpnpAvObject.h"
#include "Upnp.h"


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
const std::string Mime::PLAYLIST = Mime::TYPE_AUDIO + "/m3u";

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
 //   LOG(upnpav, debug, "matching class: " + name + " with class: " + matchName);

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
    int secondsInt = floor(seconds);
    int secondsFract = (seconds - secondsInt) * 100;
    return
        Poco::NumberFormatter::format(hours) + ":" +
        Poco::NumberFormatter::format0(minutes, 2) + ":" +
        Poco::NumberFormatter::format0(secondsInt, 2) + "." +
        Poco::NumberFormatter::format(secondsFract);
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
        LOG(upnpav, debug, "parsing time failed: " + e.message());
        return 0;
    }
    return res.timestamp();
}


std::string
AvTypeConverter::writeTime(const time& timeVal)
{
    return Poco::DateTimeFormatter::format(timeVal, Poco::DateTimeFormat::ISO8601_FORMAT, 0).substr(11);
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
    LOG(upnpav, debug, "Mime type: " + getType());
    return getType() == TYPE_AUDIO;
}


bool
Mime::isVideo()
{
    LOG(upnpav, debug, "Mime type: " + getType());
    return getType() == TYPE_VIDEO;
}


bool
Mime::isImage()
{
    LOG(upnpav, debug, "Mime type: " + getType());
    return getType() == TYPE_IMAGE;
}


const std::string SearchCriteria::logOpAnd = "and";
const std::string SearchCriteria::logOpOr = "or";
const std::string SearchCriteria::relOpEqual = "=";
const std::string SearchCriteria::relOpUnequal = "!=";
const std::string SearchCriteria::relOpLess = "<";
const std::string SearchCriteria::relOpLessEqual = "<=";
const std::string SearchCriteria::relOpGreater = ">";
const std::string SearchCriteria::relOpGreaterEqual = ">=";
const std::string SearchCriteria::stringOpContains = "contains";
const std::string SearchCriteria::stringOpContainsNot = "doesNotContain";
const std::string SearchCriteria::stringOpDerived = "derivedfrom";
const std::string SearchCriteria::existsOp = "exists";
const std::string SearchCriteria::boolValTrue = "true";
const std::string SearchCriteria::boolValFalse = "false";
const char SearchCriteria::hTab = '\x09';
const char SearchCriteria::lineFeed = '\x0A';
const char SearchCriteria::vTab = '\x0B';
const char SearchCriteria::formFeed = '\x0C';
const char SearchCriteria::returnChar = '\x0D';
const char SearchCriteria::space = ' ';
const char SearchCriteria::dQuote = '\"';
const char SearchCriteria::asterisk = '*';
const char SearchCriteria::openingBracket = '(';
const char SearchCriteria::closingBracket = ')';
const char SearchCriteria::escape = '\\';


SearchCriteria::SearchCriteria()
{
}


std::string
SearchCriteria::parse(const std::string& searchString)
{
    _searchString = searchString;
    _scanPos = 0;
    _translatedString = "";

    if (searchString == std::string(1, asterisk)) {
        _translatedString += translateAsterisk();
    }
    else {
        searchExp();
    }
    return _translatedString;
}


std::string
SearchCriteria::translateAsterisk()
{
    return std::string(1, asterisk);
}


std::string
SearchCriteria::translateCompareExp(const std::string& property, const std::string& op, const std::string& val)
{
    return property + space + op + space + val;
}


std::string
SearchCriteria::translateStringExp(const std::string& property, const std::string& op, const std::string& val)
{
    return property + space + op + space + val;
}


std::string
SearchCriteria::translateExistsExp(const std::string& property, const std::string& op, bool val)
{
    return property + space + op + space + (val ? "true" : "false");
}


std::string
SearchCriteria::translateLogOp(const std::string& logOp)
{
    return space + logOp + space;
}


std::string
SearchCriteria::translateProperty(const std::string& property)
{
    return property;
}


void
SearchCriteria::searchExp()
{
//    LOG(upnpav, debug, "search criteria, searchExp: " + Poco::NumberFormatter::format(_scanPos));

    skipBlanks();
    if (isOpeningBracket(peek())) {
        _translatedString += openingBracket;
        step();
        searchExp();
        if (isClosingBracket(peek())) {
            _translatedString += closingBracket;
            step();
        }
        else {
            throw Poco::Exception("search criteria missing closing bracket");
        }
    }
    else {
        relExp();
    }

    skipBlanks();
    if (!endOfString() && !isClosingBracket(peek())) {
        std::string tok = getToken();
        if (isLogOp(tok)) {
            _translatedString += translateLogOp(tok);
            searchExp();
        }
        else {
            throw Poco::Exception("search criteria garbage at end of expression");
        }
    }
    skipBlanks();

//    LOG(upnpav, debug, "search criteria, searchExp finished: " + Poco::NumberFormatter::format(_scanPos));
}


void
SearchCriteria::relExp()
{
//    LOG(upnpav, debug, "search criteria, relExp: " + Poco::NumberFormatter::format(_scanPos));

    std::string prop = getToken();
    std::string op = getToken();
    std::string val = getToken();
    if (isRelOp(op)) {
        if (isQuotedVal(val)) {
            _translatedString += translateCompareExp(translateProperty(prop), op, val);
        }
        else {
            throw Poco::Exception("search criteria quoted value expected");
        }
    }
    else if (isStringOp(op)) {
        if (isQuotedVal(val)) {
            _translatedString += translateStringExp(translateProperty(prop), op, val);
        }
        else {
            throw Poco::Exception("search criteria quoted value expected");
        }
    }
    else if (isExistsOp(op)) {
        bool boolVal;
        if (val == boolValTrue) {
            boolVal = true;
        }
        else if (val == boolValFalse) {
            boolVal = false;
        }
        else {
            throw Poco::Exception("search criteria boolean value expected");
        }
        _translatedString += translateExistsExp(translateProperty(prop), op, boolVal);
    }

//    LOG(upnpav, debug, "search criteria, relExp finished: " + Poco::NumberFormatter::format(_scanPos));
}


std::string
SearchCriteria::getToken()
{
    skipBlanks();
    std::string::size_type stringBegin = _scanPos;
    if (isBracket(peek())) {
        step();
    }
    else {
        while (!endOfString() && !isWChar(peek()) && !isBracket(peek())) {
            step();
        }
    }
//    LOG(upnpav, debug, "search criteria, getToken returns: " + _searchString.substr(stringBegin, _scanPos - stringBegin));
    return _searchString.substr(stringBegin, _scanPos - stringBegin);
}


char
SearchCriteria::peek()
{
    try {
        return _searchString.at(_scanPos);
    }
    catch (...) {
        throw Poco::Exception("search criteria scanner reached end of string");
    }
}


void
SearchCriteria::step()
{
    _scanPos++;
}


void
SearchCriteria::skipBlanks()
{
//    LOG(upnpav, debug, "search criteria, skipBlanks: " + Poco::NumberFormatter::format(_scanPos));

    while (!endOfString() && isWChar(peek())) {
        step();
    }
}


bool
SearchCriteria::endOfString()
{
    return _scanPos >= _searchString.length();
}


bool
SearchCriteria::isWChar(char ch)
{
    if (ch == space ||
        ch == hTab ||
        ch == lineFeed ||
        ch == vTab ||
        ch == formFeed ||
        ch == returnChar) {
        return true;
    }
    else {
        return false;
    }
}


bool
SearchCriteria::isOpeningBracket(char ch)
{
    return ch == openingBracket;
}


bool
SearchCriteria::isClosingBracket(char ch)
{
    return ch == closingBracket;
}


bool
SearchCriteria::isBracket(char ch)
{
    return isOpeningBracket(ch) || isClosingBracket(ch);
}


bool
SearchCriteria::isLogOp(const std::string& token)
{
    return (token == logOpAnd || token == logOpOr);
}


bool
SearchCriteria::isBinOp(const std::string& token)
{
    return (isRelOp(token) || isStringOp(token));

}


bool
SearchCriteria::isRelOp(const std::string& token)
{
    return (token == relOpEqual ||
            token == relOpUnequal ||
            token == relOpLess  ||
            token == relOpLessEqual ||
            token == relOpGreater ||
            token == relOpGreaterEqual
            );
}


bool
SearchCriteria::isStringOp(const std::string& token)
{
    return (token == stringOpContains ||
            token == stringOpContainsNot ||
            token == stringOpDerived);
}


bool
SearchCriteria::isExistsOp(const std::string& token)
{
    return token == existsOp;
}


bool
SearchCriteria::isQuotedVal(const std::string& token)
{
    return (token.length() >= 2 &&
            token.at(0) == dQuote &&
            token.at(token.length() - 1) == dQuote);
}


bool
SearchCriteria::isBoolVal(const std::string& token)
{
    return (token == boolValTrue || token == boolValFalse);
}


std::string
SqlSearchCriteria::translateAsterisk()
{
    return "";
}


std::string
SqlSearchCriteria::translateStringExp(const std::string& property, const std::string& op, const std::string& val)
{
    std::string translatedOp = op;
    std::string unquotedVal = val.substr(1, val.length() - 2);
    std::string translatedVal;
    if (op == stringOpContains) {
        translatedOp = "like";
        translatedVal = "\"%" + unquotedVal + "%\"";
    }
    else if (op == stringOpContainsNot) {
        translatedOp = "not like";
        translatedVal = "\"%" + unquotedVal + "%\"";
    }
    else if (op == stringOpDerived) {
        translatedOp = "like";
        translatedVal = "\"" + unquotedVal + "%\"";
    }

    return property + space + translatedOp + space + translatedVal;
}


std::string
SqlSearchCriteria::translateExistsExp(const std::string& property, const std::string& op, bool val)
{
    return property + space + op + space + (val ? "true" : "false");
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
    LOG(upnpav, debug, "abstract resource set size: " + Poco::NumberFormatter::format(size));

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
//    LOG(upnpav, debug, "AbstractProperty::getAttributeValue() name: " +  name + ", value: " + _pPropertyImpl->getAttributeValue(name));
    return _pPropertyImpl->getAttributeValue(name);
}


int
AbstractProperty::getAttributeCount()
{
    return _pPropertyImpl->getAttributeCount();
}


void
AbstractMediaObject::setUniqueProperty(const std::string& name, const std::string& value)
{
//    LOG(upnpav, debug, "AbstractMediaObject::setUniqueProperty() name: " +  name + ", value: " + value);
    // TODO: check if property title is already set
    AbstractProperty* pProp = createProperty();
    pProp->setName(name);
    pProp->setValue(value);
    addProperty(pProp);
}


void
AbstractMediaObject::setTitle(const std::string& title)
{
//    LOG(upnpav, debug, "AbstractMediaObject::setTitle() title: " + title);
    setUniqueProperty(AvProperty::TITLE, title);
}


void
AbstractMediaObject::setClass(const std::string& subclass)
{
//    LOG(upnpav, debug, "AbstractMediaObject::setClass() subclass: " + subclass);
    setUniqueProperty(AvProperty::CLASS, subclass);
}


std::string
AbstractMediaObject::getTitle()
{
//    LOG(upnpav, debug, "AbstractMediaObject::getTitle()");
    return getProperty(AvProperty::TITLE)->getValue();
}


std::string
AbstractMediaObject::getClass()
{
//    LOG(upnpav, debug, "AbstractMediaObject::getClass()");
    return getProperty(AvProperty::CLASS)->getValue();
}


AbstractResource*
AbstractMediaObject::getResource(int index)
{
//    LOG(upnpav, debug, "AbstractMediaObject::getResource() index: " + Poco::NumberFormatter::format(index));

    return static_cast<AbstractResource*>(getProperty(AvProperty::RES, index));
}


void
AbstractMediaObject::addResource(AbstractResource* pResource)
{
//    LOG(upnpav, debug, "AbstractMediaObject::addResource()");

    addProperty(pResource);
}


int
AbstractMediaObject::getResourceCount()
{
//    LOG(upnpav, debug, "AbstractMediaObject::getResourceCount()");

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
//    LOG(upnpav, debug, "memory property set value to: " + value);
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
_searchable(false),
_childCount(0)
{
}


MemoryMediaObject::MemoryMediaObject(const MemoryMediaObject& mediaObject) :
_restricted(mediaObject._restricted),
_isContainer(mediaObject._isContainer),
_searchable(mediaObject._searchable),
_childVec(mediaObject._childVec),
_propertyVec(mediaObject._propertyVec),
_propertyMap(mediaObject._propertyMap),
_childCount(mediaObject._childCount)
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
//    LOG(upnpav, debug, "MemoryMediaObject::createProperty()");
    return new AbstractProperty(new MemoryPropertyImpl);
}


AbstractResource*
MemoryMediaObject::createResource()
{
//    LOG(upnpav, debug, "MemoryMediaObject::createResource()");
    AbstractResource* pResource = new MemoryResource;
    return pResource;
}


void
MemoryMediaObject::setIsContainer(bool isContainer)
{
//    LOG(upnpav, debug, "MemoryMediaObject::setIsContainer()");
    _isContainer = isContainer;
}


void
MemoryMediaObject::setChildCount(ui4 childCount)
{
    _childCount = childCount;
}


ui4
MemoryMediaObject::getFetchedChildCount()
{
//    LOG(upnpav, debug, "MemoryMediaObject::getChildCount()");
    return _childVec.size();
}


ui4
MemoryMediaObject::getChildCount()
{
    return _childCount;
}


bool
MemoryMediaObject::isContainer()
{
//    LOG(upnpav, debug, "MemoryMediaObject::isContainer()");
    return _isContainer;
}


AbstractMediaObject*
MemoryMediaObject::getChildForRow(ui4 row)
{
//    LOG(upnpav, debug, "MemoryMediaObject::getChild() number: " + Poco::NumberFormatter::format(numChild));
    ui4 childRow = row;
    if (childRow < _childVec.size() && childRow >= 0) {
        return _childVec[childRow];
    }
    else {
        LOG(upnpav, error, "MemoryMediaObject::getChildForRow() row out of range: " + Poco::NumberFormatter::format(row));
    }
}


bool
MemoryMediaObject::isRestricted()
{
//    LOG(upnpav, debug, "MemoryMediaObject::isRestricted()");
    return _restricted;
}


bool
MemoryMediaObject::isSearchable()
{
    return _searchable;
}


void
MemoryMediaObject::setIsRestricted(bool restricted)
{
    _restricted = restricted;
}


void
MemoryMediaObject::setIsSearchable(bool searchable)
{
    _searchable = searchable;
}


void
MemoryMediaObject::addProperty(AbstractProperty* pProperty)
{
//    LOG(upnpav, debug, "MemoryMediaObject::addProperty()");
    _propertyVec.push_back(pProperty);
    _propertyMap.insert(make_pair(pProperty->getName(), pProperty));
}


AbstractProperty*
MemoryMediaObject::getProperty(int index)
{
//    LOG(upnpav, debug, "MemoryMediaObject::getProperty() index: " + Poco::NumberFormatter::format(index));
    return _propertyVec[index];
}


int
MemoryMediaObject::getPropertyCount(const std::string& name)
{
//    LOG(upnpav, debug, "MemoryMediaObject::getPropertyCount() name: " + name);
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
//    LOG(upnpav, debug, "MemoryMediaObject::getProperty() name: " + name + ", index: " + Poco::NumberFormatter::format(index));

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


MediaObjectReader::MediaObjectReader()
{
}


void
MediaObjectReader::readChildren(std::vector<AbstractMediaObject*>& children, const std::string& metaData, AbstractMediaObject* pContainer)
{
    LOG(upnpav, debug, "read children of media object ...");
    Poco::XML::DOMParser parser;
    Poco::AutoPtr<Poco::XML::Document> pDoc;
    try {
        pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, warning, "read children of media object failed, could not parse xml representation of media object: " + e.displayText());
        return;
    }
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            AbstractMediaObject* pChildObject = pContainer->createChildObject();
            readNode(pChildObject, pObjectNode);
            children.push_back(pChildObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
    LOG(upnpav, debug, "read children of media object finished.");
}


void
MediaObjectReader::read(AbstractMediaObject* pObject, const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData.substr(0, metaData.rfind('>') + 1));
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    readNode(pObject, pDidl);
}


void
MediaObjectReader::readNode(AbstractMediaObject* pObject, Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        Poco::XML::Node* pAttrNode = attr->getNamedItem(AvProperty::ID);
        if (pAttrNode) {
            pObject->setId(pAttrNode->nodeValue());
        }
        else {
            LOG(upnpav, warning, "setting mandatory object id in media object reader failed, object incomplete");
        }
        pAttrNode = attr->getNamedItem(AvProperty::RESTRICTED);
        if (pAttrNode) {
            Variant restricted(pAttrNode->nodeValue());
            bool isRestricted;
            restricted.getValue(isRestricted);
            pObject->setIsRestricted(isRestricted);
        }
        else {
            LOG(upnpav, error, "setting mandatory object restricted attribute in media object reader failed");
        }
        pAttrNode = attr->getNamedItem(AvProperty::CONTAINER_SEARCHABLE);
        if (pAttrNode) {
            Variant searchable(pAttrNode->nodeValue());
            bool isSearchable;
            searchable.getValue(isSearchable);
            pObject->setIsSearchable(isSearchable);
        }
        else {
            LOG(upnpav, warning, "media object reader searchable attribute not present");
        }
    }
    if (pNode->nodeName() == AvClass::CONTAINER) {
        pObject->setIsContainer(true);
        if (attr != 0) {
            ui4 childCount = 0;
            try {
                Poco::XML::Node* pChildAttr = attr->getNamedItem(AvProperty::CHILD_COUNT);
                if (pChildAttr) {
                    childCount = Poco::NumberParser::parseUnsigned(pChildAttr->nodeValue());
                }
            }
            catch (Poco::Exception& e) {
                LOG(upnpav, warning, "parsing child count in media object reader failed");
            }
            pObject->setChildCount(childCount);
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
                std::string importUri = "";
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
                            LOG(upnpav, error, "parsing size in media object reader failed");
                        }
                    }
                    attrNode = attr->getNamedItem(AvProperty::IMPORT_URI);
                    if (attrNode) {
                        importUri = attrNode->nodeValue();
                    }
                }
                AbstractResource* pResource = pObject->createResource();
                pResource->setUri(childNode->innerText());
                pResource->setProtInfo(protInfo);
                pResource->setSize(size);
                pResource->setAttribute(AvProperty::IMPORT_URI, importUri);
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


const std::string MediaObjectWriter::_xmlProlog = "<DIDL-Lite xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:omm=\"http://open.multimedia.org/xmlns/\" xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\">";

MediaObjectWriter::MediaObjectWriter(bool full) :
_pDoc(0),
_pDidl(0),
_full(full)
{
    LOG(upnpav, debug, "MediaObjectWriter2::MediaObjectWriter2()");
}


void
MediaObjectWriter::write(std::string& meta, AbstractMediaObject* pObject, const std::string& filter)
{
    LOG(upnpav, debug, "MediaObjectWriter2::write()");
    writeMetaDataHeader();
    writeMetaData(_pDidl, pObject);
    writeMetaDataClose(meta);
}


void
MediaObjectWriter::writeChildren(std::string& meta, const std::vector<AbstractMediaObject*>& children, const std::string& filter)
{
    LOG(upnpav, debug, "MediaObjectWriter2::writeChildren()");
    writeMetaDataHeader();
    for (std::vector<AbstractMediaObject*>::const_iterator it = children.begin(); it != children.end(); ++it) {
        MediaObjectWriter writer;
        writer.writeMetaData(_pDidl, *it);
    }
    writeMetaDataClose(meta);
}


const std::string&
MediaObjectWriter::getXmlProlog()
{
    return _xmlProlog;
}


void
MediaObjectWriter::writeMetaDataHeader()
{
    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaDataHeader()");
    _pDoc = new Poco::XML::Document;

    _pDidl = _pDoc->createElement("DIDL-Lite");

    // FIXME: is this the right way to set the namespaces?
    _pDidl->setAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
    _pDidl->setAttribute("xmlns:upnp", "urn:schemas-upnp-org:metadata-1-0/upnp/");
    _pDidl->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
    _pDidl->setAttribute("xmlns:omm", "http://open.multimedia.org/xmlns/");

    _pDoc->appendChild(_pDidl);
}


void
MediaObjectWriter::writeMetaDataClose(std::string& metaData)
{
    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaDataClose() ...");
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    try {
        writer.writeNode(ss, _pDoc);
    }
    catch (Poco::Exception& e) {
        LOG(upnpav, error, "MediaObjectWriter2::writeMetaDataClose() could not write meta data xml: " + e.displayText());
        return;
    }
    metaData = ss.str();
    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaDataClose() returns: \n" + metaData);
}


void
MediaObjectWriter::writeMetaData(Poco::XML::Element* pDidl, AbstractMediaObject* pObject)
{
    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaData()");

    Poco::XML::Document* pDoc = pDidl->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pXmlObject;
    if (pObject->isContainer()) {
        pXmlObject = pDoc->createElement(AvClass::CONTAINER);
        if (_full) {
            Poco::AutoPtr<Poco::XML::Attr> pChildCount = pDoc->createAttribute("childCount");
            pChildCount->setValue(Poco::NumberFormatter::format(pObject->getChildCount()));
            pXmlObject->setAttributeNode(pChildCount);
        }
    }
    else {
        pXmlObject = pDoc->createElement(AvClass::ITEM);
    }

    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaData() writing attributes ...");
    // write attributes:
    // id (String, required)
    if (_full) {
        std::string parentId = pObject->getParentId();
        std::string objectId = pObject->getId();
        pXmlObject->setAttribute(AvProperty::ID, objectId);
        // parentID (String, required)
        pXmlObject->setAttribute(AvProperty::PARENT_ID, parentId);
    }
    // restricted (Boolean, required)
    pXmlObject->setAttribute(AvProperty::RESTRICTED, (pObject->isRestricted() ? "1" : "0"));
    // searchable (Boolean)
    if (pObject->isSearchable()) {
        pXmlObject->setAttribute(AvProperty::CONTAINER_SEARCHABLE, "1");
    }

    // refID (String)

    // properties
    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaData() writing properties ...");
    // write properties
    int propCount = pObject->getPropertyCount();
    for (int propNum = 0; propNum < propCount; ++propNum) {
        AbstractProperty* pProp = pObject->getProperty(propNum);
        std::string name = pProp->getName();
        std::string value = pProp->getValue();
        LOG(upnpav, debug, "MediaObjectWriter2::writeMetaData() property: " + name + ", " + value);
        Poco::AutoPtr<Poco::XML::Element> pProperty = pDoc->createElement(name);
        Poco::AutoPtr<Poco::XML::Text> pPropertyValue = pDoc->createTextNode(value);
        pProperty->appendChild(pPropertyValue);

        // write property attributes
        int attrCount = pProp->getAttributeCount();
        for (int attrNum = 0; attrNum < attrCount; ++attrNum) {
            pProperty->setAttribute(pProp->getAttributeName(attrNum), pProp->getAttributeValue(attrNum));
        }

        if (value != "" || attrCount > 0) {
            pXmlObject->appendChild(pProperty);
        }
    }

    pDidl->appendChild(pXmlObject);
    // check somewhere, if the two required elements are there
    // title (String, dc)
    // class (String, upnp)

    LOG(upnpav, debug, "MediaObjectWriter2::writeMetaData() finished.");
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


/*--------------- media object caches ------------------*/


AbstractMediaObjectCache::AbstractMediaObjectCache(ui4 maxCacheSize) :
_maxCacheSize(maxCacheSize)
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


BlockCache::BlockCache(ui4 blockSize) :
_offset(0),
_blockSize(blockSize)
{
}


AbstractMediaObject*
BlockCache::getMediaObjectForRow(ui4 row)
{
    LOG(upnpav, debug, "block cache get media object in row: " + Poco::NumberFormatter::format(row) + " ...");

    if (row < 0) {
        LOG(upnpav, error, "block cache get media object failed, negative row.");
        return 0;
    }
    else if (_offset <= row && row < _offset + getCacheSize()) {
        // cache hit, do nothing
        LOG(upnpav, debug, "block cache get media object cache hit");
    }
    else if (_offset - row < _blockSize && _offset - row > 0) {
        // near cache miss, insert block before current cache
        int blockSize = std::min(_blockSize, _offset);
        LOG(upnpav, debug, "block cache get media object cache near miss before");
        if (getCacheSize() + blockSize > getMaxCacheSize()) {
            LOG(upnpav, debug, "block cache get media object deleting block at back");
            erase(_cache.end() - blockSize, _cache.end());
        }
        std::vector<AbstractMediaObject*> block;
        getBlock(block, _offset - blockSize, blockSize);
        insertBlock(block, true);
        _offset -= blockSize;
    }
    else if (row - (_offset + getCacheSize()) < _blockSize && row - (_offset + getCacheSize()) >= 0) {
        // near cache miss, insert block after current cache
        LOG(upnpav, debug, "block cache get media object cache near miss after");
        if (getCacheSize() + _blockSize > getMaxCacheSize()) {
            LOG(upnpav, debug, "block cache get media object deleting block at front");
            erase(_cache.begin(), _cache.begin() + _blockSize);
            _offset += _blockSize;
        }
        std::vector<AbstractMediaObject*> block;
        getBlock(block, _offset + getCacheSize(), _blockSize);
        insertBlock(block, false);
    }
    else {
        // far cache miss, rebuild whole cache
        LOG(upnpav, debug, "block cache get media object cache far miss");
        clear();
        std::vector<AbstractMediaObject*> block;
        getBlock(block, row, _blockSize);
        insertBlock(block);
        _offset = row;
//      scan(on);
    }
    LOG(upnpav, debug, "block cache get media object finished.");
    if (_cache.size() > row - _offset) {
        return _cache[row - _offset];
    }
    else {
        LOG(upnpav, error, "block cache get media object failed, cache not large enough.");
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
    LOG(upnpav, debug, "block cache clear ...");
    for (std::vector<AbstractMediaObject*>::iterator it = _cache.begin(); it != _cache.end(); ++it) {
        delete *it;
    }
    _cache.clear();
    LOG(upnpav, debug, "block cache clear finished.");
}


void
BlockCache::setBlockSize(ui4 blockSize)
{
    _blockSize = blockSize;
}


void
BlockCache::insertBlock(std::vector<AbstractMediaObject*>& block, bool prepend)
{
    LOG(upnpav, debug, std::string("block cache inserting block at ") + (prepend ? "front" : "back") + "...");
    if (prepend) {
        _cache.insert(_cache.begin(), block.begin(), block.begin() + block.size());
    }
    else {
        _cache.insert(_cache.end(), block.begin(), block.begin() + block.size());
    }
    LOG(upnpav, debug, "block cache inserting block finished.");
}


ui4
BlockCache::getCacheSize()
{
    return _cache.size();
}


} // namespace Av
} // namespace Omm
