/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#ifndef Splitter_INCLUDED
#define Splitter_INCLUDED

#include <vector>

#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {

class AbstractSplitterViewImpl;

class SplitterView : public View
{
public:
    SplitterView(View* pParent = 0, Orientation orientation = Horizontal);

    void setOrientation(Orientation orientation);
    void insertView(View* pView, int index = 0);
    std::vector<float> getSizes();
    void setSizes(const std::vector<float>& sizes);
    void setSize(int index, float size);

    AbstractSplitterViewImpl*    _pSplitterImpl;
};


class Splitter : public Widget<SplitterView, Controller, Model>
{
public:
    Splitter(View* pParent = 0) : Widget<SplitterView, Controller, Model>(pParent) {}
};



}  // namespace Omm
}  // namespace Gui

#endif
