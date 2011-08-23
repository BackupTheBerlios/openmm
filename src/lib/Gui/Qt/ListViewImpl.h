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
#include "WidgetImpl.h"

namespace Omm {
namespace Gui {

class Widget;
class ListView;
class ListWidget;


class ListViewImpl : public QGraphicsView, public WidgetImpl
{
    Q_OBJECT

    friend class ListView;

signals:
    void moveWidgetSignal(int targetRow, ListWidget* pWidget);
    void extendPoolSignal();

private slots:
    void moveWidgetSlot(int targetRow, ListWidget* pWidget);
    void extendPoolSlot();

private:
    ListViewImpl(Widget* pWidget, bool movableWidgets = false, Widget* pParent = 0);
    virtual ~ListViewImpl();

    int visibleRows();
    void initWidget(ListWidget* pWidget);
    void moveWidget(int row, ListWidget* pWidget);
    void extendWidgetPool();

    QGraphicsScene*                                   _pGraphicsScene;
    std::map<ListWidget*, QGraphicsProxyWidget*>      _proxyWidgets;
    bool                                              _movableWidgets;
};


//class LazyListViewImpl : public QScrollArea, public WidgetImpl
//{
//    Q_OBJECT
//
//public:
//    LazyListViewImpl(Widget* pWidget, Widget* pParent = 0);
//    virtual ~LazyListViewImpl();
//
//protected:
//    virtual int visibleRows();
//    virtual void initWidget(ListWidget* pWidget);
//    virtual void moveWidget(int row, ListWidget* pWidget);
//
//    virtual void updateScrollWidgetSize();
//    virtual int getOffset();
//
//signals:
//    void moveWidgetSignal(int targetRow, ListWidget* pWidget);
//
//private slots:
//    void moveWidgetSlot(int targetRow, ListWidget* pWidget);
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

