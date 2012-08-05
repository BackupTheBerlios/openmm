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

#include <Poco/NumberFormatter.h>

#include "DragImpl.h"
#include "Gui/Drag.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"


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


void
UIDrag::setDrag(Drag* pDrag)
{
    _pDrag = pDrag;
}


Drag*
UIDrag::getDrag()
{
    return _pDrag;
}


UIDrag::UIDrag() :
_pDrag(0)
{
}


DragImpl::DragImpl(View* pSource, Drag* pDrag) :
_pDrag(pDrag)
{
//    Omm::Gui::Log::instance()->gui().debug("Drag impl ctor");
//    _pQtDrag = new QDrag(static_cast<QWidget*>(pSource->getNativeView()));
//    _pQtDrag->setMimeData(new QtMimeData(this));
}


void
DragImpl::start()
{
    UIDrag::instance()->setDrag(_pDrag);
//    _pQtDrag->exec();
}


Drag*
DragImpl::getDrag() const
{
    return _pDrag;
}


//QDrag*
//DragImpl::getNativeDrag() const
//{
//    return _pQtDrag;
//}


}  // namespace Omm
}  // namespace Gui
