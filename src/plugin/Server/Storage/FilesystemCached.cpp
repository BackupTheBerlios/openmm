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
#include <Poco/NumberFormatter.h>

#include <Omm/UpnpAvObject.h>
#include <Omm/AvStream.h>

#include "FilesystemCached.h"


class FileDataModel : public Omm::Av::AbstractDataModel
{
public:
    FileDataModel(const std::string& basePath);
    ~FileDataModel();

    virtual void scan(bool on = true);
//    virtual std::string getContainerClass();
    virtual Omm::Av::AbstractMediaObject* getMediaObject(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index, const std::string& resourcePath = "");
    virtual std::istream* getStream(Omm::ui4 index, const std::string& resourcePath = "");

private:
    void scanDirectoryRecursively(Poco::File& directory);
    void loadTagger();
    void setClass(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta::ContainerFormat format);
    void setProperty(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta* pMeta, Omm::AvStream::Meta::TagKey);

    std::string                         _basePath;
    std::string                         _cachePath;
    std::string                         _containerClass;
    Omm::AvStream::Tagger*              _pTagger;
};


FileDataModel::FileDataModel(const std::string& basePath) :
_basePath(basePath),
_cachePath(basePath + "/.omm/cache"),
_pTagger(0)
{
//    Omm::Av::Log::instance()->upnpav().debug("file data model ctor ...");
    _cacheFile = _cachePath + "/indexCache";
    loadTagger();
//    Omm::Av::Log::instance()->upnpav().debug("file data model ctor finished.");
}


FileDataModel::~FileDataModel()
{
    delete _pTagger;
}


void
FileDataModel::scan(bool on)
{
    Omm::Av::Log::instance()->upnpav().debug("file data model starting scan ...");
    if (on) {
        Poco::File baseDir(_basePath);
        scanDirectoryRecursively(baseDir);
        flushIndexBuffer();
        writeIndexCache();
    }
    Omm::Av::Log::instance()->upnpav().debug("file data model scan finished.");
}


//std::string
//FileDataModel::getContainerClass()
//{
//    return _containerClass;
//}


Omm::Av::AbstractMediaObject*
FileDataModel::getMediaObject(Omm::ui4 index)
{
    std::string path = _basePath + getPath(index);
    Omm::Av::Log::instance()->upnpav().debug("file data model tagging:" + path);
    Omm::AvStream::Meta* pMeta = _pTagger->tag(path);
    if (pMeta) {
        Omm::Av::MemoryMediaObject* pItem = new Omm::Av::MemoryMediaObject;

        pItem->setIndex(index);
        setClass(pItem, pMeta->getContainerFormat());

        std::string title = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
        Omm::Av::AvTypeConverter::replaceNonUtf8(title);
        if (title == "") {
            title = Poco::Path(path).getFileName();
        }
        pItem->setTitle(title);

        setProperty(pItem, pMeta, Omm::AvStream::Meta::TK_ARTIST);
        setProperty(pItem, pMeta, Omm::AvStream::Meta::TK_ALBUM);
        setProperty(pItem, pMeta, Omm::AvStream::Meta::TK_TRACK);
        setProperty(pItem, pMeta, Omm::AvStream::Meta::TK_GENRE);

//        pItem->_path = dir->path().substr(_basePath.length());
//        pItem->_mime = pMeta->getMime();
//        pItem->_icon = pItem->_path;

        delete pMeta;
        return pItem;
    }
    return 0;
}


bool
FileDataModel::isSeekable(Omm::ui4 index, const std::string& resourcePath)
{
    return true;
}


std::istream*
FileDataModel::getStream(Omm::ui4 index, const std::string& resourcePath)
{
    std::string filePath = _basePath + getPath(index);
    std::istream* pRes = new std::ifstream(filePath.c_str());
    if (!*pRes) {
        Omm::Av::Log::instance()->upnpav().error("could not open file for streaming: " + filePath);
        return 0;
    }
    return pRes;
}


void
FileDataModel::setClass(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta::ContainerFormat format)
{
    switch (format) {
        case Omm::AvStream::Meta::CF_UNKNOWN:
            pItem->setClass(Omm::Av::AvClass::OBJECT);
            break;
        case Omm::AvStream::Meta::CF_AUDIO:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM));
            break;
        case Omm::AvStream::Meta::CF_VIDEO:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM));
            break;
        case Omm::AvStream::Meta::CF_IMAGE:
            pItem->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM));
            break;
    }
}


void
FileDataModel::setProperty(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta* pMeta, Omm::AvStream::Meta::TagKey key)
{
    Omm::Av::AbstractProperty* pProperty = pItem->createProperty();
    switch (key) {
        case Omm::AvStream::Meta::TK_ARTIST:
            pProperty->setName(Omm::Av::AvProperty::ARTIST);
            break;
        case Omm::AvStream::Meta::TK_ALBUM:
            pProperty->setName(Omm::Av::AvProperty::ALBUM);
            break;
        case Omm::AvStream::Meta::TK_TRACK:
            pProperty->setName(Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER);
            break;
        case Omm::AvStream::Meta::TK_GENRE:
            pProperty->setName(Omm::Av::AvProperty::GENRE);
            break;
    }
    std::string value = pMeta->getTag(key);
    Omm::Av::AvTypeConverter::replaceNonUtf8(value);
    pProperty->setValue(value);
    pItem->addProperty(pProperty);
}


void
FileDataModel::scanDirectoryRecursively(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
        try {
            if (dir->isFile()) {
                bufferIndex(getIndex(dir->path().substr(_basePath.length())));
            }
            else if (dir->isDirectory()) {
                scanDirectoryRecursively(*dir);
            }
        }
        catch(...) {
            Omm::Av::Log::instance()->upnpav().warning(dir->path() + " not found while scanning directory, ignoring.");
        }
        ++dir;
    }
}


void
FileDataModel::loadTagger()
{
    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        _pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        Omm::Av::Log::instance()->upnpav().warning("could not find avstream tagger plugin: " + taggerPlugin + " loading simple tagger ...");
        _pTagger = taggerPluginLoader.load("tagger-simple");
    }
}


// FilecachedServer::FilecachedServer() :
// CachedServer(9999)
// {
// }


void
FilecachedServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        Omm::Av::Log::instance()->upnpav().debug("file cached server set option: " + key + " to: " + value);
        FileDataModel* pDataModel = new FileDataModel(value);
        setDataModel(pDataModel);
        setTitle(value);
        setClass(pDataModel->getContainerClass());

        pDataModel->scan();
    }
}


//std::string
//FileDataModel::getClass(Omm::ui4 index)
//{
//    Omm::Av::Log::instance()->upnpav().debug("get class of item with index " + Poco::NumberFormatter::format(index) + ": ");
//
//    return "";
//}
//
//
//std::string
//FileDataModel::getTitle(Omm::ui4 index)
//{
//    Omm::Av::Log::instance()->upnpav().debug("get title: ");
////    return _items[index]->_title;
//}
//
//
//std::string
//FileDataModel::getOptionalProperty(Omm::ui4 index, const std::string& property)
//{
//    Omm::Av::Log::instance()->upnpav().debug("get property: " + property);
//
//    if (property == Omm::Av::AvProperty::ARTIST) {
////        return _items[index]->_artist;
//    }
//    else if (property == Omm::Av::AvProperty::ALBUM) {
////        return _items[index]->_album;
//    }
//    else if (property == Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER) {
////        return _items[index]->_track;
//    }
//    else if (property == Omm::Av::AvProperty::GENRE) {
////        return _items[index]->_genre;
//    }
//    else if (property == Omm::Av::AvProperty::ICON) {
////        return _items[index]->_icon;
//    }
//    else {
//        return "";
//    }
//}
//
//
//
//
//std::istream*
//FileDataModel::getStream(Omm::ui4 index)
//{
////    std::string filePath = _basePath + _items[index]->_path;
//    std::string filePath = _basePath;
//    std::istream* pRes = new std::ifstream(filePath.c_str());
//    if (!*pRes) {
//        Omm::AvStream::Log::instance()->avstream().error("could not open file for streaming: " + filePath);
//        return 0;
//    }
//    return pRes;
//}
//
//
//std::istream*
//FileDataModel::getIconStream(Omm::ui4 index)
//{
////    std::string iconPath = _basePath + "/.omm/cache/icons" + _items[index]->_path;
//    std::string iconPath = _basePath + "/.omm/cache/icons";
//    std::istream* pRes = new std::ifstream(iconPath.c_str());
//    if (!*pRes) {
//        Omm::AvStream::Log::instance()->avstream().error("could not open icon for streaming: " + iconPath);
//        return 0;
//    }
//    return pRes;
//}
//
//
//std::streamsize
//FileDataModel::getSize(Omm::ui4 index)
//{
//    Omm::ui4 res;
//    try {
////        res = Poco::File((_basePath + _items[index]->_path)).getSize();
//    }
//    catch (Poco::Exception& e) {
////        Omm::AvStream::Log::instance()->avstream().error("could not get size of file: " + _basePath + _items[index]->_path);
//        res = 0;
//    }
//    return res;
//}
//
//
//std::string
//FileDataModel::getMime(Omm::ui4 index)
//{
////    Omm::Av::Log::instance()->upnpav().debug("get mime: " + _items[index]->_mime);
////    return _items[index]->_mime;
//}
//
//
//std::string
//FileDataModel::getDlna(Omm::ui4 index)
//{
//    // TODO: Tagger should determine the dlna string
//    return "*";
//}


//void
//FileDataModel::scanDirectory(const std::string& basePath)
//{
    // read media items from cache (if already there), otherwise scan the disk for media items.
//    if (cacheExists()) {
//        readCache();
//    }
//    else {
//        loadTagger();
//        Poco::File baseDir(basePath);
//        scanDirectoryRecursively(baseDir);
        // sort items
//        for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
//            _itemMap[(*it)->sortKey()] = *it;
//        }
//        _items.clear();
//        for (std::map<std::string,FileItem*>::iterator it = _itemMap.begin(); it != _itemMap.end(); ++it) {
//            _items.push_back((*it).second);
//        }
//        _itemMap.clear();
//        writeCache();
//    }
//    Omm::AvStream::Log::instance()->avstream().debug("file data model read " + Poco::NumberFormatter::format(_items.size()) + " file items");
//}


//bool
//FileDataModel::cacheExists()
//{
//    if (Poco::File(_cachePath).exists()) {
//        return true;
//    }
//    else {
//        return false;
//    }
//}


//void
//FileDataModel::readCache()
//{
//    Poco::XML::SAXParser parser;
//    parser.setContentHandler(this);
//
//    try {
//        parser.parse(_cachePath);
//    }
//    catch (Poco::Exception& e) {
//        Omm::Av::Log::instance()->upnpav().warning("cache parser error: " + e.displayText());
//    }
//}


//void
//FileDataModel::writeCache()
//{
//    Omm::Av::Log::instance()->upnpav().debug("writing cache to disk ...");
//
//    Poco::XML::Document* pCacheDoc= new Poco::XML::Document;
//    Poco::AutoPtr<Poco::XML::Element> pFileItemList = pCacheDoc->createElement(FileItem::FILE_ITEM_LIST);
//    pFileItemList->setAttribute("class", Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER));
//    pCacheDoc->appendChild(pFileItemList);
//
//    for (std::vector<FileItem*>::iterator it = _items.begin(); it != _items.end(); ++it) {
//        (*it)->writeXml(pFileItemList);
//    }
//
//    Poco::XML::DOMWriter writer;
//    writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
//    std::ofstream ostr(_cachePath.c_str());
//    writer.writeNode(ostr, pCacheDoc);
//
//    Omm::Av::Log::instance()->upnpav().debug("cache written to disk.");
//}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FilecachedServer)
POCO_END_MANIFEST
#endif
