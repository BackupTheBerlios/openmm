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

#ifndef Filesystem_INCLUDED
#define Filesystem_INCLUDED

#include <Omm/UpnpAvServer.h>
#include <Omm/AvStream.h>


class FileModel : public Omm::Av::AbstractDataModel
{
public:
    FileModel();
    ~FileModel();

    virtual void setBasePath(const std::string& basePath);
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

    std::string                         _cachePath;
    std::string                         _containerClass;
    Omm::AvStream::Tagger*              _pTagger;
};


#endif
