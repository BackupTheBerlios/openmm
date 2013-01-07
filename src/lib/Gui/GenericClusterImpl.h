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

#ifndef GenericClusterImpl_INCLUDED
#define GenericClusterImpl_INCLUDED

#include <map>
#include <set>
#include <vector>

#include "Gui/View.h"
#include "ViewImpl.h"
#include "AbstractClusterImpl.h"
#include "SelectionView.h"

namespace Omm {
namespace Gui {


class GenericClusterViewImpl : public AbstractClusterViewImpl, public PlainViewImpl
{
    friend class GenericClusterStackedLayout;
    friend class GenericClusterController;

public:
    GenericClusterViewImpl(View* pView);

    virtual void init();
    virtual void insertView(View* pView, const std::string& label = "", int index = 0);
    virtual void removeView(View* pView);
    virtual std::string getConfiguration();
    virtual void setConfiguration(const std::string& configuration);

    virtual int getViewCount();
    virtual int getCurrentViewIndex(); /// current view has focus
    virtual void setCurrentViewIndex(int index);
    virtual int getIndexFromView(View* pView);
    virtual View* getViewFromIndex(int index);

    virtual void setHandlesHidden(bool hidden = true);
    virtual const int getHandleHeight();

private:
    typedef std::map<std::string, View*>::iterator ViewIterator;
//    typedef LabelView HandleView;
    typedef ListItemView HandleView;

    void changedConfiguration();

    std::map<std::string, View*>        _views;
    /// all views
    std::vector<View*>                  _visibleViews;
    /// visible views in the indexed order
    std::set<View*>                     _hiddenViews;
    /// hidden views (all views minus visible views)

    int                                 _currentViewIndex;
    int                                 _handleHeight;
    int                                 _handleWidth;
    bool                                _handleBarHidden;
    std::map<View*, HandleView*>        _handles;
    SelectionView*                      _pSelection;
};


}  // namespace Omm
}  // namespace Gui

#endif
