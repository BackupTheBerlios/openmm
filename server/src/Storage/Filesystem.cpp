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

#include <Omm/AvStream.h>

#include "Filesystem.h"


class FileItem
{
public:
    void setClass(Omm::AvStream::Meta::ContainerFormat format);

    std::string         _path;
    std::string         _class;
    std::string         _title;
    std::string         _artist;
    std::string         _album;
    std::string         _track;
    std::string         _genre;
};


void
FileItem::setClass(Omm::AvStream::Meta::ContainerFormat format)
{
    switch (format) {
        case Omm::AvStream::Meta::CF_UNKNOWN:
            _class = Omm::Av::AvClass::OBJECT;
        case Omm::AvStream::Meta::CF_AUDIO:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM);
        case Omm::AvStream::Meta::CF_VIDEO:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM);
        case Omm::AvStream::Meta::CF_IMAGE:
            _class = Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM);
    }
}


class FileDataModel : public Omm::Av::AbstractDataModel
{
public:
    FileDataModel(const std::string& basePath);
    ~FileDataModel();
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getClass(Omm::ui4 index);
    virtual std::string getTitle(Omm::ui4 index);
    virtual std::string getOptionalProperty(Omm::ui4 index, const std::string& property);
    
    virtual Omm::ui4 getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::streamsize stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek);

private:
//     void setBasePath(const std::string& basePath);
    void scanDirectory(const std::string& basePath);
    void scanDirectoryRecursively(Poco::File& directory);
        
//     std::vector<Poco::File>             _files;
    std::vector<FileItem*>              _items;
    Omm::AvStream::Tagger*              _pTagger;
};


FileDataModel::FileDataModel(const std::string& basePath)
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
//     return _files.size();
}


std::string
FileDataModel::getClass(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get class of item with index " + Poco::NumberFormatter::format(index) + ": " + _items[index]->_class);
    
    return _items[index]->_class;

//     Omm::Av::Log::instance()->upnpav().debug("tagging: " + _files[index].path());
//     
//     std::string res;
//     Omm::AvStream::Meta::ContainerFormat format;
//     Omm::AvStream::Meta* pMeta = _pTagger->tag(_files[index].path());
//     if (!pMeta) {
//         format = Omm::AvStream::Meta::CF_UNKNOWN;
//     }
//     else {
//         format = pMeta->getContainerFormat();
//     }
//     delete pMeta;
//     switch (format) {
//         case Omm::AvStream::Meta::CF_UNKNOWN:
//             return Omm::Av::AvClass::OBJECT;
//         case Omm::AvStream::Meta::CF_AUDIO:
//             return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM);
//         case Omm::AvStream::Meta::CF_VIDEO:
//             return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM);
//         case Omm::AvStream::Meta::CF_IMAGE:
//             return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM);
//     }
}


std::string
FileDataModel::getTitle(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("get title: " + _items[index]->_title);
    return _items[index]->_title;

//     Omm::Av::Log::instance()->upnpav().debug("tagging: " + _files[index].path());
//     
//     std::string res;
//     Omm::AvStream::Meta* pMeta = _pTagger->tag(_files[index].path());
//     if (!pMeta) {
//         res = "";
//     }
//     else {
//         res = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
//     }
//     if (res == "") {
//         res = Poco::Path(_files[index].path()).getFileName();
//     }
//     delete pMeta;
//     return res;
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
    
    
//     Omm::Av::Log::instance()->upnpav().debug("tagging: " + _files[index].path());
//     
//     std::string res;
//     Omm::AvStream::Meta* pMeta = _pTagger->tag(_files[index].path());
//     if (!pMeta) {
//         res = "";
//     }
//     else {
//         if (property == Omm::Av::AvProperty::ARTIST) {
//             res = pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST);
//         }
//         else if (property == Omm::Av::AvProperty::ALBUM) {
//             res = pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM);
//         }
//         else if (property == Omm::Av::AvProperty::ORIGINAL_TRACK_NUMBER) {
//             res = pMeta->getTag(Omm::AvStream::Meta::TK_TRACK);
//         }
//         else if (property == Omm::Av::AvProperty::GENRE) {
//             res = pMeta->getTag(Omm::AvStream::Meta::TK_GENRE);
//         }
//     }
//     delete pMeta;
//     return res;
}


bool
FileDataModel::isSeekable(Omm::ui4 index)
{
    return true;
}


std::streamsize
FileDataModel::stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek)
{
//     std::ifstream istr(_files[index].path().c_str());
    std::ifstream istr(_items[index]->_path.c_str());
    if (seek > 0) {
        istr.seekg(seek);
    }
    return Poco::StreamCopier::copyStream(istr, ostr);
}


Omm::ui4
FileDataModel::getSize(Omm::ui4 index)
{
//     return _files[index].getSize();
    return Poco::File(_items[index]->_path).getSize();
}


std::string
FileDataModel::getMime(Omm::ui4 index)
{
    // TODO: Tagger should determine the mime type
    return "*";
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
    Poco::File cache(basePath + "/.ommcache");
    if (cache.exists()) {
        // read the cache
    }
    else {
        // scan directory recursively
        Poco::File baseDir(basePath);
        scanDirectoryRecursively(baseDir);
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
    //             std::clog << "FileServer::setBasePath() adding file: " << dir.name() << std::endl;
//                 _files.push_back(*dir);
                Omm::AvStream::Meta* pMeta = _pTagger->tag((*dir).path());
                if (pMeta) {
                    FileItem* pItem = new FileItem;
                    pItem->_path = (*dir).path();
                    pItem->setClass(pMeta->getContainerFormat());
                    pItem->_title = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
                    if (pItem->_title == "") {
                        pItem->_title == Poco::Path(pItem->_path).getFileName();
//                         pItem->_title == pItem->_path;
                    }
                    pItem->_artist = pMeta->getTag(Omm::AvStream::Meta::TK_ARTIST);
                    pItem->_album = pMeta->getTag(Omm::AvStream::Meta::TK_ALBUM );
                    pItem->_track = pMeta->getTag(Omm::AvStream::Meta::TK_TRACK);
                    pItem->_genre = pMeta->getTag(Omm::AvStream::Meta::TK_GENRE);
                    
                    _items.push_back(pItem);
                    delete pMeta;
                }
            }
            else if (dir->isDirectory()) {
    //             std::clog << "FileServer::setBasePath() adding directory: " << dir.name() << std::endl;
                scanDirectoryRecursively(*dir);
            }
        }
        catch(...) {
            Omm::Av::Log::instance()->upnpav().warning("some file not found, ignoring.");
        }
        ++dir;
    }
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


POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
