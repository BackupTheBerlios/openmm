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

#ifndef List_INCLUDED
#define List_INCLUDED

#include <vector>
#include <stack>
#include <map>

#include "View.h"
#include "ScrollArea.h"
#include "ListModel.h"
#include "ListItem.h"

namespace Omm {
namespace Gui {

class ListItemController;


class ListController : public Controller
{
    friend class ListView;
    
protected:
    virtual void selectedItem(int row) {}
};


class ListView : public ScrollAreaView
{
    friend class ListViewImpl;
    friend class ListModel;
    friend class ListItemController;
    friend class ListScrollAreaController;
    
public:
    ListView(View* pParent = 0);

    virtual void setModel(ListModel* pModel);
    int getItemViewHeight();
    void setItemViewHeight(int height);

protected:
    // TODO implement syncView()
    virtual void syncView() {}

    virtual void insertItem(int row);
    virtual void removeItem(int row);
    virtual int visibleRows();
    virtual void addItemView(View* pView);
    virtual void moveItemView(int row, View* pView);

    int getOffset();
    void updateScrollWidgetSize();
    void scrollDelta(int rowDelta);
    virtual void scrolledToRow(int rowOffset);
    virtual void resize(int width, int height);
    void resizeDelta(int rowDelta, int width);

    void extendViewPool(int n);

    int visibleIndex(int row);
    int countVisibleViews();
    View* visibleView(int index);
    bool itemIsVisible(int row);
    void moveViewToRow(int row, View* pView);
    void handleSelectionHighlight();

    std::vector<View*>                  _viewPool;
    /// The view has a view pool which is large enough to fill the area of the view port
    std::vector<View*>                  _visibleViews;
    std::stack<View*>                   _freeViews;
    int                                 _rowOffset;
    int                                 _lastVisibleRows;
    int                                 _itemViewHeight;
    View*                               _pSelectedView;
    int                                 _selectedRow;

private:
    void selectedItem(int row);

    std::map<View*, ListItemController*>    _itemControllers;
};


class List : public Widget<ListView, ListController, ListModel>
{
public:
    List(View* pParent = 0) : Widget<ListView, ListController, ListModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
