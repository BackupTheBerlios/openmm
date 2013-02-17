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

#ifndef Switch_INCLUDED
#define Switch_INCLUDED

#include <string>

#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class SwitchController : public Controller
{
    friend class SwitchViewImpl;
    friend class SwitchSignalProxy;

protected:
    virtual void switched(bool on) {}
};


class SwitchView : public View
{
    friend class SwitchModel;

public:
    SwitchView(View* pParent = 0);

    bool getStateOn();
    void setState(bool on);
};


class Switch : public Widget<SwitchView, SwitchController, Model>
{
public:
    Switch(View* pParent = 0) : Widget<SwitchView, SwitchController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
