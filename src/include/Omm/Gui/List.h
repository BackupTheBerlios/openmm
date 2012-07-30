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
    virtual void draggedItem(int row) {}
    virtual void droppedItem(Model* pModel, int row) {}
};


class ListView : public ScrollAreaView
{
    friend class ListViewImpl;
    friend class ListModel;
    friend class ListItemController;
    friend class ListDragController;
    friend class ListScrollAreaController;

public:
    enum DragMode { DragNone = 1, DragSource = 2, DragTarget = 4 };

    ListView(View* pParent = 0);

    int getItemViewHeight();
    void setItemViewWidth(int width);
    void setItemViewHeight(int height);
    void selectRow(int row);

    void resetListView();
    void addTopView(View* pView);
    void setDragMode(int dragMode);

protected:
    // main operations
    virtual void syncViewImpl();
    void showItemsAt(int rowOffset, int itemOffset, int countItems);
    void scrollOneRow(int direction);
    virtual void scrollToRowOffset(int rowOffset);
    virtual void resize(int width, int height);
    virtual void scale(float factor);

    // item view management
    void extendViewPool();
    View* getFreeView();
    void putFreeView(View* pView);
    void clearVisibleViews();

    // helper methods
    virtual void addItemView(View* pView);
    void moveItemView(int row, View* pView);
    int getOffset();
    void updateScrollWidgetSize(int rows);
    int visibleIndex(int row);
    int modelItemCount();
    int totalItemCount();
    int viewPortHeightInRows();
    View* visibleView(int index);
    bool itemIsVisible(int row);
    int lastVisibleRow();
    int rowFromView(View* pView);

    std::vector<View*>                  _viewPool;
    /// The view has a view pool which is large enough to fill the area of the view port
    std::vector<View*>                  _visibleViews;
    std::stack<View*>                   _freeViews;
    int                                 _rowOffset;  // FIXME: int may be not enough for row offset on some platforms
    /// _rowOffset is view port offset in rows
    int                                 _bottomRows;
    int                                 _itemViewHeight;

    View*                               _pHighlightedView;
    int                                 _highlightedRow;

    View*                               _pTopView;
    int                                 _dragMode;

private:
    // item selection
    void selectedItem(int row);
    void highlightItem(int row);
    void selectHighlightedItem();

    // drag'n drop support
    void dragView(View* pView);
    void shiftViews(View* pFirstView, int pixel = 10);
    void dropView(Model* pSourceModel, View* pTarget);

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
