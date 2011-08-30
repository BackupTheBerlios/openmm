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

#ifndef ListView_INCLUDED
#define ListView_INCLUDED

#include <vector>
#include <stack>

#include "View.h"
#include "ListModel.h"
#include "ListItem.h"

namespace Omm {
namespace Gui {


class ListController : public Controller
{
protected:
    virtual void selectedItem(int row) {}
};


class ListView : public View
{
    friend class ListViewImpl;
    friend class ListModel;
    
public:
    ListView(View* pParent = 0);

    virtual void setModel(ListModel* pModel);
    
private:
    void insertItem(int row);
    void removeItem(int row);
    int visibleRows();
    void initView(View* pView);
    void moveView(int row, View* pView);
    void resize(int rows);

    void extendViewPool();
    void extendViewPool(int n);

    int viewPoolSize();
    /// The view has a view pool which is large enough to fill the area of the view port
    int visibleIndex(int row);
    int countVisibleViews();
    View* visibleView(int index);
    bool itemIsVisible(int row);
    void moveViewToRow(int row, View* pView);

//    bool                                _lazy;
    std::vector<View*>                  _viewPool;
//    std::vector<ListItemController*>    _controllerPool;
    std::vector<View*>                  _visibleViews;
    std::stack<View*>                   _freeViews;
    int                                 _rowOffset;
    int                                 _viewHeight;
};


class List : public Widget<ListView, ListController, ListModel>
{
public:
    List(View* pParent = 0) : Widget<ListView, ListController, ListModel>(pParent) {}
};


//class LazyListView : public ListView
//{
//public:
//    virtual int getOffset() { return 0; }
//    virtual void updateScrollWidgetSize() {}
//    void scrolledToRow(int rowOffset);
//};

}  // namespace Omm
}  // namespace Gui

#endif
