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
#ifndef USER_INTERFACE_QT_H
#define USER_INTERFACE_QT_H

#include "../Controller.h"

class UserInterfaceQt : public UserInterface
{
public:
    virtual int eventLoop();
    
    virtual void initGui() {}
    virtual void showMainWindow() {}
    
    virtual void deviceAdded(Device* pDevice) {}
    virtual void rendererAdded(MediaRendererController* pRenderer) {}
    virtual void serverAdded(MediaServerController* pRenderer) {}
    
    virtual void deviceRemoved(Device* pDevice) {}
    virtual void rendererRemoved(MediaRendererController* pRenderer) 
    {}
    virtual void serverRemoved(MediaServerController* pRenderer) {}
    
};

#endif
