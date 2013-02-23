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

    virtual void insertView(View* pView, const std::string& label = "", int index = 0) = 0;
//    virtual void updateWithVisibleViews() = 0;                                              // implementation specific, should replace insertView()
    virtual void removeView(View* pView) = 0;                                               // implementation specific, also replaced by updateWithVisibleViews()?
    virtual void showViewAtIndex(View* pView, int index) = 0;                               // implementation specific, also replaced by updateWithVisibleViews()?
    virtual void hideView(View* pView) = 0;                                                 // implementation specific, also replaced by updateWithVisibleViews()?

    virtual std::string getConfiguration() = 0;
    virtual void setConfiguration(const std::string& configuration) = 0;

    virtual int getViewCount() = 0;
    virtual int getCurrentViewIndex() = 0;
    virtual void setCurrentViewIndex(int index) = 0;
    virtual int getIndexFromView(View* pView) = 0;
    virtual View* getViewFromIndex(int index) = 0;
//    virtual void setCurrentView(View* pView) = 0;                                           // implementation specific, should replace getViewFromIndex()

    virtual void setHandlesHidden(bool hidden = true) = 0;                                  // implementation specific
    virtual const int getHandleHeight() = 0;                                                // implementation specific

protected:
    std::map<std::string, View*>        _views;
    /// all views, key is view name (not label)
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

