/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/
#ifndef JAMMUPNPAV_H
#define JAMMUPNPAV_H

#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>

#include "upnp.h"

using Poco::XML::DOMParser;
using Poco::XML::Document;
using Poco::XML::NodeIterator;
using Poco::XML::NodeFilter;
using Poco::XML::Node;
using Poco::XML::NodeList;
using Poco::XML::NamedNodeMap;
using Poco::XML::AttrMap;
using Poco::XML::Element;
using Poco::XML::AutoPtr;
using Poco::XML::InputSource;
using Poco::XML::Element;
using Poco::XML::Attr;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;
using Poco::XML::DocumentFragment;

namespace Jamm {

class AvTypeConverter
{
public:
    static Jamm::r8 readDuration(const std::string& duration);
    static std::string writeDuration(const Jamm::r8& duration);
};


class MediaObject
{
public:
    // NOTE: instead of this ctor make a MediaObjectWriter
    // MediaObject* createObject(const std::string&)
    // MediaObject* createObject(DatabasePointer)
    // and a MediaObjectReader ...
    
    MediaObject(const std::string& metaData);
    
    std::string getProperty(const std::string& name);
    
    // object properties:
    std::string getId();
    void setId(const std::string& id);
    std::string getParentId();
    void setParentId(const std::string& id);
    std::string getTitle();
    void setTitle(const std::string& id);
    std::string getRes();
    void setRes(const std::string& id);
    std::string getClass();
    void setClass(const std::string& id);
    
    
private:
    AutoPtr<Document>   m_pDoc;
    Container<Variant>  m_properties;
};


class SimpleMediaObjectModel
{
public:
    // filtering is done in ContentDirectoryServiceImplementation::Browse() by using
    // -> std::string propertyFilter(const std::string& object, const std::vector<std::string>& filterString)
    
    // searching is done in ContentDirectoryServiceImplementation::Search() by using
    // -> bool objectFilter(const std::string& object, const std::string& searchString)
    
    // sorting can be implemented in different ways:
    // 1. no sorting: Metadata can be passed from cache without modification
    // 2. sort in memory: store a sorted std::vector<MediaObject>
    // 3. cached sort index: store a sorted std::vector<std::string>
    // -> MediaObjectModel implementation may offer different sort strategies to select from
    
    // caching can be implemented in different ways:
    // 1. no caching: all meta data and object ids are generated dynamically
    // 2. meta data scan: all meta data is scanned a priori and continually updated
    // 3. meta data + indices: aditionally sorted indices are cached
    // -> MediaObjectModel implementation may offer different caching strategies to select from
    
    virtual int numChildren(const std::string& objectId) = 0;

    // return meta data of object
    // may be cached on disk or dynamically generated
    virtual std::string object(const std::string& objectId) = 0;
    virtual std::string child(const std::string& containerId, 
                              Jamm::ui4 childNumber,
                              const std::vector<std::string>& propertySortList) = 0;
};


class FastMediaObjectModel
{
public:
    // searching is done in FastMediaObjectModelImplementation::children() by using
    // -> bool objectFilter(const std::string& object, const std::string& searchString)
    
    virtual int numChildren(const std::string& objectId) = 0;
    
    // return meta data of object
    // may be cached on disk or dynamically generated
    virtual std::string object(const std::string& objectId) = 0;
    virtual std::string children(const std::string& containerId, 
                                 Jamm::ui4 childNumber,
                                 Jamm::ui4 childCount,
                                 const std::vector<std::string>& propertySortList,
                                 const std::string& searchString) = 0;
};


} // namespace Jamm

#endif
