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

#ifndef AbstractSplitterImpl_INCLUDED
#define AbstractSplitterImpl_INCLUDED


namespace Omm {
namespace Gui {


class AbstractSplitterViewImpl
{
public:
    virtual void setOrientation(View::Orientation orientation) = 0;
    virtual void insertView(View* pView, int index) = 0;
    virtual std::vector<float> getSizes() = 0;
    virtual void setSizes(const std::vector<float>& sizes) = 0;
    virtual void setSize(int index, float size, bool visibleViewsOnly) = 0;
};


}  // namespace Omm
}  // namespace Gui

#endif

