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
#include "Gui/Stack.h"
#include "ViewImpl.h"
#include "AbstractClusterImpl.h"
#include "SelectionView.h"

namespace Omm {
namespace Gui {

class HandleBarView;


class GenericClusterViewImpl : public AbstractClusterViewImpl, public PlainViewImpl
{
    friend class HandleBarView;
    friend class HandleBarController;
    friend class HandleBarLayout;

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
    typedef ListItemView HandleViewType;

    void changedConfiguration();
    void setCurrentView(View* pView);

    HandleBarView*                      _pHandleBarView;
    StackView*                          _pStackView;
};


}  // namespace Omm
}  // namespace Gui

#endif
