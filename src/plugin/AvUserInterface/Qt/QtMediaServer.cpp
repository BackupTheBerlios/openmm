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

#include "QtMediaServer.h"
#include "QtWidgetList.h"
#include "QtNavigator.h"


QtMediaServer::QtMediaServer() :
_pMediaServerWidget(0),
_charEncoding(QTextCodec::codecForName("UTF-8"))
{
}


QtMediaServer::~QtMediaServer()
{
}


void
QtMediaServer::setDeviceWidget(QtMediaServerWidget* pWidget)
{
    _pMediaServerWidget = pWidget;
}


QtMediaServerWidget*
QtMediaServer::getDeviceWidget()
{
    return _pMediaServerWidget;
}


QString
QtMediaServer::getBrowserTitle()
{
    return QString(getFriendlyName().c_str());
}


//QWidget*
//QtMediaServer::getWidget()
//{
////    return _pMediaServerWidget->getContainerWidget();
//    return _pMediaContainerWidget;
//}


void
QtMediaServer::initController()
{
    Omm::Av::Log::instance()->upnpav().debug("init Qt media server (controller)");
    // TODO: check if root object is an item or container and change the visual appearance accordingly.
    browseRootObject();
}


//void
//QtMediaServer::selected()
//{
//    Omm::Av::Log::instance()->upnpav().debug("Qt media server selected (controller)");
//    Omm::Av::CtlMediaObject* pRootObject = getRootObject();
//    QtNavigator* pNavigator = getNavigator();
//    if (pRootObject->isContainer() && pNavigator) {
//        Omm::Av::Log::instance()->upnpav().debug("Qt media server root object is container, creating container widget.");
//        QtMediaObject* pRootWidget = new QtMediaObject;
//        pRootWidget->_pObject = pRootObject;
//        pRootObject->setWidgetFactory(pRootWidget);
//        pRootWidget->_pContainerView = new QtWidgetList;
//        pRootWidget->_pContainerView->setModel(pRootObject);
////        _pMediaContainerWidget = new QtWidgetList;
////        _pMediaContainerWidget->setModel(pRootObject);
//        Omm::Av::Log::instance()->upnpav().debug("Qt media server pushing root container widget ...");
//        pNavigator->push(pRootWidget);
////        pNavigator->push(this);
//    }
//}


QtMediaServerWidget::QtMediaServerWidget(QtMediaServer* pMediaServer) :
_pMediaServer(pMediaServer)
{
}


void
QtMediaServerWidget::configure()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media server widget set name: " + _pMediaServer->getFriendlyName());

    setLabel(_pMediaServer->getFriendlyName());
}


void
QtMediaServerWidget::unconfigure()
{
    setLabel("");
}