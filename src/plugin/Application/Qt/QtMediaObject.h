/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#ifndef QtMediaObject_INCLUDED
#define QtMediaObject_INCLUDED

#include <Omm/UpnpAvCtlObject.h>
#include <Omm/Util.h>

#include "QtWidget.h"
#include "QtListWidget.h"
#include "QtWidgetList.h"
#include "QtNavigable.h"


class QtMediaObject : public QtSimpleListWidget, public QtNavigable, public WidgetListModel
/// QtMediaObject has two roles:
/// As a container it implements the WidgetListModel interface of CtlMediaObject and has a pushable WidgetList for the Navigable interface.
/// As a child widget it has _pObject as an attachable pointer to its CtlMediaObject to implement configure() of its QtSimpleListWidget interface.
{
    friend class QtMediaServer;
    friend class QtMediaServerGroup;

public:
    QtMediaObject();
    ~QtMediaObject();

    // QtNavigable interface
    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();

    // WidgetListModel interface
    virtual int totalItemCount();
    virtual void selectItem(int row);

    // lazy model related
    virtual bool canFetchMore();
    virtual void fetchMore(bool forward = true);
    virtual int fetch(int rowCount = 10, bool forward = true);
    virtual int lastFetched(bool forward = true);

    // child widget related
    virtual ListWidget* createWidget();
    virtual ListWidget* getWidget(int row);
    virtual void attachWidget(int row, ListWidget* pWidget);
    virtual void detachWidget(int row);

public slots:
    virtual void configure();
    virtual void unconfigure();

private:
    Omm::Av::CtlMediaObject*    _pObject;
    QtWidgetList*               _pContainerView;
    // lazy model related
    Omm::ui4                    _lastFetched;
};


#endif

