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
#include "Filesystem.h"


class FileResource : public Omm::Av::StreamingResource
{
    friend class FileServer;
    
public:
    FileResource(FileServer* pServer, Omm::Av::AbstractMediaObject* pItem);
    
    virtual bool isSeekable();
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
//     void setBasePath(const std::string& path);
    void setPath(const std::string& path);
    
private:
//     Omm::Av::StreamingMediaObject*      _pStreamingObject;
//     FileServer*                         _pServer;
//     FileResourceImpl*                   _pResourceImpl;
//     std::string                         _basePath;
    std::string                         _path;
};


class FileItemProperty : public Omm::Av::AbstractProperty
{
public:
    FileItemProperty();
};


class FileMediaItem : public Omm::Av::StreamingMediaItem
{
    friend class FileServer;
    
public:
    FileMediaItem(FileServer* pServer);
    virtual ~FileMediaItem();
    
    virtual int getPropertyCount(const std::string& name = "");
    virtual Omm::Av::AbstractProperty* getProperty(int index);
    virtual Omm::Av::AbstractProperty* getProperty(const std::string& name, int index = 0);
//     virtual AbstractProperty* getProperty(const std::string& name, const std::string& value);  // server object, write meta data

    void setPath(const std::string& path);
//     void setBasePath(const std::string& basePath);

private:
//     Omm::Av::StreamingMediaObject*      _pStreamingObject;
    FileItemProperty*                   _pTitleProp;
    FileResource*                       _pResource;
//     Omm::Av::AbstractResource*          _pResource;
};


// class FileResourceImpl : public Omm::Av::ResourceImpl
// {
//     friend class FileResource;
//     
// public:
// //     virtual std::string getName();
//     virtual std::string getValue();
//     
// private:
//     FileMediaItem* _pItem;
//     FileServer*    _pServer;
// };


class FilePropertyImpl : public Omm::Av::PropertyImpl
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


// std::string
// FileResourceImpl::getName()
// {
//     std::clog << "FileResource::getName()" << std::endl;
//     
//     return "res";
// }


// std::string
// FileResourceImpl::getValue()
// {
//     std::clog << "FileResource::getValue()" << std::endl;
//     
//     std::string serverAddress = _pServer->getServerAddress();
//     std::string relativeObjectId = _pItem->getObjectId().substr(_pServer->getObjectId().length()+1);
// //     std::string resourceId = Poco::NumberFormatter::format(getResourceNumber());
//     return serverAddress + "/" + relativeObjectId + "$" /*+ resourceId*/;
//     
// //     return _pServer->getServerAddress() + "/" + _pItem->getObjectId();
// }


FileResource::FileResource(FileServer* pServer, Omm::Av::AbstractMediaObject* pItem) :
StreamingResource(new Omm::Av::MemoryPropertyImpl, pServer, pItem)
// _pResourceImpl(new FileResourceImpl),
// AbstractResource(new FileResourceImpl)
{
    std::clog << "FileResource::FileResource(pServer, pItem), pServer: " << pServer << ", pItem: " << pItem << std::endl;

//     static_cast<FileResourceImpl*>(_pPropertyImpl)->_pServer = _pStreamingObject;
}


bool
FileResource::isSeekable()
{
    return true;
}


std::streamsize
FileResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    std::clog << "FileResource::stream(), relative path: " << _path << ", _pServer: " << _pServer << std::endl;
    std::string path = static_cast<FileServer*>(_pServer)->_basePath + "/" + _path;
    std::clog << "FileResource::stream(), absolute path: " << path << std::endl;
    
    std::ifstream istr(path.c_str());
    if (seek > 0) {
        istr.seekg(seek);
    }
    return Poco::StreamCopier::copyStream(istr, ostr);
}


void
FileResource::setPath(const std::string& path)
{
    _path = path;
    setProtInfo("http-get:*:audio/mpeg:DLNA.ORG_PN=MP3;DLNA.ORG_OP=01");
    setSize(0);
}


void
FilePropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
FilePropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


std::string
FilePropertyImpl::getName()
{
    std::clog << "FilePropertyImpl::getName() returns: " << _name << std::endl;

    return _name;
}


std::string
FilePropertyImpl::getValue()
{
    std::clog << "FilePropertyImpl::getValue() returns: " << _value << std::endl;

    if (_name == "dc:title") {
        return _value;
    }
}


FileItemProperty::FileItemProperty() :
AbstractProperty(new FilePropertyImpl)
{
}


FileMediaItem::FileMediaItem(FileServer* pServer) :
StreamingMediaItem(pServer),
_pTitleProp(new FileItemProperty),
_pResource(new FileResource(pServer, this))
// _pResource(_pStreamingObject->createResource())

{
    std::clog << "FileMediaItem::FileMediaItem(pServer), pServer: " << pServer << std::endl;
    _pTitleProp->setName("dc:title");
}


FileMediaItem::~FileMediaItem()
{
    delete _pTitleProp;
}


int
FileMediaItem::getPropertyCount(const std::string& name)
{
    std::clog << "FileMediaItem::getPropertyCount(name), name: " << name << std::endl;
    
    if (name == "") {
//         return 1;
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
FileMediaItem::getProperty(int index)
{
    std::clog << "FileMediaItem::getProperty(index), index: " << index << std::endl;
    
    if (index == 0) {
        return _pTitleProp;
    }
    else if (index == 1) {
        return _pResource;
    }
}


Omm::Av::AbstractProperty*
FileMediaItem::getProperty(const std::string& name, int index)
{
    std::clog << "FileMediaItem::getProperty(name, index), name: " << name << ", index: " << index << std::endl;

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
FileMediaItem::setPath(const std::string& path)
{
    _pTitleProp->setValue(path);
    _pResource->setPath(path);
}


// void
// FileMediaItem::setBasePath(const std::string& basePath)
// {
//     _pResource->setBasePath(basePath);
// }


FileServer::FileServer() :
Omm::Av::StreamingMediaObject(9999),
_pChild(static_cast<FileMediaItem*>(createChildObject()))
{
}


FileServer::~FileServer()
{
    delete _pTitleProp;
    delete _pChild;
}


void
FileServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        setBasePath(value);
    }
}


Omm::ui4
FileServer::getChildCount()
{
    return _fileNames.size();
}


bool
FileServer::isContainer()
{
    return true;
}


Omm::Av::AbstractMediaObject*
FileServer::getChild(Omm::ui4 numChild)
{
//     _pChild->getProperty(0)->setValue(_fileNames[numChild]);
    _pChild->setObjectNumber(numChild);
    _pChild->setPath(_fileNames[numChild]);
//     _pChild->getResource(0)->setPath(_fileNames[numChild]);
    return _pChild;
}


int
FileServer::getPropertyCount(const std::string& name)
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
FileServer::getProperty(int index)
{
    return _pTitleProp;
}


Omm::Av::AbstractProperty*
FileServer::getProperty(const std::string& name, int index)
{
    if (name == "dc:title") {
        return _pTitleProp;
    }
    else {
        return 0;
    }
}


void
FileServer::addProperty(Omm::Av::AbstractProperty* pProperty)
{
    _pTitleProp = pProperty;
}


Omm::Av::AbstractProperty*
FileServer::createProperty()
{
    return new Omm::Av::MemoryProperty;
}


Omm::Av::AbstractMediaObject*
FileServer::createChildObject()
{
    std::clog << "FileServer::createChildObject()" << std::endl;
    
    FileMediaItem* pRes = new FileMediaItem(this);
// //     pRes->_pStreamingObject = this;
    pRes->setParent(this);
//     pRes->_pResource->_pServer = this;
//     pRes->_pResource->setUri(getServerAddress());

    return pRes;
}


void
FileServer::setBasePath(const std::string& basePath)
{
    _basePath = basePath;
    Poco::File baseDir(basePath);
    baseDir.list(_fileNames);
}


POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
