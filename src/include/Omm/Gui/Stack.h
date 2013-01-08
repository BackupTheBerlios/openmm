/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2013                                                       |
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

#ifndef Stack_INCLUDED
#define Stack_INCLUDED

#include <set>

#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class StackController : public Controller
{
    friend class StackView;

protected:
    virtual void shownView(View* pView) {}
};


class StackView : public View
{
    friend class StackLayout;
    friend class PrivateStackController;

public:
    StackView(View* pParent = 0);

    void insertView(View* pView);
    void setCurrentView(View* pView);

private:
    virtual void syncViewImpl();

    std::set<View*>             _views;
    View*                       _pCurrentView;
    View*                       _pLastView;
};


class Stack : public Widget<StackView, StackController, Model>
{
public:
    Stack(View* pParent = 0) : Widget<StackView, StackController, Model>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
