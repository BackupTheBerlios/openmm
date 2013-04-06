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

#ifndef MPMediaModel_INCLUDED
#define MPMediaModel_INCLUDED

#include <Omm/UpnpAvObject.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/AvStream.h>


//class MPMediaItem;

class MPMediaModel : public Omm::Av::AbstractDataModel
{
public:
    MPMediaModel();
    ~MPMediaModel();

    virtual std::string getModelClass();
    virtual Omm::ui4 getSystemUpdateId(bool checkMod);
    virtual Omm::Av::CsvList getQueryProperties();
    virtual void scan();
    virtual bool useObjectCache() { return true; }

    virtual Omm::Av::ServerObject* getMediaObject(const std::string& path);
    virtual std::streamsize getSize(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");
    virtual void freeStream(std::istream* pIstream);

private:
//    MPMediaItem* getMediaItem(const std::string& path);
    void setClass(Omm::Av::ServerItem* pItem, Omm::AvStream::Meta::ContainerFormat format);

    std::string                         _cachePath;
    std::string                         _containerClass;
    std::map<std::string, std::time_t>  _directories;

    void*                               _pAssetReader;
};


#endif
