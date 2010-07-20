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


class FileResource : public Omm::Av::MemoryResource
{
public:
    virtual bool isSeekable();
    virtual std::streamsize stream(std::ostream& ostr, std::iostream::pos_type seek);
    
    void setBasePath(const std::string& path);
    void setPath(const std::string& path);
    
private:
    std::string         _basePath;
    std::string         _path;
};


bool
FileResource::isSeekable()
{
    return true;
}


std::streamsize
FileResource::stream(std::ostream& ostr, std::iostream::pos_type seek)
{
    std::string path = _basePath + "/" + _path;
    
    std::ifstream istr(path.c_str());
    if (seek > 0) {
        istr.seekg(seek);
    }
    return Poco::StreamCopier::copyStream(istr, ostr);
}


void
FileResource::setBasePath(const std::string& path)
{
    _basePath = path;
}


void
FileResource::setPath(const std::string& path)
{
    _path = path;
}


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
    return _name;
}


std::string
FilePropertyImpl::getValue()
{
    if (_name == "dc:title") {
        return _value;
    }
    else if (_name == "res") {
        return "";
    }
}


class FileItemProperty : public Omm::Av::AbstractProperty
{
public:
    FileItemProperty();
};


FileItemProperty::FileItemProperty() :
AbstractProperty(new FilePropertyImpl)
{
}


class FileMediaItem : public Omm::Av::AbstractMediaObject
{
public:
    FileMediaItem();
    virtual ~FileMediaItem();
    
    virtual int getPropertyCount(const std::string& name = "");
    virtual Omm::Av::AbstractProperty* getProperty(int index);
    virtual Omm::Av::AbstractProperty* getProperty(const std::string& name, int index = 0);
//     virtual AbstractProperty* getProperty(const std::string& name, const std::string& value);  // server object, write meta data

    void setBasePath(const std::string& basePath);

private:
    FileItemProperty*           _pTitleProp;
    FileResource*               _pResource;
};


FileMediaItem::FileMediaItem() :
_pTitleProp(new FileItemProperty),
_pResource(new FileResource)

{
    _pTitleProp->setName("dc:title");
    _pTitleProp->setValue("video.mpg");
}


FileMediaItem::~FileMediaItem()
{
    delete _pTitleProp;
}


int
FileMediaItem::getPropertyCount(const std::string& name)
{
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
FileMediaItem::getProperty(int index)
{
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
FileMediaItem::setBasePath(const std::string& basePath)
{
    _pResource->setBasePath(basePath);
}


FileServer::FileServer() :
Omm::Av::StreamingMediaObject(9999),
_pChild(new FileMediaItem)
{
    _pChild->setParent(this);
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
    _pChild->getProperty(0)->setValue(_fileNames[numChild]);
    _pChild->setObjectNumber(numChild);
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


void
FileServer::setBasePath(const std::string& basePath)
{
    _pChild->setBasePath(basePath);
    Poco::File baseDir(basePath);
    baseDir.list(_fileNames);
}


POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(FileServer)
POCO_END_MANIFEST
