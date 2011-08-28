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

#ifndef ListViewImpl_INCLUDED
#define ListViewImpl_INCLUDED

#include <QtGui>
#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;
class ListView;
class ListItemView;


class ListViewImpl : public QGraphicsView, public ViewImpl
{
    Q_OBJECT

    friend class ListView;

signals:
    void moveWidgetSignal(int targetRow, ListItemView* pView);
    void extendPoolSignal();

private slots:
    void moveWidgetSlot(int targetRow, ListItemView* pView);
    void extendPoolSlot();

private:
    ListViewImpl(View* pView, bool movableViews = false, View* pParent = 0);
    virtual ~ListViewImpl();

    int visibleRows();
    void initView(ListItemView* pView);
    void moveView(int row, ListItemView* pView);
    void extendViewPool();

    QGraphicsScene*                                   _pGraphicsScene;
    std::map<ListItemView*, QGraphicsProxyWidget*>    _proxyWidgets;
    bool                                              _movableViews;
};


//class LazyListViewImpl : public QScrollArea, public WidgetImpl
//{
//    Q_OBJECT
//
//public:
//    LazyListViewImpl(Widget* pView, Widget* pParent = 0);
//    virtual ~LazyListViewImpl();
//
//protected:
//    virtual int visibleRows();
//    virtual void initWidget(ListWidget* pView);
//    virtual void moveWidget(int row, ListWidget* pView);
//
//    virtual void updateScrollWidgetSize();
//    virtual int getOffset();
//
//signals:
//    void moveWidgetSignal(int targetRow, ListWidget* pView);
//
//private slots:
//    void moveWidgetSlot(int targetRow, ListWidget* pView);
//    void viewScrolledSlot(int value);
//
//private:
//    virtual void resizeEvent(QResizeEvent* event);
//
//    QWidget*                 _pScrollWidget;
//};


}  // namespace Omm
}  // namespace Gui

#endif

