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
#include <Omm/UpnpAvLogger.h>
#include "QtWidgetList.h"


QtWidgetListModel::QtWidgetListModel() :
_pView(0)
{
}


void
QtWidgetListModel::insertItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Omm::Av::Log::instance()->upnpav().debug("widget list model insert row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        _pView->insertItem(row);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list model tries to insert item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
QtWidgetListModel::removeItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Omm::Av::Log::instance()->upnpav().debug("widget list model remove row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        _pView->removeItem(row);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list model tries to remove item in row number not less than total row count or less than zero (ignoring)");
    }
}


QtWidgetListView::QtWidgetListView() :
_pModel(0)
{
}


void
QtWidgetListView::setModel(QtWidgetListModel* pModel)
{
    // double link model and view.
    _pModel = pModel;
    _pModel->_pView = this;

    // create an initial widget pool. This also retrieves the height of the widget.
    extendWidgetPool(10);
}


void
QtWidgetListView::extendWidgetPool(int n)
{
    for (int i = 0; i < n; ++i) {
        QWidget* pWidget = _pModel->createWidget();
        pWidget->hide();
        _widgetPool.push_back(pWidget);
        _freeWidgets.push(pWidget);
        addWidgetToView(pWidget);

//        Omm::Util::Log::instance()->util().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
        Omm::Av::Log::instance()->upnpav().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
    }
}


void
QtWidgetListView::insertItem(int row)
{
    // check if item is visible
//    if (!itemIsVisible(row)) {
//        return;
//    }

    // attach item to a free (not visible) widget
    if (_freeWidgets.size()) {
        QWidget* pWidget = _freeWidgets.top();
        _freeWidgets.pop();
        _pModel->attachWidget(row, pWidget);
        insertItemAndWidgetIntoView(row, pWidget);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to attach widget to item, widget pool is empty (ignoring)");
    }
}


void
QtWidgetListView::removeItem(int row)
{
    // check if item is visible
//    if (!itemIsVisible(row)) {
//        return;
//    }
    
    // detach item from visible widget
    QWidget* pWidget = _pModel->getWidget(row);
    removeItemAndWidgetFromView(row, pWidget);
    _pModel->detachWidget(row);
    _freeWidgets.push(pWidget);
}


QtWidgetList::QtWidgetList(QWidget* pParent) :
QGraphicsView(pParent)
{
    _pGraphicsScene = new QGraphicsScene;
    setScene(_pGraphicsScene);
}


QtWidgetList::~QtWidgetList()
{
}


bool
QtWidgetList::itemIsVisible(int row)
{

}


void
QtWidgetList::addWidgetToView(QWidget* pWidget)
{
    QGraphicsProxyWidget* pProxyWidget = _pGraphicsScene->addWidget(pWidget);
    _proxyWidgetPool[pWidget] = pProxyWidget;
    _widgetHeight = pWidget->height();
}


void
QtWidgetList::insertItemAndWidgetIntoView(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt widget list insert row: " + Poco::NumberFormatter::format(row));
    
    _proxyWidgetPool[pWidget]->setPos(-100, _widgetHeight * row);
}


void
QtWidgetList::removeItemAndWidgetFromView(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt widget list remove row: " + Poco::NumberFormatter::format(row));

}
