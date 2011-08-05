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

#include <stack>
#include <QtGui>

class WidgetListModel;
class WidgetListView;


class Widget
{
public:
    Widget();

    virtual void showWidget() {}
    virtual void hideWidget() {}

    int getRow();
    void setRow(int row);

private:
    int _row;
};


class WidgetFactory
{
public:
    virtual Widget* createWidget() { return 0; }
};


class WidgetListModel
{
    friend class WidgetListView;
    
public:
    WidgetListModel();

    // item related
    virtual int totalItemCount() { return 0; }
    void insertItem(int row);
    /// Ask the view to show an item at row. No data is created (cached), as
    /// the whole data of the model is already present in the underlying implementation.
    /// The model is only an abstraction layer on top of the data.
    void removeItem(int row);
    /// See insertItem().
    virtual void selectItem(int row) {}
    /// View should call selectItem() when the item in row is selected.

    // lazy model related
    virtual bool canFetchMore() { return false; }
    virtual void fetchMore(bool forward = true) {}
    virtual int lastFetched(bool forward = true) { return totalItemCount(); }

    // widget related
    void setWidgetFactory(WidgetFactory* pWidgetFactory);
    virtual Widget* createWidget();
    virtual Widget* getWidget(int row) { return 0; }
    virtual void attachWidget(int row, Widget* pWidget) {}
    virtual void detachWidget(int row) {}

private:
    WidgetListView*                 _pView;
    WidgetFactory*                  _pWidgetFactory;
};


class WidgetListView
{
public:
    WidgetListView(int widgetHeight, bool lazy = false);
    
    void setModel(WidgetListModel* pModel);
    void insertItem(int row);
    void removeItem(int row);
    void selectedRow(int row);

protected:
    virtual int visibleRows() { return 0; }
    virtual void initWidget(Widget* pWidget) {}
    virtual void moveWidgetToRow(int row, Widget* pWidget) {}

    // non-lazy views only
    virtual void extendWidgetPool() {}
    void extendWidgetPool(int n);

    // lazy views only
    virtual int getOffset() { return 0; }
    virtual void updateScrollWidgetSize() {}
    void scrolledToRow(int rowOffset);

    WidgetListModel*                _pModel;
    int                             _widgetHeight;

private:
    int widgetPoolSize();
    /// The view has a widget pool which is large enough to fill the area of the view
    /// with widgets (created by the model).
    int visibleIndex(int row);
    int countVisibleWidgets();
    Widget* visibleWidget(int index);
    bool itemIsVisible(int row);

    bool                            _lazy;
    std::vector<Widget*>            _widgetPool;
    std::vector<Widget*>            _visibleWidgets;
    std::stack<Widget*>             _freeWidgets;
    int                             _rowOffset;
};


class QtWidget : public QWidget, public Widget
{
    Q_OBJECT
    
public:
    virtual void showWidget();
    virtual void hideWidget();
};


class QtWidgetList : public QScrollArea, public WidgetListView
{
    Q_OBJECT

public:
    QtWidgetList(QWidget* pParent = 0);
    virtual ~QtWidgetList();

protected:
    virtual int visibleRows();
    virtual void initWidget(Widget* pWidget);
    virtual void moveWidgetToRow(int row, Widget* pWidget);

    virtual void updateScrollWidgetSize();
    virtual int getOffset();

signals:
    void moveWidget(int targetRow, Widget* pWidget);
    void selectedWidget(int row);

private slots:
    void move(int targetRow, Widget* pWidget);
    void viewScrolled(int value);

private:
    QWidget*                 _pScrollWidget;
};


class QtWidgetCanvasItem;

class QtWidgetCanvas : public QGraphicsView, public WidgetListView
{
    Q_OBJECT

public:
    QtWidgetCanvas(bool movableWidgets = false, QWidget* pParent = 0);
    virtual ~QtWidgetCanvas();

protected:
    virtual int visibleRows();
    virtual void initWidget(Widget* pWidget);
    virtual void moveWidgetToRow(int row, Widget* pWidget);

    virtual void extendWidgetPool();

signals:
    void moveWidget(int targetRow, Widget* pWidget);
    void extendPoolSignal();
    void selectedWidget(int row);

private slots:
    void move(int targetRow, Widget* pWidget);
    void extendPoolSlot();

private:
    QGraphicsScene*                              _pGraphicsScene;
    std::map<Widget*, QtWidgetCanvasItem*>    _proxyWidgets;
    bool                                         _movableWidgets;
};


#endif

