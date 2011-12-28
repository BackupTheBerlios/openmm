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

    virtual bool preserveIndexCache() { return true; }
    virtual bool useObjectCache() { return true; }

    virtual void createIndex();
//    virtual std::string getContainerClass();

    virtual std::string getParentPath(const std::string& path);

    virtual Omm::Av::AbstractMediaObject* getMediaObject(const std::string& path);
    virtual std::streamsize getSize(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");

private:
    void scanDirectoryRecursively(Poco::File& directory);
    void loadTagger();
    void setClass(Omm::Av::ServerItem* pItem, Omm::AvStream::Meta::ContainerFormat format);

    Poco::Path                          _basePath;
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
    createIndex();

//    Omm::Av::Log::instance()->upnpav().debug("file data model ctor finished.");
}


FileDataModel::~FileDataModel()
{
    delete _pTagger;
}


void
FileDataModel::createIndex()
{
    Omm::Av::Log::instance()->upnpav().debug("file data model starting index scan ...");
//    if (preserveIndexCache()) {
//        readIndexCache();
//    }
    Poco::File baseDir(_basePath);
    scanDirectoryRecursively(baseDir);
    if (preserveIndexCache()) {
        writeIndexCache();
    }
    Omm::Av::Log::instance()->upnpav().debug("file data model index scan finished.");
}


//std::string
//FileDataModel::getContainerClass()
//{
//    return _containerClass;
//}


std::string
FileDataModel::getParentPath(const std::string& path)
{
    std::string::size_type pos = path.rfind("/");
    Omm::Av::Log::instance()->upnpav().debug("file data model path: " + path + ", parent path: " + path.substr(0, pos));
    if (pos == std::string::npos) {
        return "";
    }
    else {
        return path.substr(0, pos);
    }
}


Omm::Av::AbstractMediaObject*
FileDataModel::getMediaObject(const std::string& path)
{
    Poco::Path fullPath(_basePath, path);
    if (Poco::File(fullPath).isDirectory()) {
        Omm::Av::Log::instance()->upnpav().debug("file data model creating container for: " + fullPath.toString(Poco::Path::PATH_UNIX));
        Omm::Av::AbstractMediaObject* pContainer = getServerContainer()->createMediaContainer();
        pContainer->setTitle(fullPath.getFileName());
        return pContainer;
    }
    Omm::Av::Log::instance()->upnpav().debug("file data model tagging: " + fullPath.toString(Poco::Path::PATH_UNIX));
    Omm::AvStream::Meta* pMeta = _pTagger->tag(fullPath.toString(Poco::Path::PATH_UNIX));
    if (pMeta) {
        Omm::Av::ServerItem* pItem = getServerContainer()->createMediaItem();

        setClass(pItem, pMeta->getContainerFormat());

        std::string title = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
        Omm::Av::AvTypeConverter::replaceNonUtf8(title);
        if (title == "") {
            title = fullPath.getFileName();
        }
        pItem->setTitle(title);

        std::string propertyValue = pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST);
        Omm::Av::AvTypeConverter::replaceNonUtf8(propertyValue);
        pItem->setUniqueProperty(Omm::Av::AvProperty::ARTIST, propertyValue);

        propertyValue = pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM);
        Omm::Av::AvTypeConverter::replaceNonUtf8(propertyValue);
        pItem->setUniqueProperty(Omm::Av::AvProperty::ALBUM, propertyValue);

        propertyValue = pMeta->getTag(Omm::AvStream::Meta::TK_TRACK);
        Omm::Av::AvTypeConverter::replaceNonUtf8(propertyValue);
        pItem->setUniqueProperty(Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER, propertyValue);

        propertyValue = pMeta->getTag(Omm::AvStream::Meta::TK_GENRE);
        Omm::Av::AvTypeConverter::replaceNonUtf8(propertyValue);
        pItem->setUniqueProperty(Omm::Av::AvProperty::GENRE, propertyValue);

        Omm::Av::ServerItemResource* pResource = pItem->createResource();
        pResource->setSize(getSize(path));
        // FIXME: add some parts of protinfo in server container / media server.
        pResource->setProtInfo("http-get:*:" + pMeta->getMime() + ":*");
        pItem->addResource(pResource);
        // TODO: add icon property
//        pItem->_icon = pItem->_path;

        delete pMeta;
        return pItem;
    }
    return 0;
}


std::streamsize
FileDataModel::getSize(const std::string& path)
{
    std::streamsize res;
    Poco::Path fullPath(_basePath, path);
    try {
        res = Poco::File(fullPath).getSize();
    }
    catch (Poco::Exception& e) {
        Omm::Av::Log::instance()->upnpav().error("could not get size of file: " + fullPath.toString(Poco::Path::PATH_UNIX));
        res = 0;
    }
    return res;
}


bool
FileDataModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return true;
}


std::istream*
FileDataModel::getStream(const std::string& path, const std::string& resourcePath)
{
    Poco::Path fullPath(_basePath, path);
    std::istream* pRes = new std::ifstream(fullPath.toString(Poco::Path::PATH_UNIX).c_str());
    if (!*pRes) {
        Omm::Av::Log::instance()->upnpav().error("could not open file for streaming: " + _basePath.append(path).toString(Poco::Path::PATH_UNIX));
        return 0;
    }
    return pRes;
}


void
FileDataModel::setClass(Omm::Av::ServerItem* pItem, Omm::AvStream::Meta::ContainerFormat format)
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
FileDataModel::scanDirectoryRecursively(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
        try {
//            addPath(dir->path().substr(_basePath.toString(Poco::Path::PATH_UNIX).length()));
            if (dir->isDirectory()) {
                scanDirectoryRecursively(*dir);
            }
            else {
                addPath(dir->path().substr(_basePath.toString(Poco::Path::PATH_UNIX).length()));
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
        Omm::Av::Log::instance()->upnpav().warning("could not find avstream tagger plugin: " + taggerPlugin + " loading vlc tagger ...");
        try {
            _pTagger = taggerPluginLoader.load("tagger-vlc");
        }
        catch(Poco::NotFoundException) {
            Omm::Av::Log::instance()->upnpav().warning("could not find avstream tagger plugin: tagger-vlc loading simple tagger ...");
            _pTagger = taggerPluginLoader.load("tagger-simple");
        }
    }
}


// FilecachedServer::FilecachedServer() :
// CachedServer(9999)
// {
// }


void
FilecachedServer::setBasePath(const std::string& basePath)
{
    ServerContainer::setBasePath(basePath);
    setCacheFilePath(basePath + "/.omm/cache/objectCache");
    FileDataModel* pDataModel = new FileDataModel(basePath);
    setDataModel(pDataModel);
    setTitle(basePath);
    setClass(pDataModel->getContainerClass());
    scan();
}


std::string
FilecachedServer::getPluginType()
{
    return "FileServerContainer";
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::ServerContainer)
POCO_EXPORT_CLASS(FilecachedServer)
POCO_END_MANIFEST
#endif
