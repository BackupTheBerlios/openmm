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

#include <Omm/UpnpAv.h>
#include <Omm/UpnpAvController.h>

#include "QtMediaRendererGroup.h"
#include "QtMediaRendererControlPanel.h"
#include "QtMediaRenderer.h"
#include "QtController.h"
#include "QtListWidget.h"
#include "QtWidgetList.h"


QtMediaRendererGroup::QtMediaRendererGroup() :
DeviceGroupModel(new Omm::Av::MediaRendererGroupDelegate)
{
//    _pWidgetList = new QtWidgetList;
//    _pWidgetList->setModel(this);

    // widgets movable:
    _pWidgetCanvas = new QtWidgetCanvas(true);
    // widgets not movable
//    _pWidgetCanvas = new QtWidgetCanvas;
    _pWidgetCanvas->setModel(this);
}


Omm::Device*
QtMediaRendererGroup::createDevice()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group create media renderer");
    return new QtMediaRenderer;
}


void
QtMediaRendererGroup::show()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer group show: " + getDeviceType());

    QtController* pController = static_cast<QtController*>(getController());

    if (_pWidgetCanvas && pController) {
        pController->addTab(_pWidgetCanvas, shortName().c_str());
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("Qt media renderer group failed to show device group, no widget available: " + getDeviceType());
    }
}


//Omm::Gui::Widget*
//QtMediaRendererGroup::getDeviceGroupWidget()
//{
////    return _pWidgetList;
//    return _pWidgetCanvas;
//}


Omm::Gui::ListWidget*
QtMediaRendererGroup::createWidget()
{
    return new QtMediaRendererWidget;
}


Omm::Gui::ListWidget*
QtMediaRendererGroup::getChildWidget(int row)
{
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    return pRenderer->getDeviceWidget();
}


void
QtMediaRendererGroup::attachWidget(int row, Omm::Gui::ListWidget* pWidget)
{
//    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget");
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget row: " + Poco::NumberFormatter::format(row) + ", name: " + pRenderer->getFriendlyName());
    QtMediaRendererWidget* pRendererWidget = static_cast<QtMediaRendererWidget*>(pWidget);
    pRendererWidget->_pRenderer = pRenderer;
    pRenderer->setDeviceWidget(pRendererWidget);

    connect(pRendererWidget, SIGNAL(showWidgetSignal()), pRendererWidget, SLOT(show()));
    connect(pRendererWidget, SIGNAL(hideWidgetSignal()), pRendererWidget, SLOT(hide()));
    connect(pRendererWidget, SIGNAL(configureWidget()), pRendererWidget, SLOT(configure()));
    connect(pRendererWidget, SIGNAL(unconfigureWidget()), pRendererWidget, SLOT(unconfigure()));

    emit pRendererWidget->configureWidget();
    emit pRendererWidget->showWidgetSignal();
    Omm::Av::Log::instance()->upnpav().debug("media renderer group attach widget finished.");
}


void
QtMediaRendererGroup::detachWidget(int row)
{
    QtMediaRenderer* pRenderer = static_cast<QtMediaRenderer*>(getDevice(row));
    Omm::Av::Log::instance()->upnpav().debug("media renderer group detach widget row: " + Poco::NumberFormatter::format(row) + ", name: " + pRenderer->getFriendlyName());
    QtMediaRendererWidget* pRendererWidget = pRenderer->getDeviceWidget();

    emit pRendererWidget->hideWidgetSignal();
    emit pRendererWidget->unconfigureWidget();

    disconnect(pRendererWidget, SIGNAL(showWidgetSignal()), pRendererWidget, SLOT(show()));
    disconnect(pRendererWidget, SIGNAL(hideWidgetSignal()), pRendererWidget, SLOT(hide()));
    disconnect(pRendererWidget, SIGNAL(configureWidget()), pRendererWidget, SLOT(configure()));
    disconnect(pRendererWidget, SIGNAL(unconfigureWidget()), pRendererWidget, SLOT(unconfigure()));

    pRendererWidget->_pRenderer = 0;
    pRenderer->setDeviceWidget(0);
    Omm::Av::Log::instance()->upnpav().debug("media renderer group detach widget finished.");
}


void
QtMediaRendererGroup::playButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->playPressed();
    }
}


void
QtMediaRendererGroup::stopButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->stopPressed();
    }
}


void
QtMediaRendererGroup::volumeSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->volumeChanged(value);
    }
}


void
QtMediaRendererGroup::positionSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->positionMoved(value);
    }
}


void
QtMediaRendererGroup::init()
{
    _pControlPanel = new QtMediaRendererControlPanel;
    static_cast<QtController*>(getController())->addPanel(_pControlPanel);

    connect(_pControlPanel, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(_pControlPanel, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(_pControlPanel, SIGNAL(volumeSliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_pControlPanel, SIGNAL(positionSliderMoved(int)), this, SLOT(positionSliderMoved(int)));
}
