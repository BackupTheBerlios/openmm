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

#ifndef QtStdController_INCLUDED
#define QtStdController_INCLUDED

#include <QtGui>
#include <Omm/Upnp.h>

class QtStdApplication;
class QtStdDeviceGroup;

class QtStdController : public QTabWidget, public Omm::Controller
{
    Q_OBJECT

public:
    QtStdController(QtStdApplication* pQtApplication);

//    virtual void showDeviceGroup(QtStdDeviceGroup* pDeviceGroup);
    void addPanel(QToolBar* pPanel);

private:
    QtStdApplication*       _pQtApplication;
};

#endif

