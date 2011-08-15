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

#ifndef Navigator_INCLUDED
#define Navigator_INCLUDED

#include <stack>
#include "Widget.h"

namespace Omm {
namespace Gui {

class Navigator;


class Navigable
{
    friend class Navigator;

public:
    Navigable();

    virtual std::string getBrowserTitle() { return ""; }
    virtual Widget* getWidget() { return 0; }
    /// If getWidget() returns not null but a valid widget, the widget
    /// is pushed on QtNavigator::_pStackedWidget.
    virtual void show() {}
    /// Additionally, show() can be implemented if for example no widget
    /// is pushed but some other action is necessary to show the correct view.
    Navigator* getNavigator() const;

private:
    Navigator*    _pNavigator;
};


class Navigator : public Widget
{
public:
    Navigator();
    ~Navigator();

    void push(Navigable* pNavigable);

protected:
    virtual void pushImpl(Navigable* pNavigable) = 0;

private:
    std::stack<Navigable*>    _navigableStack;
};


}  // namespace Omm
}  // namespace Gui

#endif
