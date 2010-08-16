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
#include <Poco/DirectoryIterator.h>

#include "DvbServer.h"


class DvbItemResource : public Omm::Av::StreamingResource
{
    friend class DvbServer;
    friend class DvbItem;
    
public:
    DvbItemResource(DvbServer* pServer, Omm::Av::AbstractMediaObject* pItem);
    
    virtual Omm::ui4 getSize();
    virtual std::string getMime();
    virtual std::string getDlna();
    
    virtual bool isSeekable();
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
    void setPath(const std::string& path);
    
private:
    std::string         _path;
    std::string                 _channelName;
    Omm::Dvb::DvbChannel*       _pChannel;

    
};


class DvbItemProperty : public Omm::Av::AbstractProperty
{
public:
    DvbItemProperty();
};


class DvbItemPropertyImpl : public Omm::Av::PropertyImpl
{
public:
    virtual void setName(const std::string& name);
    virtual void setValue(const std::string& value);
    virtual std::string getName();
    virtual std::string getValue();
    
private:
    std::string         _name;
    std::string         _value;
};


class DvbItem : public Omm::Av::StreamingMediaItem
{
    friend class DvbServer;
    
public:
    DvbItem(DvbServer* pServer);
    virtual ~DvbItem();
    
    virtual int getPropertyCount(const std::string& name = "");
    virtual Omm::Av::AbstractProperty* getProperty(int index);
    virtual Omm::Av::AbstractProperty* getProperty(const std::string& name, int index = 0);

    void setPath(const std::string& path);
    void setChannel(const std::string& channelName, Omm::Dvb::DvbChannel* pChannel);

private:
    DvbItemProperty*                   _pTitleProp;
    DvbItemResource*                   _pResource;
};


DvbItemResource::DvbItemResource(DvbServer* pServer, Omm::Av::AbstractMediaObject* pItem) :
StreamingResource(new Omm::Av::MemoryPropertyImpl, pServer, pItem)
{
}


bool
DvbItemResource::isSeekable()
{
    return false;
}


std::streamsize
DvbItemResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
//     std::clog << "DvbResource::stream(), relative path: " << _path << ", _pServer: " << _pServer << std::endl;
//     std::string path = static_cast<DvbServer*>(_pServer)->_basePath + "/" + _path;
//     std::clog << "DvbResource::stream(), absolute path: " << path << std::endl;
//     
//     std::ifstream istr(path.c_str());
//     if (seek > 0) {
//         istr.seekg(seek);
//     }
//     return Poco::StreamCopier::copyStream(istr, ostr);
    
    
    Omm::Dvb::DvbDevice::instance()->tune(_pChannel);
    
    Omm::Dvb::Log::instance()->dvb().debug("reading from dvr device ...");
    std::ifstream istr("/dev/dvb/adapter0/dvr0");
//     std::istream& istr = DvbDevice::instance()->getTransportStream(_pChannel);
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
    return bytes;
}


void
DvbItemResource::setPath(const std::string& path)
{
    _path = path;
}


Omm::ui4
DvbItemResource::getSize()
{
    return 0;
}


std::string
DvbItemResource::getMime()
{
    return "video/mpeg";
}


std::string
DvbItemResource::getDlna()
{
    return "DLNA.ORG_PN=MPEG_PS_PAL";
}


void
DvbItemPropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
DvbItemPropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


std::string
DvbItemPropertyImpl::getName()
{
    std::clog << "DvbPropertyImpl::getName() returns: " << _name << std::endl;

    return _name;
}


std::string
DvbItemPropertyImpl::getValue()
{
    std::clog << "DvbPropertyImpl::getValue() returns: " << _value << std::endl;

    if (_name == "dc:title") {
        return _value;
    }
}


DvbItemProperty::DvbItemProperty() :
AbstractProperty(new DvbItemPropertyImpl)
{
}


DvbItem::DvbItem(DvbServer* pServer) :
StreamingMediaItem(pServer),
_pTitleProp(new DvbItemProperty),
_pResource(new DvbItemResource(pServer, this))
{
    std::clog << "DvbMediaItem::DvbMediaItem(pServer), pServer: " << pServer << std::endl;
    _pTitleProp->setName("dc:title");
}


DvbItem::~DvbItem()
{
    delete _pTitleProp;
}


int
DvbItem::getPropertyCount(const std::string& name)
{
    std::clog << "DvbMediaItem::getPropertyCount(name), name: " << name << std::endl;
    
    if (name == "") {
        return 2;
    }
    else if (name == "dc:title" || name == "res") {
        return 1;
    }
    else {
        return 0;
    }
}


Omm::Av::AbstractProperty*
DvbItem::getProperty(int index)
{
    std::clog << "DvbMediaItem::getProperty(index), index: " << index << std::endl;
    
    if (index == 0) {
        return _pTitleProp;
    }
    else if (index == 1) {
        return _pResource;
    }
}


Omm::Av::AbstractProperty*
DvbItem::getProperty(const std::string& name, int index)
{
    std::clog << "DvbMediaItem::getProperty(name, index), name: " << name << ", index: " << index << std::endl;

    if (name == "dc:title") {
        return _pTitleProp;
    }
    else if (name == "res") {
        return _pResource;
    }
    else {
        return 0;
    }
}


void
DvbItem::setPath(const std::string& path)
{
    _pTitleProp->setValue(path);
    _pResource->setPath(path);
}


void
DvbItem::setChannel(const std::string& channelName, Omm::Dvb::DvbChannel* pChannel)
{
    _pTitleProp->setValue(channelName);
    _pResource->_pChannel = pChannel;
}


DvbServer::DvbServer() :
// Omm::Av::StreamingMediaObject(9999),
_pChild(static_cast<DvbItem*>(createChildObject()))
{
    Omm::Dvb::DvbAdapter* pAdapter = new Omm::Dvb::DvbAdapter(0);
    Omm::Dvb::DvbDevice::instance()->addAdapter(pAdapter);
}


DvbServer::~DvbServer()
{
    delete _pTitleProp;
    delete _pChild;
}


void
DvbServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        setBasePath(value);
    }
}


Omm::ui4
DvbServer::getChildCount()
{
    return _channelNames.size();
}


bool
DvbServer::isContainer()
{
    return true;
}


Omm::Av::AbstractMediaObject*
DvbServer::getChild(Omm::ui4 numChild)
{
    _pChild->setObjectNumber(numChild);
//    _pChild->setPath(_files[numChild].path().substr(_basePath.length()+1));
    _pChild->setChannel(_channelNames[numChild], _channels[numChild]);
    return _pChild;
}


int
DvbServer::getPropertyCount(const std::string& name)
{
    // only one property overall and one title property in particular
    if (name == "" || name == "dc:title") {
        return 1;
    }
    else {
        return 0;
    }
}


Omm::Av::AbstractProperty*
DvbServer::getProperty(int index)
{
    return _pTitleProp;
}


Omm::Av::AbstractProperty*
DvbServer::getProperty(const std::string& name, int index)
{
    if (name == "dc:title") {
        return _pTitleProp;
    }
    else {
        return 0;
    }
}


void
DvbServer::addProperty(Omm::Av::AbstractProperty* pProperty)
{
    _pTitleProp = pProperty;
}


Omm::Av::AbstractProperty*
DvbServer::createProperty()
{
    return new Omm::Av::MemoryProperty;
}


Omm::Av::AbstractMediaObject*
DvbServer::createChildObject()
{
    std::clog << "DvbServer::createChildObject()" << std::endl;
    
    DvbItem* pRes = new DvbItem(this);
    pRes->setParent(this);

    return pRes;
}


void
DvbServer::setBasePath(const std::string& channelFileName)
{
//     _basePath = basePath;
//     Poco::File baseDir(basePath);
//     scanDirectory(baseDir);

//    _channelConfigFile = channelFileName;
    scanChannelConfig(channelFileName);
}


void
DvbServer::scanDirectory(Poco::File& directory)
{
    Poco::DirectoryIterator dir(directory);
    Poco::DirectoryIterator end;
    while(dir != end) {
        if (dir->isFile()) {
//             std::clog << "DvbServer::setBasePath() adding file: " << dir.name() << std::endl;
            _files.push_back(*dir);
        }
        else if (dir->isDirectory()) {
//             std::clog << "DvbServer::setBasePath() adding directory: " << dir.name() << std::endl;
            scanDirectory(*dir);
        }
        ++dir;
    }
}


void
DvbServer::scanChannelConfig(const std::string& channelConfig)
{
    std::ifstream channels(channelConfig.c_str());
    std::string line;
    while (getline(channels, line)) {
        Poco::StringTokenizer channelParams(line, ":");
        Poco::StringTokenizer channelName(channelParams[0], ";");
        _channelNames.push_back(channelName[0]);
        unsigned int freq = Poco::NumberParser::parseUnsigned(channelParams[1]) * 1000;
        Omm::Dvb::DvbChannel::Polarization pol = (channelParams[2][0] == 'h') ? Omm::Dvb::DvbChannel::HORIZ : Omm::Dvb::DvbChannel::VERT;
        unsigned int symbolRate = Poco::NumberParser::parseUnsigned(channelParams[4]) * 1000;
        Poco::StringTokenizer videoPid(channelParams[5], "+");
        unsigned int vpid = Poco::NumberParser::parseUnsigned(videoPid[0]);
        Poco::StringTokenizer audioChannel(channelParams[6], ";");
        Poco::StringTokenizer audioPid(audioChannel[0], "=");
        unsigned int apid = Poco::NumberParser::parseUnsigned(audioPid[0]);
        int sid = Poco::NumberParser::parseUnsigned(channelParams[9]);
        _channels.push_back(new Omm::Dvb::DvbChannel(0, freq, pol, symbolRate, vpid, apid, sid));
    }
    
}


// Poco::File&
// DvbServer::getFileReference(Omm::ui4 childNum)
// {
//     return _files[childNum];
// }



POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(DvbServer)
POCO_END_MANIFEST
