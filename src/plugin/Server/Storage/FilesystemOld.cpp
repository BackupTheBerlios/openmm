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
#include <Poco/SAX/SAXParser.h>
#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/Attributes.h>
#include <Poco/NumberFormatter.h>

#include <Omm/UpnpAvObject.h>
#include <Omm/AvStream.h>

#include "Filesystem.h"


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
    std::string         _icon;

    const static std::string FILE_ITEM_LIST;
    const static std::string FILE_ITEM_CONTAINER_CLASS_PROPERTY;
    const static std::string FILE_ITEM;
    const static std::string FILE_ITEM_PATH;
    const static std::string FILE_ITEM_CLASS;
    const static std::string FILE_ITEM_TITLE;
    const static std::string FILE_ITEM_ALBUM;
    const static std::string FILE_ITEM_ARTIST;
    const static std::string FILE_ITEM_TRACK;
    const static std::string FILE_ITEM_GENRE;
    const static std::string FILE_ITEM_MIME;
    const static std::string FILE_ITEM_ICON;
};


const std::string FileItem::FILE_ITEM_LIST = "itemList";
const std::string FileItem::FILE_ITEM_CONTAINER_CLASS_PROPERTY = "class";
const std::string FileItem::FILE_ITEM = "item";
const std::string FileItem::FILE_ITEM_PATH = "path";
const std::string FileItem::FILE_ITEM_CLASS = "class";
const std::string FileItem::FILE_ITEM_TITLE = "title";
const std::string FileItem::FILE_ITEM_ALBUM = "album";
const std::string FileItem::FILE_ITEM_ARTIST = "artist";
const std::string FileItem::FILE_ITEM_TRACK = "track";
const std::string FileItem::FILE_ITEM_GENRE = "genre";
const std::string FileItem::FILE_ITEM_MIME = "mime";
const std::string FileItem::FILE_ITEM_ICON = "icon";


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

    Poco::AutoPtr<Poco::XML::Element> pIcon = pDoc->createElement(FILE_ITEM_ICON);
    Poco::AutoPtr<Poco::XML::Text> pIconValue = pDoc->createTextNode(_icon);
    pIcon->appendChild(pIconValue);
    pFileItem->appendChild(pIcon);
}


class FileModel : public Omm::Av::SimpleDataModel, public Poco::XML::ContentHandler
{
public:
    FileDataModel(const std::string& basePath);
    ~FileModel();

//    virtual Omm::ui4 getChildCount();
    virtual std::string getContainerClass();
    virtual std::string getClass(const std::string& path);
    virtual std::string getTitle(const std::string& path);
    virtual std::string getOptionalProperty(const std::string& path, const std::string& name);

    virtual std::streamsize getSize(const std::string& path);
    virtual std::string getMime(const std::string& path);
    virtual std::string getDlna(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getIconStream(const std::string& path);

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
    void scanDirectory(Poco::File& directory);
    void loadTagger();
    bool cacheExists();
    void readCache();
    void writeCache();

    std::string                         _basePath;
    std::string                         _cachePath;
    std::string                         _containerClass;
    Omm::AvStream::Tagger*              _pTagger;
    std::vector<FileItem*>              _itemList;
    std::map<std::string, FileItem*>    _items;
//    std::map<std::string, FileItem*>    _itemMap;
    FileItem*                           _pParseItem;
    std::string                         _parseElement;
};


FileModel::FileDataModel(const std::string& basePath) :
_basePath(basePath),
_cachePath(basePath + "/.ommcache"),
_pTagger(0),
_pParseItem(0),
_parseElement("")
{
    scanDirectory(basePath);
}


FileModel::~FileModel()
{
//    for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
//        delete *it;
//    }
    for (std::map<std::string, FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
        delete (*it).second;
    }
    delete _pTagger;
}


//Omm::ui4
//FileDataModel::getChildCount()
//{
//    return _items.size();
//}


std::string
FileModel::getContainerClass()
{
    return _containerClass;
}


std::string
FileModel::getClass(const std::string& path)
{
    LOGNS(Omm::Av, upnpav, debug, "get class of item with path " + path + ": ");
    LOGNS(Omm::Av, upnpav, debug, _items[path]->_class);

    return _items[path]->_class;
}


std::string
FileModel::getTitle(const std::string& path)
{
    LOGNS(Omm::Av, upnpav, debug, "get title: " + _items[path]->_title);
    return _items[path]->_title;
}


std::string
FileModel::getOptionalProperty(const std::string& path, const std::string& name)
{
    LOGNS(Omm::Av, upnpav, debug, "get property: " + name);

    if (name == Omm::Av::AvProperty::ARTIST) {
        return _items[path]->_artist;
    }
    else if (name == Omm::Av::AvProperty::ALBUM) {
        return _items[path]->_album;
    }
    else if (name == Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER) {
        return _items[path]->_track;
    }
    else if (name == Omm::Av::AvProperty::GENRE) {
        return _items[path]->_genre;
    }
    else if (name == Omm::Av::AvProperty::ICON) {
        return _items[path]->_icon;
    }
    else {
        return "";
    }
}


bool
FileModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return true;
}


std::istream*
FileModel::getStream(const std::string& path, const std::string& resourcePath)
{
    std::string filePath = _basePath + path;
    std::istream* pRes = new std::ifstream(filePath.c_str());
    if (!*pRes) {
        LOGNS(Omm::Av, upnpav, error, "could not open file for streaming: " + _basePath + path);
        return 0;
    }
    return pRes;
}


std::istream*
FileModel::getIconStream(const std::string& path)
{
    std::string iconPath = _basePath + "/.omm/cache/icons" + path;
    std::istream* pRes = new std::ifstream(iconPath.c_str());
    if (!*pRes) {
        LOGNS(Omm::Av, upnpav, error, "could not open icon for streaming: " + iconPath);
        return 0;
    }
    return pRes;
}


std::streamsize
FileModel::getSize(const std::string& path)
{
    Omm::ui4 res;
    try {
        res = Poco::File((_basePath + path)).getSize();
    }
    catch (Poco::Exception& e) {
        LOGNS(Omm::Av, upnpav, error, "could not get size of file: " + _basePath + path);
        res = 0;
    }
    return res;
}


std::string
FileModel::getMime(const std::string& path)
{
    LOGNS(Omm::Av, upnpav, debug, "get mime: " + _items[path]->_mime);
    return _items[path]->_mime;
}


std::string
FileModel::getDlna(const std::string& path)
{
    // TODO: Tagger should determine the dlna string
    return "*";
}


void
FileModel::endDocument()
{
    LOGNS(Omm::Av, upnpav, debug, "parsed items: " + Poco::NumberFormatter::format(_items.size()));
}


void
FileModel::startElement(const Poco::XML::XMLString& uri, const Poco::XML::XMLString& localName, const Poco::XML::XMLString& qname, const Poco::XML::Attributes& attributes)
{
    if (localName == FileItem::FILE_ITEM) {
        _pParseItem = new FileItem;
        _itemList.push_back(_pParseItem);
    }
    else if (localName == FileItem::FILE_ITEM_LIST) {
        //_containerClass = attributes.getValue(FileItem::FILE_ITEM_CONTAINER_CLASS_PROPERTY);
        _containerClass = attributes.getValue(0);
        LOGNS(Omm::Av, upnpav, debug, "local name: " + localName + " number of attributes: " + Poco::NumberFormatter::format(attributes.getLength()) + " parsing attribute container class: " + _containerClass);
    }
    else {
        _parseElement = localName;
    }
}


void
FileModel::characters(const Poco::XML::XMLChar ch[], int start, int length)
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
    else if (_parseElement == FileItem::FILE_ITEM_ICON) {
        _pParseItem->_icon += text;
    }
}


void
FileModel::scanDirectory(const std::string& basePath)
{
    // read media items from cache (if already there), otherwise scan the disk for media items.
    if (cacheExists()) {
        readCache();
    }
    else {
        loadTagger();
        Poco::File baseDir(basePath);
        scanDirectory(baseDir);

        // sort items
//        for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
//            _itemMap[(*it)->sortKey()] = *it;
//        }
//        _items.clear();
//        for (std::map<std::string,FileItem*>::iterator it = _itemMap.begin(); it != _itemMap.end(); ++it) {
//            _items.push_back((*it).second);
//        }
//        _itemMap.clear();
        writeCache();
    }
    LOGNS(Omm::Av, upnpav, debug, "file data model read " + Poco::NumberFormatter::format(_items.size()) + " file items");
}


void
FileModel::scanDirectory(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
//         if (dir->isFile() && dir->exists() && dir->canRead() && !dir->isLink()) {
        try {
            if (dir->isFile() && !dir->isHidden()) {
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
                    pItem->_icon = pItem->_path;

                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_title);
                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_artist);
                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_album);

//                    _items.push_back(pItem);
                    _items[pItem->_path] = pItem;
                    addPath(pItem->_path);

                    delete pMeta;
                }
            }
            else if (dir->isDirectory()) {
                scanDirectory(*dir);
            }
        }
        catch(...) {
            LOGNS(Omm::Av, upnpav, warning, dir->path() + " not found while scanning directory, ignoring.");
        }
        ++dir;
    }
}


void
FileModel::loadTagger()
{
    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        _pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        LOGNS(Omm::Av, upnpav, warning, "could not find avstream tagger plugin: " + taggerPlugin + " loading simple tagger ...");
        try {
            _pTagger = taggerPluginLoader.load("tagger-vlc");
        }
        catch(Poco::NotFoundException) {
            LOGNS(Omm::Av, upnpav, warning, "could not find avstream tagger plugin: tagger-vlc loading simple tagger ...");
            _pTagger = taggerPluginLoader.load("tagger-simple");
        }
    }
}


bool
FileModel::cacheExists()
{
    if (Poco::File(_cachePath).exists()) {
        return true;
    }
    else {
        return false;
    }
}


void
FileModel::readCache()
{
    Poco::XML::SAXParser parser;
    parser.setContentHandler(this);

    try {
        parser.parse(_cachePath);
    }
    catch (Poco::Exception& e) {
        LOGNS(Omm::Av, upnpav, warning, "cache parser error: " + e.displayText());
    }
    for (std::vector<FileItem*>::iterator it = _itemList.begin(); it != _itemList.end(); ++it) {
        addPath((*it)->_path);
        _items[(*it)->_path] = *it;
    }
    _itemList.clear();
}


void
FileModel::writeCache()
{
    LOGNS(Omm::Av, upnpav, debug, "writing cache to disk ...");

    Poco::XML::Document* pCacheDoc= new Poco::XML::Document;
    Poco::AutoPtr<Poco::XML::Element> pFileItemList = pCacheDoc->createElement(FileItem::FILE_ITEM_LIST);
    pFileItemList->setAttribute("class", Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER));
    pCacheDoc->appendChild(pFileItemList);

//    for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
//        (*it)->writeXml(pFileItemList);
//    }
    for (std::map<std::string, FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
        (*it).second->writeXml(pFileItemList);
    }

    Poco::XML::DOMWriter writer;
    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
    std::ofstream ostr(_cachePath.c_str());
    writer.writeNode(ostr, pCacheDoc);

    LOGNS(Omm::Av, upnpav, debug, "cache written to disk.");
}


// FileServer::FileServer() :
// TorchServer(9999)
// {
// }


//void
//FileServer::setBasePath(const std::string& basePath)
//{
//    ServerContainer::setBasePath(basePath);
//    FileDataModel* pDataModel = new FileDataModel(basePath);
//    setDataModel(pDataModel);
//    setTitle(basePath);
//    setClass(pDataModel->getContainerClass());
//}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
#endif
