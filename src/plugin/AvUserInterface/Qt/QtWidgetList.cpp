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


int
QtWidgetListView::widgetPoolSize()
{
    return _widgetPool.size();
}


void
QtWidgetListView::extendWidgetPool(int n)
{
    for (int i = 0; i < n; ++i) {
        QWidget* pWidget = _pModel->createWidget();
        pWidget->hide();
        _widgetPool.push_back(pWidget);
        _freeWidgets.push(pWidget);
        createProxyWidget(pWidget);

//        Omm::Util::Log::instance()->util().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
        Omm::Av::Log::instance()->upnpav().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
    }
}


int
QtWidgetListView::visibleIndex(int row)
{
    // FIX: row is not correct index of visible widgets.
    return row;
}


int
QtWidgetListView::countVisibleWidgets()
{
    return _visibleWidgets.size();
}


QWidget*
QtWidgetListView::visibleWidget(int index)
{
    if (0 <= index && index < _visibleWidgets.size()) {
        return _visibleWidgets[index];
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to retrieve visible widget, out of range (ignoring)");
        return 0;
    }
}


void
QtWidgetListView::insertItem(int row)
{
    // resize view to the size with this item added
    updateSize();

    // check if item is visible
//    if (!itemIsVisible(row)) {
//        Omm::Av::Log::instance()->upnpav().debug("widget list view insert item that is not visible (ignoring)");
//        return;
//    }

    // attach item to a free (not visible) widget
    if (_freeWidgets.size()) {
        QWidget* pWidget = _freeWidgets.top();
        _freeWidgets.pop();
        _visibleWidgets.insert(_visibleWidgets.begin() + visibleIndex(row), pWidget);
        _pModel->attachWidget(row, pWidget);
        showItemWidget(row, pWidget);
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("widget list view failed to attach widget to item, widget pool is empty (ignoring)");
    }
}


void
QtWidgetListView::removeItem(int row)
{
    // resize view to the size with this item added
    updateSize();

    // check if item is visible
//    if (!itemIsVisible(row)) {
//        Omm::Av::Log::instance()->upnpav().debug("widget list view insert item that is not visible (ignoring)");
//        return;
//    }
    
    // detach item from visible widget
    QWidget* pWidget = _pModel->getWidget(row);
    hideItemWidget(row, pWidget);
    _pModel->detachWidget(row);
    _visibleWidgets.erase(_visibleWidgets.begin() + visibleIndex(row));
    _freeWidgets.push(pWidget);
}


QtWidgetList::QtWidgetList(QWidget* pParent) :
//QGraphicsView(pParent)
QScrollArea(pParent)
{
//    _pGraphicsScene = new QGraphicsScene;
//    setScene(_pGraphicsScene);
//    setAlignment(Qt::AlignTop);

    _pScrollWidget = new QWidget;
   _pScrollWidget->resize(700, 100);
    setWidget(_pScrollWidget);

//     QPushButton* pButton = new QPushButton(_pScrollWidget);
//    pButton->hide();
//   _pScrollWidget->resize(800, 1000);

//    pButton->setGeometry(0, 100, 100, 20);
//    pButton->show();


//    _pScrollWidget->show();
//    pButton->show();

//    _pLayout = new QGridLayout(_pTopWidget);
//    _pLayout->addWidget(new QPushButton);
//    for (int i = 0; i < 10; i++) {
//        _pLayout->addWidget(new QPushButton);
//    }
//    _pTopWidget->resize(1, 1);
//    _proxyWidgetPool[_pTopWidget] = _pGraphicsScene->addWidget(_pTopWidget);
//    _proxyWidgetPool[_pTopWidget]->setPos(0, 0);

//    _pLayout->addWidget(new QPushButton);

//    _pBottomWidget = new QWidget;
//    _pBottomWidget->resize(1, 1);
//    _proxyWidgetPool[_pBottomWidget] = _pGraphicsScene->addWidget(_pBottomWidget);
//    _proxyWidgetPool[_pBottomWidget]->setPos(0, 1000);
//    viewport()->resize(QSize(800, 1000));
    
//    connect(this, SIGNAL(showWidget(QWidget*)), this, SLOT(show(QWidget*)));
//    connect(this, SIGNAL(hideWidget(QWidget*)), this, SLOT(hide(QWidget*)));
    connect(this, SIGNAL(moveWidget(int, QWidget*)), this, SLOT(move(int, QWidget*)));
//    connect(verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(viewScrolled(int)));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(viewScrolled(int)));
}


QtWidgetList::~QtWidgetList()
{
}


bool
QtWidgetList::itemIsVisible(int row)
{
    // TODO: implement itemIsVisible()
}


void
QtWidgetList::createProxyWidget(QWidget* pWidget)
{
//    QGraphicsProxyWidget* pProxyWidget = _pGraphicsScene->addWidget(pWidget);
//    _proxyWidgetPool[pWidget] = pProxyWidget;
    _widgetHeight = pWidget->height();

    pWidget->setParent(_pScrollWidget);
}


void
QtWidgetList::showItemWidget(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list show item widget at row: " + Poco::NumberFormatter::format(row));
    
    emit moveWidget(row, pWidget);
//    emit show(pWidget);
}


void
QtWidgetList::hideItemWidget(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list hide item widget at row: " + Poco::NumberFormatter::format(row));

//    emit hide(pWidget);
    
    int index = visibleIndex(row);
    if (index < 0) {
        Omm::Av::Log::instance()->upnpav().error("widget list failed to hide item widget, item is not visible (ignoring)");
        return;
    }
    for (int i = index + 1; i < countVisibleWidgets(); i++) {
        emit moveWidget(row + i - index - 1, visibleWidget(i));
    }
}


void
QtWidgetList::updateSize()
{
//    QSize size = viewport()->size();
//    Omm::Av::Log::instance()->upnpav().debug("viewport size: " + Poco::NumberFormatter::format(size.width()) + "," + Poco::NumberFormatter::format(size.height()));
//    size.setHeight(_pModel->totalItemCount() * _widgetHeight);
//    Omm::Av::Log::instance()->upnpav().debug("viewport size: " + Poco::NumberFormatter::format(size.width()) + "," + Poco::NumberFormatter::format(size.height()));
//    viewport()->resize(size);
//    emit moveWidget(_pModel->totalItemCount(), _pBottomWidget);
    _pScrollWidget->resize(_pScrollWidget->width(), _pModel->totalItemCount() * _widgetHeight);
}


int
QtWidgetList::getViewOffset()
{
    return _pScrollWidget->geometry().y();
    
//    QRect rv = viewport()->geometry();
////    QPoint pv = mapToGlobal(QPoint(rv.x(), rv.y()));
//    QRect rw = _pScrollWidget->geometry();
////    QPoint pw = mapToGlobal(QPoint(rw.x(), rw.y()));
//
////    QRectF rs = sceneRect();
////    QPoint ps = mapToGlobal(QPoint(rs.x(), rs.y()));
////    QPointF pt = _proxyWidgetPool[_pTopWidget]->mapToParent(_proxyWidgetPool[_pTopWidget]->pos());
////    QPointF ptg = mapToGlobal(QPoint(pt.x(), pt.y()));
//
//
////    QPoint p = viewport()->mapFromGlobal(QPoint(0,0));
//    Omm::Av::Log::instance()->upnpav().debug("viewport offset: " + Poco::NumberFormatter::format(rv.y()));
//    Omm::Av::Log::instance()->upnpav().debug("widget offset: " + Poco::NumberFormatter::format(rw.y()));
////    Omm::Av::Log::instance()->upnpav().debug("scene offset: " + Poco::NumberFormatter::format(ps.y()));
////    Omm::Av::Log::instance()->upnpav().debug("top widget offset: " + Poco::NumberFormatter::format(ptg.y()));
}


//void
//QtWidgetList::show(QWidget* pWidget)
//{
//    Omm::Av::Log::instance()->upnpav().debug("widget list show widget");
//
//    _proxyWidgetPool[pWidget]->show();
//}
//
//
//void
//QtWidgetList::hide(QWidget* pWidget)
//{
//    Omm::Av::Log::instance()->upnpav().debug("widget list hide widget");
//
//    _proxyWidgetPool[pWidget]->hide();
//}


void
QtWidgetList::move(int row, QWidget* pWidget)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list move widget to row: " + Poco::NumberFormatter::format(row));

    pWidget->move(0, _widgetHeight * row);

    // TODO: use moveBy() or scroll() instead of setPos()?
//    _proxyWidgetPool[pWidget]->setPos(0, _widgetHeight * row);
}


void
QtWidgetList::viewScrolled(int value)
{
    Omm::Av::Log::instance()->upnpav().debug("widget list scrolling ...");
    getViewOffset();
}
