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

protected:
    ListView(ViewImpl* pViewImpl, View* pParent = 0);

    // TODO implement syncView()
    virtual void syncView() {}

    virtual void insertItem(int row);
    virtual void removeItem(int row);
    virtual int visibleRows();
    virtual void addItemView(View* pView);
    virtual void moveItemView(int row, View* pView);

    int getOffset();
    void updateScrollWidgetSize();
    void scrolledToRow(int rowOffset);
    void resize(int rows);

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


class List : public Widget<ListView, ListController, ListModel>
{
public:
    List(View* pParent = 0) : Widget<ListView, ListController, ListModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
