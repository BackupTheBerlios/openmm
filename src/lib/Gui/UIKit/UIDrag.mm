/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2012                                                       |
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
#include "Gui/Color.h"
#include "UIDrag.h"


namespace Omm {
namespace Gui {

UIDrag* UIDrag::_pInstance = 0;

UIDrag*
UIDrag::instance()
{
    if (!_pInstance) {
        _pInstance = new UIDrag;
    }
    return _pInstance;
}


Drag*
UIDrag::getDrag()
{
    return _pDrag;
}


void
UIDrag::setDrag(Drag* pDrag)
{
    _pDrag = pDrag;
}


View*
UIDrag::getMainView()
{
    return _pMainView;
}


void
UIDrag::setMainView(View* pView)
{
    _pMainView = pView;
    _pPointerView = new View(_pMainView);
    _pPointerView->hide();
    _pPointerView->resize(20, 20);
    _pPointerView->setBackgroundColor(Color("blue"));
}


View*
UIDrag::getDropView()
{
    return _pDropView;
}


void
UIDrag::setDropView(View* pView)
{
    _pDropView = pView;
}


View*
UIDrag::getPointerView()
{
    return _pPointerView;
}


void
UIDrag::setPointerView(View* pView)
{
    [static_cast<UIView*>(_pPointerView->getNativeView()) removeFromSuperview];
    _pPointerView = pView;
    _pMainView->addSubview(_pPointerView);
    _pPointerView->hide();
}


UIDrag::UIDrag() :
_pDrag(0),
_pDropView(0),
_pPointerView(0)
{
}


}  // namespace Omm
}  // namespace Gui
