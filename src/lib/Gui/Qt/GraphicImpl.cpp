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

#include <QtGui>

#include <Poco/NumberFormatter.h>

#include "GraphicImpl.h"
#include "Gui/Graphic.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


PathImpl::PathImpl()
{
    _pNativePath = new QPainterPath;
}


PathImpl::~PathImpl()
{
}


GraphicImpl::GraphicImpl(void* pNativeGraphic)
{
    QPainter* pPainter = static_cast<QPainter*>(pNativeGraphic);
    if (!pNativeGraphic) {
        pPainter = new QPainter;
    }
    pPainter->setRenderHint(QPainter::Antialiasing);
    _pNativeGraphic = pPainter;
}


GraphicImpl::~GraphicImpl()
{
}


}  // namespace Omm
}  // namespace Gui
