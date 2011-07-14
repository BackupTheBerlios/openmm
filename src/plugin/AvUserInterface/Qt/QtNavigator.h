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

#ifndef QtBrowser_INCLUDED
#define QtBrowser_INCLUDED

#include <QWidget>

class QtNavigable
{
public:
    virtual QString getBrowserTitle() { return ""; }
//    virtual void show(QtNavigator* pNavigator) {}
    virtual QWidget* getWidget() { return 0; }
    // if getWidget() returns not null but a valid widget, the widget
    // is pushed on QtNavigator::_pStackedWidget.
};


class QtNavigatorPanel : public QWidget
{
    Q_OBJECT

public:
};


class QtNavigator : public QWidget
{
    Q_OBJECT

public:
    void push(QtNavigable* pNavigable, QWidget* pWidget = 0);
    void pop(QtNavigable* pNavigable, QWidget* pWidget = 0);

private:
    QtNavigatorPanel*           _pNavigatorPanel;
    QStackedWidget*             _pStackedWidget;
};





#endif

