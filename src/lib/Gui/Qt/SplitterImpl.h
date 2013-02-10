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

#ifndef SplitterImpl_INCLUDED
#define SplitterImpl_INCLUDED

#include "ViewImpl.h"
#include "../AbstractSplitterImpl.h"

namespace Omm {
namespace Gui {

class View;

class SplitterViewImpl : public AbstractSplitterViewImpl, public ViewImpl
{
    friend class SplitterView;
    friend class ColumnClusterViewImpl;

private:
    SplitterViewImpl(View* pView, View::Orientation orientation);
    ~SplitterViewImpl();

    void setOrientation(View::Orientation orientation);
    void insertView(View* pView, int index);
    std::vector<float> getSizes();
    void setSizes(const std::vector<float>& sizes);
    void setSize(int index, float size, bool visibleViewsOnly) {}
};


}  // namespace Omm
}  // namespace Gui

#endif

