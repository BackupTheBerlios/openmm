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

#include <Poco/NumberFormatter.h>

#include <Omm/Util.h>

#include "ListImpl.h"
#include "Gui/List.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


MovableListViewImpl::MovableListViewImpl(View* pView, bool movableViews, View* pParent) :
QGraphicsView(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
ViewImpl(pView, this),
_movableViews(movableViews)
{
    LOG(gui, debug, "list view impl ctor");
    setAlignment((Qt::AlignLeft | Qt::AlignTop));

    _pGraphicsScene = new QGraphicsScene;
    setScene(_pGraphicsScene);

    connect(this, SIGNAL(moveWidgetSignal(int, View*)), this, SLOT(moveWidgetSlot(int, View*)));
    connect(this, SIGNAL(extendPoolSignal()), this, SLOT(extendPoolSlot()));
}


MovableListViewImpl::~MovableListViewImpl()
{
}


int
MovableListViewImpl::visibleRows()
{
    Log::instance()->gui().debug("list view impl viewport width: " + Poco::NumberFormatter::format(viewport()->geometry().width())
            + ", height: " Poco::NumberFormatter::format(viewport()->geometry().height()));
    ListView* pListView =  static_cast<ListView*>(_pView);
    int rows = viewport()->geometry().height() / pListView->_viewHeight;
    LOG(gui, debug, "list view impl number of visible rows: " + Poco::NumberFormatter::format(rows));
    return rows;
}


void
MovableListViewImpl::addItemView(View* pView)
{
    LOG(gui, debug, "list view impl add item view");
    ListView* pListView =  static_cast<ListView*>(_pView);
    pView->resize(viewport()->width(), pListView->_viewHeight);

    QWidget* pQWidget = static_cast<QWidget*>(pView->getNativeView());
    if (_movableViews) {
        _proxyWidgets[pView] = _pGraphicsScene->addWidget(pQWidget, Qt::Window);
    }
    else {
        _proxyWidgets[pView] = _pGraphicsScene->addWidget(pQWidget);
   }
}


void
MovableListViewImpl::moveItemView(int row, View* pView)
{
    LOG(gui, debug, "list view impl move item widget to row: " + Poco::NumberFormatter::format(row));
    emit moveWidgetSignal(row, pView);
}


void
MovableListViewImpl::extendViewPool()
{
    emit extendPoolSignal();
}


void
MovableListViewImpl::moveWidgetSlot(int row, View* pView)
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    _proxyWidgets[pView]->setPos(0, pListView->_viewHeight * row);
}


void
MovableListViewImpl::extendPoolSlot()
{
    ListView* pListView =  static_cast<ListView*>(_pView);
    pListView->extendViewPool(visibleRows());
}


}  // namespace Omm
}  // namespace Gui
