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
class SelectionView;


class ListController : public Controller
{
    friend class ListView;

protected:
    virtual void selectedItem(int row) {}
    virtual void activatedItem(int row) {}
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
    enum SelectionType { Frame, Filled };

    ListView(View* pParent = 0, SelectionType selectionType = Filled);

    int getItemViewHeight();
    void setItemViewWidth(int width);
    void setItemViewHeight(int height);
    void selectRow(int row);
    void activateRow(int row);

    void resetListView();
    void addTopView(View* pView);
    void setDragMode(int dragMode);
    void setSelectionType(SelectionType selectionType);

protected:
    // main operations
    virtual void syncViewImpl();
    void showItemsAt(int rowOffset, int itemOffset, int countItems);
    void scrollOneRow(int direction);
    virtual void scrollToRowOffset(int rowOffset);
    virtual void scale(float factor);
    void onResize(int width, int height);

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

//    View*                               _pHighlightedView;
    SelectionView*                      _pSelectionView;
    Model*                              _pSelectedModel;
    int                                 _highlightedRow;
    View*                               _pDropLine;

    View*                               _pTopView;
    int                                 _dragMode;
    SelectionType                       _selectionType;

private:
    // item selection
    void selectedItem(int row, bool async);
    void activatedItem(int row, bool async);
    void highlightItem(int row, bool async);
    void selectHighlightedItem();
    void activateHighlightedItem();

    // drag'n drop support
    void dragView(View* pView);
    void shiftViews(View* pFirstView, int pixel = 10);
    void showDropLine(View* pView);
    void hideDropLine();
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
