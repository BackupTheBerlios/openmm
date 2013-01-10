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

#ifndef ModelImpl_INCLUDED
#define ModelImpl_INCLUDED

#include "Gui/Model.h"

namespace Omm {
namespace Gui {


class ModelImpl
{
    friend class Model;

public:
    ModelImpl(Model* pModel);
    virtual ~ModelImpl();

    Model* getModel();
    void* getNativeModel();

protected:
    Model*                       _pModel;
    void*                        _pNativeModel;
};


}  // namespace Omm
}  // namespace Gui

#endif

