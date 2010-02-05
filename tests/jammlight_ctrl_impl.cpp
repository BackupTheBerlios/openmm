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

#include "jammlight_ctrl_impl.h"

void
SwitchPowerControllerImpl::_ansGetStatus(const bool& ResultStatus)
{
    // begin of your own code
    std::cout << "async request answer _ansGetStatus() returns: " << ResultStatus << std::endl;
    // end of your own code
}


void
SwitchPowerControllerImpl::_changedStatus(const bool& Status)
{
    // begin of your own code
    std::cout << " value of evented state variable Status changed to: " << Status << std::endl;
    // end of your own code
}
