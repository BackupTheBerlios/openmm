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

#include "Gui/GuiLogger.h"
#include "Log.h"
#include "SelectionView.h"
//#include "SelectionViewImpl.h"


namespace Omm {
namespace Gui {



SelectionView::SelectionView() :
//View(0, false)
View(0, true)
{
//    LOG(gui, debug, "selection view ctor.");
    setName("selection view");

    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 20;
    _height = _prefHeight;

    setBackgroundColor(Color("blue"));
//    resize(_prefWidth, 2);

    _pBottom = new View;
//    _pBottom->resize(_prefWidth, 2);
//    _pBottom->move(0, _prefHeight - 2);
    _pBottom->setBackgroundColor(Color("blue"));

//    hide(true);
}


void
SelectionView::setChildView(View* pParent)
{
    if (pParent) {
        pParent->addSubview(this);
        pParent->addSubview(_pBottom);
    }
}


void
SelectionView::raise()
{
    View::raise();
    _pBottom->raise();
}


void
SelectionView::show(bool async)
{
    View::show(async);
    _pBottom->show(async);
}


void
SelectionView::hide(bool async)
{
    View::hide(async);
    _pBottom->hide(async);
}


void
SelectionView::resize(int width, int height)
{
    _height = height;
    View::resize(width, 2);
    _pBottom->resize(width, 2);
    _pBottom->move(posX(), posY() + height - 2);
}


void
SelectionView::move(int x, int y)
{
    View::move(x, y);
    _pBottom->move(x, y + _height - 2);
}


} // namespace Gui
} // namespace Omm
