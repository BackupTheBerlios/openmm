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

#define DoNotUseSelectionViewImpl

#include "Gui/GuiLogger.h"
#include "Log.h"
#include "Gui/Label.h"
#include "SelectionView.h"
#ifndef DoNotUseSelectionViewImpl
#include "SelectionViewImpl.h"
#endif


namespace Omm {
namespace Gui {


SelectionView::SelectionView() :
#ifdef DoNotUseSelectionViewImpl
View(0, true)
#else
View(0, false)
#endif
{
//    LOG(gui, debug, "selection view ctor.");
    setName("selection view");

    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 20;
    _lineWidth = 3;
    _height = _prefHeight;
    Color selectionColor(114, 4, 4, 191);

#ifdef DoNotUseSelectionViewImpl
    setBackgroundColor(selectionColor);
//    resize(_prefWidth, _lineWidth);

    _pBottom = new View;
    _pLeft = new View;
    _pRight = new View;
//    _pBottom->resize(_prefWidth, _lineWidth);
//    _pBottom->move(0, _prefHeight - _lineWidth);
//    _pBottom->setBackgroundColor(Color("grey"));
//    _pLeft->setBackgroundColor(Color("grey"));
//    _pRight->setBackgroundColor(Color("grey"));
    _pBottom->setBackgroundColor(selectionColor);
    _pLeft->setBackgroundColor(selectionColor);
    _pRight->setBackgroundColor(selectionColor);

    hide(true);
#else
    _pImpl = new SelectionViewImpl(this);
#endif
}


void
SelectionView::setParentView(View* pParent)
{
    if (pParent) {
        setParent(pParent);
#ifdef DoNotUseSelectionViewImpl
        _pBottom->setParent(pParent);
        _pLeft->setParent(pParent);
        _pRight->setParent(pParent);
#endif
    }
}


void
SelectionView::raise(bool async)
{
    View::raise(async);
#ifdef DoNotUseSelectionViewImpl
    _pBottom->raise(async);
    _pLeft->raise(async);
    _pRight->raise(async);
#endif
}


void
SelectionView::show(bool async)
{
    View::show(async);
#ifdef DoNotUseSelectionViewImpl
    _pBottom->show(async);
    _pLeft->show(async);
    _pRight->show(async);
#endif
}


void
SelectionView::hide(bool async)
{
    View::hide(async);
#ifdef DoNotUseSelectionViewImpl
    _pBottom->hide(async);
    _pLeft->hide(async);
    _pRight->hide(async);
#endif
}


void
SelectionView::resize(int width, int height)
{
    _height = height;
#ifdef DoNotUseSelectionViewImpl
    View::resize(width, _lineWidth);
    _pBottom->resize(width, _lineWidth);
    _pBottom->move(posX(), posY() + height - _lineWidth);
    _pLeft->resize(_lineWidth, height - 2 * _lineWidth);
    _pLeft->move(posX(), posY() + _lineWidth);
    _pRight->resize(_lineWidth, height - 2 * _lineWidth);
    _pRight->move(posX() + width - _lineWidth, posY() + _lineWidth);
#else
    View::resize(width, height);
#endif
}


void
SelectionView::move(int x, int y)
{
    View::move(x, y);
#ifdef DoNotUseSelectionViewImpl
    _pBottom->move(x, y + _height - _lineWidth);
    _pLeft->move(x, y + _lineWidth);
    _pRight->move(x + width() - _lineWidth, y + _lineWidth);
#endif
}


} // namespace Gui
} // namespace Omm
