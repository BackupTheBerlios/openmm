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

#ifndef GraphicImpl_INCLUDED
#define GraphicImpl_INCLUDED


namespace Omm {
namespace Gui {

class View;


class PathImpl
{
    friend class Path;

    PathImpl();
    ~PathImpl();

    void*   _pNativePath;
};


class GraphicImpl
{
    friend class Graphic;

    GraphicImpl(void* pNativeGraphic = 0);
    ~GraphicImpl();

    void*   _pNativeGraphic;
};


}  // namespace Omm
}  // namespace Gui

#endif

