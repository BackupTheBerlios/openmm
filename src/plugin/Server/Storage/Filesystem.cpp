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

#include "Filesystem.h"


FileModel::FileModel() :
_pTagger(0)
{
    loadTagger();
}


FileModel::~FileModel()
{
    delete _pTagger;
}


void
FileModel::scan(bool recurse)
{
    Poco::File baseDir(getBasePath());
    scanDirectory(baseDir, recurse);
}


std::string
FileModel::getParentPath(const std::string& path)
{
    std::string::size_type pos = path.rfind("/");
    if (pos == std::string::npos) {
        Omm::Av::Log::instance()->upnpav().debug("file data model path: " + path + ", parent path: \"\"");
        return "";
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("file data model path: " + path + ", parent path: " + path.substr(0, pos));
        return path.substr(0, pos);
    }
}


Omm::Av::AbstractMediaObject*
FileModel::getMediaObject(const std::string& path)
{
    Poco::Path fullPath(getBasePath(), path);
    if (Poco::File(fullPath).isDirectory()) {
        Omm::Av::Log::instance()->upnpav().debug("file data model creating container for: " + fullPath.toString());
        Omm::Av::ServerContainer* pContainer = getServerContainer()->createMediaContainer();
        pContainer->setDataModel(this);
        pContainer->setTitle(fullPath.getFileName());
        return pContainer;
    }
    Omm::Av::Log::instance()->upnpav().debug("file data model tagging: " + fullPath.toString());
    Omm::AvStream::Meta* pMeta = _pTagger->tag(fullPath.toString());
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
FileModel::getSize(const std::string& path)
{
    std::streamsize res;
    Poco::Path fullPath(getBasePath(), path);
    try {
        res = Poco::File(fullPath).getSize();
    }
    catch (Poco::Exception& e) {
        Omm::Av::Log::instance()->upnpav().error("could not get size of file: " + fullPath.toString());
        res = 0;
    }
    return res;
}


bool
FileModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return true;
}


std::istream*
FileModel::getStream(const std::string& path, const std::string& resourcePath)
{
    Poco::Path fullPath(getBasePath(), path);
    std::istream* pRes = new std::ifstream(fullPath.toString().c_str());
    if (!*pRes) {
        Omm::Av::Log::instance()->upnpav().error("could not open file for streaming: " + fullPath.toString());
        return 0;
    }
    return pRes;
}


void
FileModel::setClass(Omm::Av::ServerItem* pItem, Omm::AvStream::Meta::ContainerFormat format)
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
FileModel::scanDirectory(Poco::File& directory, bool recurse)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
        try {
//            addPath(dir->path().substr(getBasePath().length()));
            if (recurse && dir->isDirectory()) {
                scanDirectory(*dir);
            }
            else {
                addPath(dir->path().substr(getBasePath().length()));
            }
        }
        catch(...) {
            Omm::Av::Log::instance()->upnpav().warning(dir->path() + " not found while scanning directory, ignoring.");
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


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(FileModel)
POCO_END_MANIFEST
#endif
