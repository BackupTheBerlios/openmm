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

#ifndef MovableList_INCLUDED
#define MovableList_INCLUDED

#include <vector>
#include <stack>

#include "View.h"
#include "ListModel.h"
#include "ListItem.h"

namespace Omm {
namespace Gui {


class MovableListController : public Controller
{
protected:
    virtual void selectedItem(int row) {}
};


class MovableListView : public View
{
    friend class MovableListViewImpl;
    friend class MovableListModel;
    
public:
    MovableListView(View* pParent = 0);

    virtual void setModel(MovableListModel* pModel);

protected:
    MovableListView(ViewImpl* pViewImpl, View* pParent = 0);

    // TODO implement syncView()
    virtual void syncViewImpl() {}

    virtual void insertItem(int row);
    virtual void removeItem(int row);
    virtual int visibleRows();
    virtual void addItemView(View* pView);
    virtual void moveItemView(int row, View* pView);

    void extendViewPool();
    void extendViewPool(int n);

    int viewPoolSize();
    /// The view has a view pool which is large enough to fill the area of the view port
    int visibleIndex(int row);
    int countVisibleViews();
    View* visibleView(int index);
    bool itemIsVisible(int row);
    void moveViewToRow(int row, View* pView);

    std::vector<View*>                  _viewPool;
    std::vector<View*>                  _visibleViews;
    std::stack<View*>                   _freeViews;
    int                                 _rowOffset;
    int                                 _viewHeight;
};


class MovableList : public Widget<MovableListView, MovableListController, MovableListModel>
{
public:
    MovableList(View* pParent = 0) : Widget<MovableListView, MovableListController, MovableListModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
