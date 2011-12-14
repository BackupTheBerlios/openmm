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

    virtual Omm::ui4 getChildCount();
    virtual std::string getContainerClass();
    virtual std::string getClass(Omm::ui4 index);
    virtual std::string getTitle(Omm::ui4 index);
    virtual std::string getOptionalProperty(Omm::ui4 index, const std::string& property);

    virtual std::streamsize getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::istream* getStream(Omm::ui4 index);
    virtual std::istream* getIconStream(Omm::ui4 index);

private:
//     void setBasePath(const std::string& basePath);
    void scanDirectory(const std::string& basePath);
    void scanDirectoryRecursively(Poco::File& directory);
    void loadTagger();
    bool cacheExists();
    void readCache();
    void writeCache();

    std::string                         _basePath;
    std::string                         _cachePath;
    std::string                         _containerClass;
    Omm::AvStream::Tagger*              _pTagger;
};


FileDataModel::FileDataModel(const std::string& basePath) :
_basePath(basePath),
_cachePath(basePath + "/.ommcache"),
_pTagger(0)
{
//    scanDirectory(basePath);
}


FileDataModel::~FileDataModel()
{
    delete _pTagger;
}


Omm::ui4
FileDataModel::getChildCount()
{
    return 0;
}


std::string
FileDataModel::getContainerClass()
{
    return _containerClass;
}


std::string
FileDataModel::getClass(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get class of item with index " + Poco::NumberFormatter::format(index) + ": ");

    return "";
}


std::string
FileDataModel::getTitle(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get title: ");
//    return _items[index]->_title;
}


std::string
FileDataModel::getOptionalProperty(Omm::ui4 index, const std::string& property)
{
    Omm::Av::Log::instance()->upnpav().debug("get property: " + property);

    if (property == Omm::Av::AvProperty::ARTIST) {
//        return _items[index]->_artist;
    }
    else if (property == Omm::Av::AvProperty::ALBUM) {
//        return _items[index]->_album;
    }
    else if (property == Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER) {
//        return _items[index]->_track;
    }
    else if (property == Omm::Av::AvProperty::GENRE) {
//        return _items[index]->_genre;
    }
    else if (property == Omm::Av::AvProperty::ICON) {
//        return _items[index]->_icon;
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
//    std::string filePath = _basePath + _items[index]->_path;
    std::string filePath = _basePath;
    std::istream* pRes = new std::ifstream(filePath.c_str());
    if (!*pRes) {
        Omm::AvStream::Log::instance()->avstream().error("could not open file for streaming: " + filePath);
        return 0;
    }
    return pRes;
}


std::istream*
FileDataModel::getIconStream(Omm::ui4 index)
{
//    std::string iconPath = _basePath + "/.omm/cache/icons" + _items[index]->_path;
    std::string iconPath = _basePath + "/.omm/cache/icons";
    std::istream* pRes = new std::ifstream(iconPath.c_str());
    if (!*pRes) {
        Omm::AvStream::Log::instance()->avstream().error("could not open icon for streaming: " + iconPath);
        return 0;
    }
    return pRes;
}


std::streamsize
FileDataModel::getSize(Omm::ui4 index)
{
    Omm::ui4 res;
    try {
//        res = Poco::File((_basePath + _items[index]->_path)).getSize();
    }
    catch (Poco::Exception& e) {
//        Omm::AvStream::Log::instance()->avstream().error("could not get size of file: " + _basePath + _items[index]->_path);
        res = 0;
    }
    return res;
}


std::string
FileDataModel::getMime(Omm::ui4 index)
{
//    Omm::Av::Log::instance()->upnpav().debug("get mime: " + _items[index]->_mime);
//    return _items[index]->_mime;
}


std::string
FileDataModel::getDlna(Omm::ui4 index)
{
    // TODO: Tagger should determine the dlna string
    return "*";
}


void
FileDataModel::scanDirectory(const std::string& basePath)
{
    // read media items from cache (if already there), otherwise scan the disk for media items.
    if (cacheExists()) {
        readCache();
    }
    else {
        loadTagger();
        Poco::File baseDir(basePath);
        scanDirectoryRecursively(baseDir);
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
//    Omm::AvStream::Log::instance()->avstream().debug("file data model read " + Poco::NumberFormatter::format(_items.size()) + " file items");
}


void
FileDataModel::scanDirectoryRecursively(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
        try {
            if (dir->isFile()) {
                Omm::AvStream::Meta* pMeta = _pTagger->tag(dir->path());
                if (pMeta) {
//                    FileItem* pItem = new FileItem;
//                    pItem->_path = dir->path().substr(_basePath.length());
//                    pItem->setClass(pMeta->getContainerFormat());
//                    pItem->_title = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
//                    if (pItem->_title == "") {
//                        pItem->_title = Poco::Path(dir->path()).getFileName();
//                    }
//                    pItem->_artist = pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST);
//                    pItem->_album = pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM );
//                    pItem->_track = pMeta->getTag(Omm::AvStream::Meta::TK_TRACK);
//                    pItem->_genre = pMeta->getTag(Omm::AvStream::Meta::TK_GENRE);
//                    pItem->_mime = pMeta->getMime();
//                    pItem->_icon = pItem->_path;
//
//                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_title);
//                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_artist);
//                    Omm::Av::AvTypeConverter::replaceNonUtf8(pItem->_album);

//                    _items.push_back(pItem);
                    delete pMeta;
                }
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
        Omm::AvStream::Log::instance()->avstream().warning("Error could not find avstream tagger plugin: " + taggerPlugin + " loading simple tagger ...");
        _pTagger = taggerPluginLoader.load("tagger-simple");
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
//    Poco::XML::SAXParser parser;
//    parser.setContentHandler(this);
//
//    try {
//        parser.parse(_cachePath);
//    }
//    catch (Poco::Exception& e) {
//        Omm::Av::Log::instance()->upnpav().warning("cache parser error: " + e.displayText());
//    }
}


void
FileDataModel::writeCache()
{
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
}


// FileServer::FileServer() :
// TorchServer(9999)
// {
// }


void
FilecachedServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        FileDataModel* pDataModel = new FileDataModel(value);
        setDataModel(pDataModel);
        setTitle(value);
        setClass(pDataModel->getContainerClass());
    }
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FilecachedServer)
POCO_END_MANIFEST
#endif
