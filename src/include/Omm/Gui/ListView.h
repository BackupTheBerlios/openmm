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

#include "Widget.h"
#include "ListWidget.h"

namespace Omm {
namespace Gui {

class ListModel;
class ListWidget;

class ListView : public Widget
{
    friend class ListViewImpl;
    
public:
    ListView(int widgetHeight, Widget* pParent = 0);

    void setModel(ListModel* pModel);
    void insertItem(int row);
    void removeItem(int row);

private:
    int visibleRows();
    void initWidget(ListWidget* pWidget);
    void moveWidget(int row, ListWidget* pWidget);
    void resize(int rows);

    void extendWidgetPool();
    void extendWidgetPool(int n);

    int widgetPoolSize();
    /// The view has a widget pool which is large enough to fill the area of the view
    /// with widgets (created by the model).
    int visibleIndex(int row);
    int countVisibleWidgets();
    ListWidget* visibleWidget(int index);
    bool itemIsVisible(int row);
    void moveWidgetToRow(int row, ListWidget* pWidget);
    void selectNotificationHandler(ListWidget::RowSelectNotification* pSelectNotification);

    ListModel*                          _pModel;
//    bool                                _lazy;
    std::vector<ListWidget*>            _widgetPool;
    std::vector<ListWidget*>            _visibleWidgets;
    std::stack<ListWidget*>             _freeWidgets;
    int                                 _rowOffset;
    int                                 _widgetHeight;
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
