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
#include <vector>

#include "Gui/View.h"
#include "ViewImpl.h"
#include "AbstractClusterImpl.h"

namespace Omm {
namespace Gui {


class GenericClusterViewImpl : public AbstractClusterViewImpl, public PlainViewImpl
{
    friend class GenericClusterStackedLayout;

public:
    GenericClusterViewImpl(View* pView);

    virtual void init();
    virtual void insertView(View* pView, const std::string& name = "", int index = 0);
    virtual void removeView(View* pView);
    virtual int getViewCount();
    virtual int getCurrentViewIndex(); /// current view has focus
    virtual void setCurrentViewIndex(int index);
    virtual int getIndexFromView(View* pView);
    virtual View* getViewFromIndex(int index);

    virtual void setHandlesHidden(bool hidden = true);
    virtual const int getHandleHeight();

    virtual void sizeConstraintReached(View::SizeConstraint& width, View::SizeConstraint& height);
    virtual std::string writeLayout();

private:
    int                                 _handleHeight;
    int                                 _handleWidth;
    bool                                _handleBarHidden;
    std::map<View*, ListItemView*>      _handles;
    std::vector<View*>                  _views;
    int                                 _currentViewIndex;
};


}  // namespace Omm
}  // namespace Gui

#endif
