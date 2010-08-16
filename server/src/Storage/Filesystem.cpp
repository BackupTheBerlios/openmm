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
#include "Filesystem.h"


class FileDataModel : public Omm::Av::AbstractDataModel
{
public:
    FileDataModel(const std::string& basePath);
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getTitle(Omm::ui4 index);
    
    virtual Omm::ui4 getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::streamsize stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek);

private:
//     void setBasePath(const std::string& basePath);
    void scanDirectory(Poco::File& directory);
    
    std::vector<Poco::File>              _files;
};


FileDataModel::FileDataModel(const std::string& basePath)
{
    Poco::File baseDir(basePath);
    scanDirectory(baseDir);
}


Omm::ui4
FileDataModel::getChildCount()
{
    return _files.size();
}


std::string
FileDataModel::getTitle(Omm::ui4 index)
{
    return Poco::Path(_files[index].path()).getFileName();
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
        if (dir->isFile()) {
//             std::clog << "FileServer::setBasePath() adding file: " << dir.name() << std::endl;
            _files.push_back(*dir);
        }
        else if (dir->isDirectory()) {
//             std::clog << "FileServer::setBasePath() adding directory: " << dir.name() << std::endl;
            scanDirectory(*dir);
        }
        ++dir;
    }
}


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
