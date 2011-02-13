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
#include <Poco/File.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/Attributes.h>

#include <Omm/AvStream.h>

#include "Filesystem.h"

// TODO: store UPnP AV media items in item cache, not own format of XML item description
// parse it with a DOM parser into memory and build an array of pointers to the item Nodes
// remove "private" nodes, such as path to file on filesystem.

class FileItem
{
public:
    std::string sortKey();
    void setClass(Omm::AvStream::Meta::ContainerFormat format);
    void writeXml(Poco::XML::Node* node);

    std::string         _path;
    std::string         _class;
    std::string         _title;
    std::string         _artist;
    std::string         _album;
    std::string         _track;
    std::string         _genre;
    std::string         _mime;
    
    const static std::string FILE_ITEM_LIST;
    const static std::string FILE_ITEM;
    const static std::string FILE_ITEM_PATH;
    const static std::string FILE_ITEM_CLASS;
    const static std::string FILE_ITEM_TITLE;
    const static std::string FILE_ITEM_ALBUM;
    const static std::string FILE_ITEM_ARTIST;
    const static std::string FILE_ITEM_TRACK;
    const static std::string FILE_ITEM_GENRE;
    const static std::string FILE_ITEM_MIME;
};


const std::string FileItem::FILE_ITEM_LIST = "itemList";
const std::string FileItem::FILE_ITEM = "item";
const std::string FileItem::FILE_ITEM_PATH = "path";
const std::string FileItem::FILE_ITEM_CLASS = "class";
const std::string FileItem::FILE_ITEM_TITLE = "title";
const std::string FileItem::FILE_ITEM_ALBUM = "album";
const std::string FileItem::FILE_ITEM_ARTIST = "artist";
const std::string FileItem::FILE_ITEM_TRACK = "track";
const std::string FileItem::FILE_ITEM_GENRE = "genre";
const std::string FileItem::FILE_ITEM_MIME = "mime";


std::string
FileItem::sortKey()
{
    return _artist + _album + _track + _title;
}


void
FileItem::setClass(Omm::AvStream::Meta::ContainerFormat format)
{
    switch (format) {
        case Omm::AvStream::Meta::CF_UNKNOWN:
            _class = Omm::Av::AvClass::OBJECT;
            break;
        case Omm::AvStream::Meta::CF_AUDIO:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM);
            break;
        case Omm::AvStream::Meta::CF_VIDEO:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM);
            break;
        case Omm::AvStream::Meta::CF_IMAGE:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM);
            break;
    }
}


void
FileItem::writeXml(Poco::XML::Node* pNode)
{
    Poco::XML::Document* pDoc = pNode->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pFileItem = pDoc->createElement(FILE_ITEM);
    pNode->appendChild(pFileItem);
    
    Poco::AutoPtr<Poco::XML::Element> pPath = pDoc->createElement(FILE_ITEM_PATH);
    Poco::AutoPtr<Poco::XML::Text> pPathValue = pDoc->createTextNode(_path);
    pPath->appendChild(pPathValue);
    pFileItem->appendChild(pPath);

    Poco::AutoPtr<Poco::XML::Element> pClass = pDoc->createElement(FILE_ITEM_CLASS);
    Poco::AutoPtr<Poco::XML::Text> pClassValue = pDoc->createTextNode(_class);
    pClass->appendChild(pClassValue);
    pFileItem->appendChild(pClass);
    
    Poco::AutoPtr<Poco::XML::Element> pTitle = pDoc->createElement(FILE_ITEM_TITLE);
    Poco::AutoPtr<Poco::XML::Text> pTitleValue = pDoc->createTextNode(_title);
    pTitle->appendChild(pTitleValue);
    pFileItem->appendChild(pTitle);
    
    Poco::AutoPtr<Poco::XML::Element> pArtist = pDoc->createElement(FILE_ITEM_ARTIST);
    Poco::AutoPtr<Poco::XML::Text> pArtistValue = pDoc->createTextNode(_artist);
    pArtist->appendChild(pArtistValue);
    pFileItem->appendChild(pArtist);
    
    Poco::AutoPtr<Poco::XML::Element> pAlbum = pDoc->createElement(FILE_ITEM_ALBUM);
    Poco::AutoPtr<Poco::XML::Text> pAlbumValue = pDoc->createTextNode(_album);
    pAlbum->appendChild(pAlbumValue);
    pFileItem->appendChild(pAlbum);
    
    Poco::AutoPtr<Poco::XML::Element> pTrack = pDoc->createElement(FILE_ITEM_TRACK);
    Poco::AutoPtr<Poco::XML::Text> pTrackValue = pDoc->createTextNode(_track);
    pTrack->appendChild(pTrackValue);
    pFileItem->appendChild(pTrack);
    
    Poco::AutoPtr<Poco::XML::Element> pGenre = pDoc->createElement(FILE_ITEM_GENRE);
    Poco::AutoPtr<Poco::XML::Text> pGenreValue = pDoc->createTextNode(_genre);
    pGenre->appendChild(pGenreValue);
    pFileItem->appendChild(pGenre);

    Poco::AutoPtr<Poco::XML::Element> pMime = pDoc->createElement(FILE_ITEM_MIME);
    Poco::AutoPtr<Poco::XML::Text> pMimeValue = pDoc->createTextNode(_mime);
    pMime->appendChild(pMimeValue);
    pFileItem->appendChild(pMime);
}


class FileDataModel : public Omm::Av::AbstractDataModel, public Poco::XML::ContentHandler
{
public:
    FileDataModel(const std::string& basePath);
    ~FileDataModel();
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getClass(Omm::ui4 index);
    virtual std::string getTitle(Omm::ui4 index);
    virtual std::string getOptionalProperty(Omm::ui4 index, const std::string& property);
    
    virtual std::streamsize getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::istream* getStream(Omm::ui4 index);
    
    // SAX parser interface
    virtual void setDocumentLocator(const Poco::XML::Locator* loc) {}
    virtual void startDocument() {}
    virtual void endDocument();
    virtual void startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes);
    virtual void endElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname) {}
    virtual void characters(const Poco::XML::XMLChar ch[], int start, int length);
    virtual void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length) {}
    virtual void processingInstruction(const Poco::XML::XMLString& target, const Poco::XML::XMLString& data) {}
    virtual void startPrefixMapping(const Poco::XML::XMLString& prefix, const Poco::XML::XMLString& uri) {}
    virtual void endPrefixMapping(const Poco::XML::XMLString& prefix) {}
    virtual void skippedEntity(const Poco::XML::XMLString& name) {}

private:
//     void setBasePath(const std::string& basePath);
    void scanDirectory(const std::string& basePath);
    void scanDirectoryRecursively(Poco::File& directory);
    bool cacheExists();
    void readCache();
    void writeCache();
    
    std::string                         _basePath;
    std::string                         _cachePath;
    Omm::AvStream::Tagger*              _pTagger;
    std::vector<FileItem*>              _items;
    std::map<std::string,FileItem*>     _itemMap;
    FileItem*                           _pParseItem;
    std::string                         _parseElement;
};


FileDataModel::FileDataModel(const std::string& basePath) :
_basePath(basePath),
_cachePath(basePath + "/.ommcache"),
_pTagger(0),
_pParseItem(0),
_parseElement("")
{
    
    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        _pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream tagger plugin: " + taggerPlugin);
    }
    
//     Poco::File baseDir(basePath);
    scanDirectory(basePath);
}


FileDataModel::~FileDataModel()
{
    for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
        delete *it;
    }
    delete _pTagger;
}


Omm::ui4
FileDataModel::getChildCount()
{
    return _items.size();
}


std::string
FileDataModel::getClass(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get class of item with index " + Poco::NumberFormatter::format(index) + ": " + _items[index]->_class);
    
    return _items[index]->_class;
}


std::string
FileDataModel::getTitle(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get title: " + _items[index]->_title);
    return _items[index]->_title;
}


std::string
FileDataModel::getOptionalProperty(Omm::ui4 index, const std::string& property)
{
    Omm::Av::Log::instance()->upnpav().debug("get property: " + property);
    
    if (property == Omm::Av::AvProperty::ARTIST) {
        return _items[index]->_artist;
    }
    else if (property == Omm::Av::AvProperty::ALBUM) {
        return _items[index]->_album;
    }
    else if (property == Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER) {
        return _items[index]->_track;
    }
    else if (property == Omm::Av::AvProperty::GENRE) {
        return _items[index]->_genre;
    }
    else {
        return "";
    }
}


bool
FileDataModel::isSeekable(Omm::ui4 index)
{
    return true;
}


std::istream*
FileDataModel::getStream(Omm::ui4 index)
{
    std::istream* pRes = new std::ifstream((_basePath + _items[index]->_path).c_str());
    if (!*pRes) {
        Omm::AvStream::Log::instance()->avstream().error("could not open file for streaming: " + _basePath + _items[index]->_path);
        return 0;
    }
    return pRes;
}


std::streamsize
FileDataModel::getSize(Omm::ui4 index)
{
    Omm::ui4 res;
    try {
        res = Poco::File((_basePath + _items[index]->_path)).getSize();
    }
    catch (Poco::Exception& e) {
        Omm::AvStream::Log::instance()->avstream().error("could not get size of file: " + _basePath + _items[index]->_path);
        res = 0;
    }
    return res;
}


std::string
FileDataModel::getMime(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get mime: " + _items[index]->_mime);
    return _items[index]->_mime;
}


std::string
FileDataModel::getDlna(Omm::ui4 index)
{
    // TODO: Tagger should determine the dlna string
    return "*";
}


void
FileDataModel::endDocument()
{
    Omm::AvStream::Log::instance()->avstream().debug("parsed items: " + Poco::NumberFormatter::format(_items.size()));
}


void
FileDataModel::startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes)
{
    if (localName == FileItem::FILE_ITEM) {
        _pParseItem = new FileItem;
        _items.push_back(_pParseItem);
    }
    else {
        _parseElement = localName;
    }
}


void
FileDataModel::characters(const Poco::XML::XMLChar ch[], int start, int length)
{
    std::string text = Poco::trim(std::string(ch + start, length));
    if (_parseElement == FileItem::FILE_ITEM_PATH) {
        _pParseItem->_path += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_CLASS) {
        _pParseItem->_class += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_TITLE) {
        _pParseItem->_title += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_ARTIST) {
        _pParseItem->_artist += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_ALBUM) {
        _pParseItem->_album += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_TRACK) {
        _pParseItem->_track += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_GENRE) {
        _pParseItem->_genre += text;
    }
    else if (_parseElement == FileItem::FILE_ITEM_MIME) {
        _pParseItem->_mime += text;
    }
}


void
FileDataModel::scanDirectory(const std::string& basePath)
{
    // read media items from cache (if already there), otherwise scan the disk for media items.
    if (cacheExists()) {
        readCache();
    }
    else {
        Poco::File baseDir(basePath);
        scanDirectoryRecursively(baseDir);
        // sort items
        for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
            _itemMap[(*it)->sortKey()] = *it;
        }
        _items.clear();
        for (std::map<std::string,FileItem*>::iterator it = _itemMap.begin(); it != _itemMap.end(); ++it) {
            _items.push_back((*it).second);
        }
        _itemMap.clear();
        writeCache();
    }
    Omm::AvStream::Log::instance()->avstream().debug("file data model read " + Poco::NumberFormatter::format(_items.size()) + " file items");
}


void
FileDataModel::scanDirectoryRecursively(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
//         if (dir->isFile() && dir->exists() && dir->canRead() && !dir->isLink()) {
        try {
            if (dir->isFile()) {
                Omm::AvStream::Meta* pMeta = _pTagger->tag(dir->path());
                if (pMeta) {
                    FileItem* pItem = new FileItem;
                    pItem->_path = dir->path().substr(_basePath.length());
                    pItem->setClass(pMeta->getContainerFormat());
                    pItem->_title = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
                    if (pItem->_title == "") {
                        pItem->_title = Poco::Path(dir->path()).getFileName();
                    }
                    pItem->_artist = pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST);
                    pItem->_album = pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM );
                    pItem->_track = pMeta->getTag(Omm::AvStream::Meta::TK_TRACK);
                    pItem->_genre = pMeta->getTag(Omm::AvStream::Meta::TK_GENRE);
                    pItem->_mime = pMeta->getMime();
                    
                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_title);
                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_artist);
                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_album);
                    
                    _items.push_back(pItem);
                    delete pMeta;
                }
            }
            else if (dir->isDirectory()) {
                scanDirectoryRecursively(*dir);
            }
        }
        catch(...) {
            Omm::Av::Log::instance()->upnpav().warning("some file not found, ignoring.");
        }
        ++dir;
    }
}


bool
FileDataModel::cacheExists()
{
    if (Poco::File(_cachePath).exists()) {
        return true;
    }
    else {
        return false;
    }
}


void
FileDataModel::readCache()
{
    Poco::XML::SAXParser parser;
    parser.setContentHandler(this);
    
    try {
        parser.parse(_cachePath);
    }
    catch (Poco::Exception& e) {
        Omm::Av::Log::instance()->upnpav().warning("cache parser error: " + e.displayText());
    }
}


void
FileDataModel::writeCache()
{
    Omm::Av::Log::instance()->upnpav().debug("writing cache to disk ...");
    
    Poco::XML::Document* pCacheDoc= new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pFileItemList = pCacheDoc->createElement(FileItem::FILE_ITEM_LIST);
    pCacheDoc->appendChild(pFileItemList);

    for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
        (*it)->writeXml(pFileItemList);
    }
    
    Poco::XML::DOMWriter writer;
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    std::ofstream ostr(_cachePath.c_str());
    writer.writeNode(ostr, pCacheDoc);
    
    Omm::Av::Log::instance()->upnpav().debug("cache written to disk.");
}


// FileServer::FileServer() :
// TorchServer(9999)
// {
// }


void
FileServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        setDataModel(new FileDataModel(value));
    }
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
#endif
