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

#ifndef QtWidgetList_INCLUDED
#define QtWidgetList_INCLUDED

#include <QtGui>

#include "QtWidget.h"
#include "AppGui.h"


class QtWidgetList : public QScrollArea, public WidgetListView
{
    Q_OBJECT

public:
    QtWidgetList(QWidget* pParent = 0);
    virtual ~QtWidgetList();

protected:
    virtual int visibleRows();
    virtual void initWidget(ListWidget* pWidget);
    virtual void moveWidget(int row, ListWidget* pWidget);

    virtual void updateScrollWidgetSize();
    virtual int getOffset();

signals:
    void moveWidgetSignal(int targetRow, ListWidget* pWidget);

private slots:
    void moveWidgetSlot(int targetRow, ListWidget* pWidget);
    void viewScrolledSlot(int value);

private:
    virtual void resizeEvent(QResizeEvent* event);

    QWidget*                 _pScrollWidget;
};


class QtWidgetCanvas : public QGraphicsView, public Widget, public WidgetListView
{
    Q_OBJECT

public:
    QtWidgetCanvas(bool movableWidgets = false, QWidget* pParent = 0);
    virtual ~QtWidgetCanvas();

protected:
    virtual int visibleRows();
    virtual void initWidget(ListWidget* pWidget);
    virtual void moveWidget(int row, ListWidget* pWidget);

    virtual void extendWidgetPool();

signals:
    void moveWidgetSignal(int targetRow, ListWidget* pWidget);
    void extendPoolSignal();

private slots:
    void moveWidgetSlot(int targetRow, ListWidget* pWidget);
    void extendPoolSlot();

private:
    QGraphicsScene*                                             _pGraphicsScene;
    std::map<ListWidget*, QGraphicsProxyWidget*>     _proxyWidgets;
    bool                                                        _movableWidgets;
};


#endif

