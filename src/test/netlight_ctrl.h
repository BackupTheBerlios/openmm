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
#ifndef OMMLIGHT_CTRL_H
#define OMMLIGHT_CTRL_H

#include "Upnp.h"

// NOTE: Controller does not support QueryStateVar for unevented StateVars because it is depricated.
//       Only devices should respond to QueryStateVar requests.


class SwitchPowerController
{
    friend class DimmableLightController;
    
public:
        // sync interface
    void GetStatus(bool& ResultStatus);
    
    // async interface request
    void _reqGetStatus();
    
    // getter for evented StateVars
    bool _getStatus();
    
protected:
    // async interface answer
    virtual void _ansGetStatus(const bool& ResultStatus) = 0;
    
    // value of evented StateVar changed
    virtual void _changedStatus(const bool&) = 0;
    
private:
    void _threadGetStatus(Omm::Action* pAction);
    
    Omm::Service* _pService;
};


class DimmableLightController : public Omm::CtlDeviceCode
{
public:
    DimmableLightController(Omm::Device* pDevice, SwitchPowerController* pSwitchPowerController);
    
    SwitchPowerController* SwitchPower() { return _pSwitchPowerController; }
    
private:
    virtual void eventHandler(Omm::StateVar* pStateVar);
    
    Omm::Device* _pDevice;
    SwitchPowerController* _pSwitchPowerController;
};

#endif
