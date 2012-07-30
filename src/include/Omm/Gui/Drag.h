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

#ifndef Drag_INCLUDED
#define Drag_INCLUDED

#include "View.h"
#include "Model.h"

namespace Omm {
namespace Gui {

class DragImpl;


class Drag
{
    friend class ViewImpl;

public:
    Drag(View* pSource, Model* pModel);

    void start();
    void* getNativeDrag() const;
    View* getSource();
    Model* getModel();

private:
    DragImpl*  _pImpl;
    View*      _pSource;
    Model*     _pModel;
};


}  // namespace Omm
}  // namespace Gui

#endif
