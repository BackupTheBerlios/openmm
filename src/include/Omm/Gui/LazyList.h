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

#ifndef LazyList_INCLUDED
#define LazyList_INCLUDED

#include <vector>
#include <stack>

#include "List.h"
#include "ListModel.h"
#include "ListItem.h"

namespace Omm {
namespace Gui {


class LazyListView : public ListView
{
    friend class LazyListViewImpl;

public:
    LazyListView(View* pParent = 0);

    virtual void setModel(LazyListModel* pModel);

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
    void scrolledToRow(int rowOffset);
};


class LazyList : public Widget<LazyListView, ListController, LazyListModel>
{
public:
    LazyList(View* pParent = 0) : Widget<LazyListView, ListController, LazyListModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
