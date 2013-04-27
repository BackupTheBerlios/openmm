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
#ifndef UpnpAvCtlObject_INCLUDED
#define UpnpAvCtlObject_INCLUDED

#include <Poco/Timer.h>
#include <Poco/DOM/Node.h>
#include <Poco/Notification.h>

#include "Upnp.h"
#include "UpnpInternal.h"
#include "UpnpAvObject.h"


namespace Omm {
namespace Av {

class CtlMediaServer;
class CtlMediaServerCode;
class MediaItemNotification;


class CtlMediaObject : public MemoryMediaObject, public BlockCache
{
    friend class CtlMediaServer;
    friend class CtlMediaRenderer;

    friend class AvServerView;
    friend class AvRendererView;

public:
    CtlMediaObject();
    CtlMediaObject(const CtlMediaObject& mediaObject);

    // factory methods
    virtual AbstractMediaObject* createChildObject();

    CtlMediaServer* getServer() const;
    void setServer(CtlMediaServer* pServer);
    void setServerController(CtlMediaServerCode* pServerCode);

    virtual std::string getId();
    virtual void setId(const std::string& id);
    void setSearch(const std::string& searchText);

    int fetchChildren(ui4 count = 25);
    bool fetchedAllChildren();
    AbstractMediaObject* getParent();

    virtual AbstractMediaObject* getChildForRow(ui4 row, bool useBlockCache = true);
    /// if you don't use the block cache, you have to fetch some children first with fetchChildren()

    Icon* getIcon();
    Icon* getImageRepresentation();

    void writeResource(const std::string& sourceUri, int index = 0);
    void createChildObject(CtlMediaObject* pObject);
    void destroyObject(const std::string& objectId);

private:
    virtual void getBlock(std::vector<AbstractMediaObject*>& block, ui4 offset, ui4 size);

    std::string                      _id;
    CtlMediaServer*                  _pServer;
    CtlMediaServerCode*              _pServerCode;
    std::string                      _searchText;
    AbstractMediaObject*             _pParent;
};


class MediaObjectSelectedNotification : public Poco::Notification
{
public:
    MediaObjectSelectedNotification(CtlMediaObject* pObject, CtlMediaObject* pParentObject, ui4 row);

//    CtlMediaObject* getMediaItem() const;

//private:
    CtlMediaObject*     _pObject;
    CtlMediaObject*     _pParentObject;
    ui4                 _row;
};


} // namespace Av
} // namespace Omm


#endif
