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


class FileDataModel : public Omm::Av::AbstractDataModel
{
public:
    FileDataModel(const std::string& basePath);
    ~FileDataModel();
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getClass(Omm::ui4 index);
    virtual std::string getTitle(Omm::ui4 index);
    
    virtual Omm::ui4 getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::streamsize stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek);

private:
//     void setBasePath(const std::string& basePath);
    void scanDirectory(Poco::File& directory);
        
    std::vector<Poco::File>             _files;
    Omm::AvStream::Tagger*              _pTagger;
};


FileDataModel::FileDataModel(const std::string& basePath)
{
    Poco::File baseDir(basePath);
    
    std::string taggerPlugin("tagger-ffmpeg");
    Omm::Util::PluginLoader<Omm::AvStream::Tagger> taggerPluginLoader;
    try {
        _pTagger = taggerPluginLoader.load(taggerPlugin, "Tagger", "FFmpeg");
    }
    catch(Poco::NotFoundException) {
        Omm::AvStream::Log::instance()->avstream().error("Error could not find avstream tagger plugin: " + taggerPlugin);
    }
    
    scanDirectory(baseDir);
}


FileDataModel::~FileDataModel()
{
    delete _pTagger;
}


Omm::ui4
FileDataModel::getChildCount()
{
    return _files.size();
}


std::string
FileDataModel::getClass(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("tagging: " + _files[index].path());
    
    std::string res;
    Omm::AvStream::Meta::ContainerFormat format;
    Omm::AvStream::Meta* pMeta = _pTagger->tag(_files[index].path());
    if (!pMeta) {
        format = Omm::AvStream::Meta::CF_UNKNOWN;
    }
    else {
        format = pMeta->getContainerFormat();
    }
    delete pMeta;
    switch (format) {
        case Omm::AvStream::Meta::CF_UNKNOWN:
            return Omm::Av::AvClass::OBJECT;
        case Omm::AvStream::Meta::CF_AUDIO:
            return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_ITEM);
        case Omm::AvStream::Meta::CF_VIDEO:
            return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::VIDEO_ITEM);
        case Omm::AvStream::Meta::CF_IMAGE:
            return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::IMAGE_ITEM);
    }
}


std::string
FileDataModel::getTitle(Omm::ui4 index)
{
    Omm::Av::Log::instance()->upnpav().debug("tagging: " + _files[index].path());
    
    std::string res;
    Omm::AvStream::Meta* pMeta = _pTagger->tag(_files[index].path());
    if (!pMeta) {
        res = "";
    }
    else {
        res = pMeta->getTag(Omm::AvStream::Meta::TK_TITLE);
    }
    if (res == "") {
        res = Poco::Path(_files[index].path()).getFileName();
    }
    delete pMeta;
    return res;
}


bool
FileDataModel::isSeekable(Omm::ui4 index)
{
    return true;
}


std::streamsize
FileDataModel::stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek)
{
    std::ifstream istr(_files[index].path().c_str());
    if (seek > 0) {
        istr.seekg(seek);
    }
    return Poco::StreamCopier::copyStream(istr, ostr);
}


Omm::ui4
FileDataModel::getSize(Omm::ui4 index)
{
    return _files[index].getSize();
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
FileDataModel::scanDirectory(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
//         if (dir->isFile() && dir->exists() && dir->canRead() && !dir->isLink()) {
        try {
            if (dir->isFile()) {
    //             std::clog << "FileServer::setBasePath() adding file: " << dir.name() << std::endl;
                _files.push_back(*dir);
            }
            else if (dir->isDirectory()) {
    //             std::clog << "FileServer::setBasePath() adding directory: " << dir.name() << std::endl;
                scanDirectory(*dir);
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
