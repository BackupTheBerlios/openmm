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
#include "Filesystem.h"


class FileItemResource : public Omm::Av::StreamingResource
{
    friend class FileServer;
    
public:
    FileItemResource(FileServer* pServer, Omm::Av::AbstractMediaObject* pItem);
    
    virtual Omm::ui4 getSize();
    virtual std::string getMime();
    virtual std::string getDlna();
    
    virtual bool isSeekable();
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
    void setPath(const std::string& path);
    
private:
    std::string         _path;
};


class FileItemProperty : public Omm::Av::AbstractProperty
{
public:
    FileItemProperty();
};


class FileItemPropertyImpl : public Omm::Av::PropertyImpl
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


class FileItem : public Omm::Av::StreamingMediaItem
{
    friend class FileServer;
    
public:
    FileItem(FileServer* pServer);
    virtual ~FileItem();
    
    virtual int getPropertyCount(const std::string& name = "");
    virtual Omm::Av::AbstractProperty* getProperty(int index);
    virtual Omm::Av::AbstractProperty* getProperty(const std::string& name, int index = 0);

    void setPath(const std::string& path);

private:
    FileItemProperty*                   _pTitleProp;
    FileItemResource*                   _pResource;
};


FileItemResource::FileItemResource(FileServer* pServer, Omm::Av::AbstractMediaObject* pItem) :
StreamingResource(new Omm::Av::MemoryPropertyImpl, pServer, pItem)
{
}


bool
FileItemResource::isSeekable()
{
    return true;
}


std::streamsize
FileItemResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
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
FileItemResource::setPath(const std::string& path)
{
    _path = path;
}


Omm::ui4
FileItemResource::getSize()
{
    return static_cast<FileServer*>(_pServer)->getFileReference(_pItem->getObjectNumber()).getSize();
}


std::string
FileItemResource::getMime()
{
    // TODO: Tagger should determine the mime type
    return "*";
}


std::string
FileItemResource::getDlna()
{
    // TODO: Tagger should determine the dlna string
    return "*";
}


void
FileItemPropertyImpl::setName(const std::string& name)
{
    _name = name;
}


void
FileItemPropertyImpl::setValue(const std::string& value)
{
    _value = value;
}


std::string
FileItemPropertyImpl::getName()
{
    std::clog << "FilePropertyImpl::getName() returns: " << _name << std::endl;

    return _name;
}


std::string
FileItemPropertyImpl::getValue()
{
    std::clog << "FilePropertyImpl::getValue() returns: " << _value << std::endl;

    if (_name == "dc:title") {
        return _value;
    }
}


FileItemProperty::FileItemProperty() :
AbstractProperty(new FileItemPropertyImpl)
{
}


FileItem::FileItem(FileServer* pServer) :
StreamingMediaItem(pServer),
_pTitleProp(new FileItemProperty),
_pResource(new FileItemResource(pServer, this))
{
    std::clog << "FileMediaItem::FileMediaItem(pServer), pServer: " << pServer << std::endl;
    _pTitleProp->setName("dc:title");
}


FileItem::~FileItem()
{
    delete _pTitleProp;
}


int
FileItem::getPropertyCount(const std::string& name)
{
    std::clog << "FileMediaItem::getPropertyCount(name), name: " << name << std::endl;
    
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
FileItem::getProperty(int index)
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
FileItem::getProperty(const std::string& name, int index)
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
FileItem::setPath(const std::string& path)
{
    _pTitleProp->setValue(path);
    _pResource->setPath(path);
}


FileServer::FileServer() :
// Omm::Av::StreamingMediaObject(9999),
_pChild(static_cast<FileItem*>(createChildObject()))
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
//     return _fileNames.size();
    return _files.size();
}


bool
FileServer::isContainer()
{
    return true;
}


Omm::Av::AbstractMediaObject*
FileServer::getChild(Omm::ui4 numChild)
{
    _pChild->setObjectNumber(numChild);
//     _pChild->setPath(_fileNames[numChild]);
    _pChild->setPath(_files[numChild].path().substr(_basePath.length()+1));
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
    
    FileItem* pRes = new FileItem(this);
    pRes->setParent(this);

    return pRes;
}


void
FileServer::setBasePath(const std::string& basePath)
{
    _basePath = basePath;
    Poco::File baseDir(basePath);
//     baseDir.list(_fileNames);
//     baseDir.list(_files);
    scanDirectory(baseDir);

}


void
FileServer::scanDirectory(Poco::File& directory)
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


Poco::File&
FileServer::getFileReference(Omm::ui4 childNum)
{
    return _files[childNum];
}



POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
