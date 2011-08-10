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
#include "Util.h"


namespace Omm {
namespace Av {

class CtlMediaServer;
class CtlMediaServerCode;
class MediaItemNotification;


class CtlMediaObject : public MediaObject //, public Util::WidgetListModel
{
    friend class CtlMediaServer;
    friend class CtlMediaRenderer;

    friend class AvServerView;
    friend class AvRendererView;
    friend class AvUserInterface;

public:
    CtlMediaObject();

    CtlMediaServer* getServer() const;
    void setServer(CtlMediaServer* pServer);
    void setServerController(CtlMediaServerCode* pServerCode);

    int fetchChildren(ui4 count = 10);
    bool fetchedAllChildren();
    ui4 childCount();
    CtlMediaObject* parent();

    std::string getProperty(const std::string& name);

    Icon* getIcon();
    Icon* getImageRepresentation();

    void setListWidget(Omm::Util::ListWidget* pWidget);
    Omm::Util::ListWidget* getListWidget();

//    // WidgetListModel interface
//    virtual int totalItemCount();
//    virtual void selectItem(int row);
//
//    // lazy model related
//    virtual bool canFetchMore();
//    virtual void fetchMore(bool forward = true);
//    virtual int lastFetched(bool forward = true);

private:
    Resource* getResource(int num = 0);
    virtual void addResource(Resource* pResource);
    void setFetchedAllChildren(bool fetchedAllChildren);

    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);

    unsigned int                     _childCount;
    bool                             _fetchedAllChildren;
    CtlMediaServer*                  _pServer;
    CtlMediaServerCode*              _pServerCode;
    Omm::Util::ListWidget*           _pListWidget;
};


class MediaItemNotification : public Poco::Notification
{
public:
    MediaItemNotification(CtlMediaObject* pItem);

    CtlMediaObject* getMediaItem() const;

private:
    CtlMediaObject*     _pItem;
};


} // namespace Av
} // namespace Omm


#endif
