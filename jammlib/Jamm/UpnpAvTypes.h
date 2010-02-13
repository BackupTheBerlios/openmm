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
#ifndef JAMMUPNPAV_TYPES_H
#define JAMMUPNPAV_TYPES_H

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

#include "Upnp.h"
#include "UpnpAvControllers.h"


namespace Jamm {
namespace Av {

class AvTypeConverter
{
public:
    static r8 readDuration(const std::string& duration);
    static std::string writeDuration(const r8& duration);
};


class MediaObject
{
public:
    // NOTE: instead of this ctor make a MediaObjectWriter
    // MediaObject* createObject(const std::string&)
    // MediaObject* createObject(DatabasePointer)
    // and a MediaObjectReader ...
    
    MediaObject();
    // NOTE: should be split into a Controller and Device part?
//     MediaObject(const std::string& metaData);
//     MediaObject(Poco::XML::Node* pNode);
    
    bool isContainer() { return m_isContainer; }
    const std::string& getTitle();
    int fetchChildren();
    
    const std::string& getProperty(const std::string& name);
    
    MediaObject*                            m_parent;
    std::vector<MediaObject*>               m_children;
    std::string                             m_objectId;
    std::string                             m_parentId;
    unsigned int                            m_childCount;
    bool                                    m_fetchedAllChildren;
    
    
    // object properties:
//     const std::string& getId();
//     void setId(const std::string& id);
//     std::string getParentId();
//     void setParentId(const std::string& id);
//     void setTitle(const std::string& id);
//     std::string getRes();
//     void setRes(const std::string& id);
//     std::string getClass();
//     void setClass(const std::string& id);
    
    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);
    
// private:
    ContentDirectoryController*             m_server;
    Container<Variant>                      m_properties;
    bool                                    m_isContainer;
};


} // namespace Av
} // namespace Jamm

#endif
