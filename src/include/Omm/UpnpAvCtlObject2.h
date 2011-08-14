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
#ifndef UpnpAvCtlObject2_INCLUDED
#define UpnpAvCtlObject2_INCLUDED

#include <Poco/Timer.h>
#include <Poco/DOM/Node.h>
#include <Poco/Notification.h>

#include "Upnp.h"
#include "UpnpInternal.h"
#include "UpnpAvObject.h"
#include "Gui/Gui.h"


namespace Omm {
namespace Av {

class CtlMediaServer;
class CtlMediaServerCode;
class MediaItemNotification;


class CtlMediaObject2 : public MemoryMediaObject //, public Util::WidgetListModel
{
    friend class CtlMediaServer;
    friend class CtlMediaRenderer;

    friend class AvServerView;
    friend class AvRendererView;
    friend class AvUserInterface;

public:
    CtlMediaObject2();

    // factory methods
    virtual AbstractMediaObject* createChildObject();

    CtlMediaServer* getServer() const;
    void setServer(CtlMediaServer* pServer);
    void setServerController(CtlMediaServerCode* pServerCode);

    virtual std::string getId();
    virtual void setId(const std::string& id);

    int fetchChildren(ui4 count = 10);

    Icon* getIcon();
    Icon* getImageRepresentation();

    void setListWidget(Omm::Gui::ListWidget* pWidget);
    Omm::Gui::ListWidget* getListWidget();

//    // WidgetListModel interface
//    virtual int totalItemCount();
//    virtual void selectItem(int row);
//
//    // lazy model related
//    virtual bool canFetchMore();
//    virtual void fetchMore(bool forward = true);
//    virtual int lastFetched(bool forward = true);

private:
    std::string                      _id;
    CtlMediaServer*                  _pServer;
    CtlMediaServerCode*              _pServerCode;
    Omm::Gui::ListWidget*           _pListWidget;
};


class MediaItemNotification2 : public Poco::Notification
{
public:
    MediaItemNotification2(CtlMediaObject2* pItem);

    CtlMediaObject2* getMediaItem() const;

private:
    CtlMediaObject2*     _pItem;
};


} // namespace Av
} // namespace Omm


#endif
