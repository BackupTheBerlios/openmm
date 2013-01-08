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

#ifndef AbstractClusterImpl_INCLUDED
#define AbstractClusterImpl_INCLUDED

#include <map>
#include <vector>
#include <set>

//#include "ViewImpl.h"

namespace Omm {
namespace Gui {


class AbstractClusterViewImpl
{
public:
    AbstractClusterViewImpl() : _currentViewIndex(-1) {}

    virtual void insertView(View* pView, const std::string& name = "", int index = 0) = 0;
    virtual void removeView(View* pView) = 0;
    virtual std::string getConfiguration() = 0;
    virtual void setConfiguration(const std::string& configuration) = 0;

    virtual int getViewCount() = 0;
    virtual int getCurrentViewIndex() = 0; /// current view has focus
    virtual void setCurrentViewIndex(int index) = 0;
    virtual int getIndexFromView(View* pView) = 0;
    virtual View* getViewFromIndex(int index) = 0;

    virtual void setHandlesHidden(bool hidden = true) = 0;
    virtual const int getHandleHeight() = 0;

protected:
    std::map<std::string, View*>        _views;
    /// all views
    std::vector<View*>                  _visibleViews;
    /// visible views in the indexed order
    std::set<View*>                     _hiddenViews;
    /// hidden views (all views minus visible views)
    int                                 _currentViewIndex;
    /// view that is visible
};


}  // namespace Omm
}  // namespace Gui

#endif

