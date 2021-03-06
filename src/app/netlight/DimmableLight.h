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

/***************************************************************************|
|                               WARNING                                     |
|        This file is generated by the omm stub generator ommgen.           |
|       Don't edit, it will be overriden at the next run of ommgen.         |
****************************************************************************/

#ifndef DIMMABLELIGHT_H
#define DIMMABLELIGHT_H

#include <Omm/Upnp.h>
#include <Omm/UpnpInternal.h>

using Omm::DevDeviceCode;
using Omm::Service;
using Omm::Action;

class DimmableLight;

class Dimming
{
    friend class DimmableLight;

protected:
    virtual void SetLoadLevelTarget(const Omm::ui1& newLoadlevelTarget) = 0;
    virtual void GetLoadLevelTarget(Omm::ui1& retLoadlevelTarget) = 0;
    virtual void GetLoadLevelStatus(Omm::ui1& retLoadlevelStatus) = 0;

    virtual void initStateVars() = 0;

    void _setLoadLevelTarget(const Omm::ui1& val);
    Omm::ui1 _getLoadLevelTarget();
    void _setLoadLevelStatus(const Omm::ui1& val);
    Omm::ui1 _getLoadLevelStatus();

private:
    static std::string  _description;
    Service* _pService;
};


class SwitchPower
{
    friend class DimmableLight;

protected:
    virtual void SetTarget(const bool& NewTargetValue) = 0;
    virtual void GetTarget(bool& RetTargetValue) = 0;
    virtual void GetStatus(bool& ResultStatus) = 0;

    virtual void initStateVars() = 0;

    void _setTarget(const bool& val);
    bool _getTarget();
    void _setStatus(const bool& val);
    bool _getStatus();

private:
    static std::string  _description;
    Service* _pService;
};


class DimmableLight : public DevDeviceCode
{
public:
    DimmableLight(SwitchPower* pSwitchPowerImpl, Dimming* pDimmingImpl);


private:
    virtual void actionHandler(Action* action);
    virtual void initStateVars(Service* pThis);

    static std::string _deviceDescription;
    SwitchPower* _pSwitchPowerImpl;
    Dimming* _pDimmingImpl;
};

#endif

