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

#ifndef DragImpl_INCLUDED
#define DragImpl_INCLUDED

#include "ViewImpl.h"

class QDrag;

namespace Omm {
namespace Gui {

class Drag;

class DragImpl
{
public:
    DragImpl(View* pSource, Drag* pDrag);

    void start();
    Drag* getDrag() const;
    QDrag* getNativeDrag() const;

private:
    Drag*    _pDrag;
    QDrag*   _pQtDrag;
};


}  // namespace Omm
}  // namespace Gui

#endif
