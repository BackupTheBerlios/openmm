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

#ifndef GenericSplitterImpl_INCLUDED
#define GenericSplitterImpl_INCLUDED

#include "ViewImpl.h"
#include "AbstractSplitterImpl.h"

namespace Omm {
namespace Gui {

class View;
class SplitterBar;

class GenericSplitterViewImpl : public AbstractSplitterViewImpl, public PlainViewImpl
{
    friend class GenericSplitterLayout;
    friend class SplitterView;
    friend class SplitterBar;
    friend class ColumnClusterViewImpl;

    GenericSplitterViewImpl(View* pView, View::Orientation orientation);
    ~GenericSplitterViewImpl();

    virtual void init();

    void setOrientation(View::Orientation orientation);
    void insertView(View* pView, int index);
    std::vector<float> getSizes();
    void setSizes(const std::vector<float>& sizes);
    void setSize(int index, float size);

    void showBarAt(int index, int pos);

    View::Orientation               _orientation;
    std::vector<float>              _sizes;
    std::vector<View*>              _views;
    std::vector<SplitterBar*>       _bars;
    int                             _barWidth;
};


}  // namespace Omm
}  // namespace Gui

#endif

