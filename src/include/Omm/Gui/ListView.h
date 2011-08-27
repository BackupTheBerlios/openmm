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
#include "ListItemView.h"

namespace Omm {
namespace Gui {

class ListModel;
class ListItemView;

class ListView : public View
{
    friend class ListViewImpl;
    
public:
    ListView(int viewHeight, bool movableViews = true, View* pParent = 0);

    void setModel(ListModel* pModel);
    void insertItem(int row);
    void removeItem(int row);

private:
    int visibleRows();
    void initView(ListItemView* pView);
    void moveView(int row, ListItemView* pView);
    void resize(int rows);

    void extendViewPool();
    void extendViewPool(int n);

    int viewPoolSize();
    /// The view has a widget pool which is large enough to fill the area of the view
    /// with widgets (created by the model).
    int visibleIndex(int row);
    int countVisibleViews();
    ListItemView* visibleView(int index);
    bool itemIsVisible(int row);
    void moveViewToRow(int row, ListItemView* pView);
    void selectNotificationHandler(ListItemView::RowSelectNotification* pSelectNotification);

    ListModel*                          _pModel;
//    bool                                _lazy;
    std::vector<ListItemView*>          _viewPool;
    std::vector<ListItemView*>          _visibleViews;
    std::stack<ListItemView*>           _freeViews;
    int                                 _rowOffset;
    int                                 _viewHeight;
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
