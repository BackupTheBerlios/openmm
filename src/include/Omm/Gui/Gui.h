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

#ifndef Gui_INCLUDED
#define Gui_INCLUDED

#include <stack>

#include <Poco/Format.h>
#include <Poco/Logger.h>
#include <Poco/ClassLoader.h>
#include <Poco/Environment.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Runnable.h>
#include <Poco/Thread.h>
#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>


namespace Omm {
namespace Gui {

class WidgetListModel;
class WidgetListView;
class Navigator;


class Log
{
public:
    static Log* instance();
    
    Poco::Logger& gui();
    
private:
    Log();
    
    static Log*     _pInstance;
    Poco::Logger*   _pGuiLogger;
};


class Widget
{
public:
    Widget();

    virtual void showWidget() {}
    virtual void hideWidget() {}

    class SelectNotification : public Poco::Notification
    {
    public:
        SelectNotification();
    };

    void registerEventNotificationHandler(const Poco::AbstractObserver& observer);

protected:
    virtual void select();

    Poco::NotificationCenter _eventNotificationCenter;
};


//template<class C>
//class WidgetImpl : public Widget
//{
//
//};


class ListWidget : public Widget
{
public:
    ListWidget();

    int getRow();
    void setRow(int row);

    class RowSelectNotification : public Poco::Notification
    {
    public:
        RowSelectNotification(int row);

        int _row;
    };

protected:
    virtual void select();

private:
    int _row;
};


class ListWidgetFactory
{
public:
    virtual ListWidget* createWidget() { return 0; }
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
    virtual int fetch(int rowCount = 10, bool forward = true) { return 0; }
    virtual int lastFetched(bool forward = true) { return (forward ? totalItemCount() : 0); }

    // widget related
    void setWidgetFactory(ListWidgetFactory* pWidgetFactory);
    virtual ListWidget* createWidget();
    virtual ListWidget* getChildWidget(int row) { return 0; }
    virtual void attachWidget(int row, ListWidget* pWidget) {}
    virtual void detachWidget(int row) {}

private:
    WidgetListView*                     _pView;
    ListWidgetFactory*                  _pWidgetFactory;
};


class WidgetListView
{
public:
    WidgetListView(int widgetHeight, bool lazy = false);

    void setModel(WidgetListModel* pModel);
    void insertItem(int row);
    void removeItem(int row);

protected:
    virtual int visibleRows() { return 0; }
    virtual void initWidget(ListWidget* pWidget) {}
    virtual void moveWidget(int row, ListWidget* pWidget) {}
    void resize(int rows);

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
    ListWidget* visibleWidget(int index);
    bool itemIsVisible(int row);
    void moveWidgetToRow(int row, ListWidget* pWidget);
    void selectNotificationHandler(ListWidget::RowSelectNotification* pSelectNotification);

    bool                                _lazy;
    std::vector<ListWidget*>            _widgetPool;
    std::vector<ListWidget*>            _visibleWidgets;
    std::stack<ListWidget*>             _freeWidgets;
    int                                 _rowOffset;
};


class Navigable
{
    friend class Navigator;

public:
    Navigable();

    virtual std::string getBrowserTitle() { return ""; }
    virtual Widget* getWidget() { return 0; }
    /// If getWidget() returns not null but a valid widget, the widget
    /// is pushed on QtNavigator::_pStackedWidget.
    virtual void show() {}
    /// Additionally, show() can be implemented if for example no widget
    /// is pushed but some other action is necessary to show the correct view.
    Navigator* getNavigator() const;

private:
    Navigator*    _pNavigator;
};


class Navigator : public Widget
{
public:
    Navigator();
    ~Navigator();

    void push(Navigable* pNavigable);

protected:
    virtual void pushImpl(Navigable* pNavigable) = 0;

private:
    std::stack<Navigable*>    _navigableStack;
};


}  // namespace Omm
}  // namespace Gui

#endif
