/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
***************************************************************************/
#ifndef JAMMLIGHT_CTRL_IMPL_H
#define JAMMLIGHT_CTRL_IMPL_H

#include "jammlight_ctrl.h"

class SwitchPowerControllerImpl : public SwitchPowerController
{
private:
    // async interface answer
    virtual void _ansGetStatus(const bool& ResultStatus);
    
    // value of evented StateVar changed
    virtual void _changedStatus(const bool&);
};


class JammlightControllerImpl : public Jamm::Controller
{
public:
    JammlightControllerImpl(SwitchPowerControllerImpl* pSwitchPowerControllerImpl);
    
private:
    void deviceAdded(Jamm::DeviceRoot* device);
    void deviceRemoved(Jamm::DeviceRoot* device);
    
    SwitchPowerControllerImpl* m_pSwitchPowerCtrlImpl;
};

#endif
