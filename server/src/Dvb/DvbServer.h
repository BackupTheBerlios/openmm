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

#include <Poco/File.h>

#include <Omm/UpnpAvServer.h>
#include <Omm/Dvb.h>

class DvbItem;

class DvbServer : public Omm::Av::StreamingMediaObject
{
    friend class DvbItemResource;
    
public:
    DvbServer();
    virtual ~DvbServer();
    
    virtual Omm::ui4 getChildCount();
    virtual bool isContainer();
    virtual Omm::Av::AbstractMediaObject* getChild(Omm::ui4 numChild);
    virtual int getPropertyCount(const std::string& name = "");
    virtual Omm::Av::AbstractProperty* getProperty(int index);
    virtual Omm::Av::AbstractProperty* getProperty(const std::string& name, int index = 0);
    virtual void addProperty(Omm::Av::AbstractProperty* pProperty);
    virtual Omm::Av::AbstractProperty* createProperty();
    virtual Omm::Av::AbstractMediaObject* createChildObject();
    
    virtual void setOption(const std::string& key, const std::string& value);
    
private:
    void setBasePath(const std::string& basePath);
    void scanDirectory(Poco::File& directory);
    void scanChannelConfig(const std::string& channelConfig);
//     Poco::File& getFileReference(Omm::ui4 childNum);
    
    std::string                          _basePath;
    std::vector<std::string>             _fileNames;
    std::vector<Poco::File>              _files;
    
//    std::string                          _channelConfigFile;
    std::vector<std::string>             _channelNames;
    std::vector<Omm::Dvb::DvbChannel*>   _channels;
    
    Omm::Av::AbstractProperty*           _pTitleProp;
    DvbItem*                             _pChild;
};

#endif
