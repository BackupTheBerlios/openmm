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

#include <QtGui>
#include <Poco/NumberFormatter.h>

#include "DragImpl.h"
#include "QtDragImpl.h"
#include "Gui/Drag.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"


namespace Omm {
namespace Gui {


DragImpl::DragImpl(View* pSource, Drag* pDrag) :
_pDrag(pDrag)
{
//    LOG(gui, debug, "Drag impl ctor");
    _pQtDrag = new QDrag(static_cast<QWidget*>(pSource->getNativeView()));
    _pQtDrag->setMimeData(new QtMimeData(this));
}


void
DragImpl::start()
{
    _pQtDrag->exec();
}


Drag*
DragImpl::getDrag() const
{
    return _pDrag;
}


QDrag*
DragImpl::getNativeDrag() const
{
    return _pQtDrag;
}


}  // namespace Omm
}  // namespace Gui
