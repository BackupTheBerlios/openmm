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
#ifndef JAMMLIGHT_CTRL_H
#define JAMMLIGHT_CTRL_H

#include <jamm/upnp.h>
using Jamm::Service;

class SwitchPowerController
{
    friend class JammlightController;
    
public:
        // sync interface
    void GetStatus(bool& ResultStatus);
    
    // async interface request
    void _reqGetStatus();
    
    // getter for StateVars
    // -> query for unevented StateVars
    // -> read cached value for evented StateVars
    bool _getStatus();
    
protected:
    // async interface answer
    virtual void _ansGetStatus(const bool& ResultStatus) = 0;
    
    // value of evented StateVar changed
    virtual void _changedStatus(const bool&) = 0;
    
private:
    Service* m_pService;
};

#endif
