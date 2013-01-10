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

#include <Poco/NumberFormatter.h>

#include "Gui/GuiLogger.h"
#include "ModelImpl.h"


namespace Omm {
namespace Gui {


ModelImpl::ModelImpl(Model* pModel) :
_pModel(pModel),
_pNativeModel(0)
{

}


ModelImpl::~ModelImpl()
{
}


Model*
ModelImpl::getModel()
{
    return _pModel;
}


void*
ModelImpl::getNativeModel()
{
    return _pNativeModel;
}

}  // namespace Omm
}  // namespace Gui
