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

#include "Gui/Drag.h"
#include "Gui/GuiLogger.h"
#include "DragImpl.h"


namespace Omm {
namespace Gui {


Drag::Drag(View* pSource, Model* pModel) :
_pImpl(new DragImpl(pSource, this)),
_pSource(pSource),
_pModel(pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("drag ctor.");

}


void
Drag::start()
{
    _pImpl->start();
}


//void*
//Drag::getNativeDrag() const
//{
//    return _pImpl->getNativeDrag();
//}


View*
Drag::getSource()
{
    return _pSource;
}


Model*
Drag::getModel()
{
    return _pModel;
}


} // namespace Gui
} // namespace Omm
