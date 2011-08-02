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
QtWidgetListModel::insertWidget(int row)
{
    _pView->insertWidget(row);
}


void
QtWidgetListModel::removeWidget(int row)
{
    _pView->removeWidget(row);
}


QtWidgetListView::QtWidgetListView() :
_pModel(0)
{
}


void
QtWidgetListView::setModel(QtWidgetListModel* pModel)
{
    _pModel = pModel;
    _pModel->_pView = this;

    for (int i = 0; i < 10; ++i) {
        QWidget* pWidget = _pModel->createWidget();
        pWidget->hide();
        _widgetPool.push_back(pWidget);
        addWidget(pWidget);

//        pWidget->hide();
//        connect(_pModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)), this, SLOT(rowsAboutToBeInserted(const QModelIndex&, int, int)));
//        Omm::Util::Log::instance()->util().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
        Omm::Av::Log::instance()->upnpav().debug("allocate QtMediaRenderer[" + Poco::NumberFormatter::format(i) + "]: " + Poco::NumberFormatter::format(pWidget));
    }
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


void
QtWidgetList::addWidget(QWidget* pWidget)
{
    _proxyWidgets.push_back(_pGraphicsScene->addWidget(pWidget));
    _widgetHeight = pWidget->height();
}


void
QtWidgetList::insertWidget(int row)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt widget list insert row: " + Poco::NumberFormatter::format(row));
    _pModel->attachWidget(row, _widgetPool[row]);
    _proxyWidgets[row]->setPos(-100, _widgetHeight * row);
}


void
QtWidgetList::removeWidget(int row)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt widget list remove row: " + Poco::NumberFormatter::format(row));
    _pModel->detachWidget(row);
}


//void
//QtWidgetList::rowsAboutToBeInserted(const QModelIndex& parent, int start, int end)
//{
//    Omm::Av::Log::instance()->upnpav().debug("widget list adding item in row: " + Poco::NumberFormatter::format(start));
//    _pModel->index(start, 0, parent);
////    _pModel->attachWidget(_pModel->index(start, 0, parent), _widgetPool[start]);
//}


//QWidget*
//QtWidgetDeviceGroup::getDeviceGroupWidget()
//{
//    return _pGrahpicsView;
//}
//
//
//void
//QtWidgetDeviceGroup::addWidget(QWidget* pWidget)
//{
//    _widgetPool.push_back(pWidget);
//    _pGraphicsScene->addWidget(pWidget);
//    pWidget->hide();
////    pWidget->show();
//}
//
//
//QWidget*
//QtWidgetDeviceGroup::getWidget()
//{
//    Omm::Av::Log::instance()->upnpav().debug("get device widget[" + Poco::NumberFormatter::format(_lastWidget) + "]: " + Poco::NumberFormatter::format(_widgetPool[_lastWidget]));
//
//    QWidget* pRes = _widgetPool[_lastWidget];
//    _lastWidget++;
//
//    return pRes;
//}
//
//
//void
//QtWidgetDeviceGroup::showWidget(QWidget* pWidget)
//{
//    Omm::Av::Log::instance()->upnpav().debug("Qt widget device group show device widget");
//
//    pWidget->show();
////    _pGraphicsScene->addWidget(pWidget);
//}
