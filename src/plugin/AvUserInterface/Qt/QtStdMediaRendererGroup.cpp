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

#include "QtStdMediaRendererGroup.h"
#include "QtStdController.h"
#include "QtNavigator.h"
#include "QtMediaRenderer.h"
#include "QtMediaRendererControlPanel.h"


QtStdMediaRendererGroup::QtStdMediaRendererGroup() :
QtStandardDeviceGroup(new Omm::Av::MediaRendererGroupDelegate)
{
}


Omm::Device*
QtStdMediaRendererGroup::createDevice()
{
    return new QtMediaRenderer;
}


void
QtStdMediaRendererGroup::selectDevice(Omm::Device* pDevice, int index)
{
//    _pNavigator->push(static_cast<QtMediaRenderer*>(pDevice));
}


void
QtStdMediaRendererGroup::playButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->playPressed();
    }
}


void
QtStdMediaRendererGroup::stopButtonPressed()
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->stopPressed();
    }
}


void
QtStdMediaRendererGroup::volumeSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->volumeChanged(value);
    }
}


void
QtStdMediaRendererGroup::positionSliderMoved(int value)
{
    Omm::Av::CtlMediaRenderer* pRenderer = static_cast<Omm::Av::CtlMediaRenderer*>(getSelectedDevice());
    if (pRenderer) {
        pRenderer->positionMoved(value);
    }
}


void
QtStdMediaRendererGroup::init()
{
    _pControlPanel = new QtMediaRendererControlPanel;
    static_cast<QtStdController*>(getController())->addPanel(_pControlPanel);

    connect(_pControlPanel, SIGNAL(playButtonPressed()), this, SLOT(playButtonPressed()));
    connect(_pControlPanel, SIGNAL(stopButtonPressed()), this, SLOT(stopButtonPressed()));
    connect(_pControlPanel, SIGNAL(volumeSliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_pControlPanel, SIGNAL(positionSliderMoved(int)), this, SLOT(positionSliderMoved(int)));
}
