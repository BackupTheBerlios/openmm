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
    virtual std::streamsize getSize(Omm::ui4 index);
    virtual std::istream* getStream(Omm::ui4 index, const std::string& resourcePath = "");

private:
    void scanDirectoryRecursively(Poco::File& directory);
    void loadTagger();
    void setClass(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta::ContainerFormat format);
    void setProperty(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta* pMeta, Omm::AvStream::Meta::TagKey);
    void addResource(Omm::Av::MemoryMediaObject* pItem, Omm::AvStream::Meta* pMeta, Omm::AvStream::Meta::TagKey);

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

        Omm::Av::MemoryResource* pResource = new Omm::Av::MemoryResource;
        pResource->setSize(getSize(index));
        pItem->addResource(pResource);

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


std::streamsize
FileDataModel::getSize(Omm::ui4 index)
{
    std::streamsize res;
    std::string filePath = _basePath + getPath(index);
    try {
        res = Poco::File(filePath).getSize();
    }
    catch (Poco::Exception& e) {
        Omm::Av::Log::instance()->upnpav().error("could not get size of file: " + filePath);
        res = 0;
    }
    return res;
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
        _basePath = value;
        FileDataModel* pDataModel = new FileDataModel(value);
        setDataModel(pDataModel);
        setTitle(value);
        setClass(pDataModel->getContainerClass());

        pDataModel->scan();
    }
}


std::string
FilecachedServer::getOption(const std::string& key)
{
    if (key == "basePath") {
        return _basePath;
    }
}



#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FilecachedServer)
POCO_END_MANIFEST
#endif
