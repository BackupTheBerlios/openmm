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

#ifndef QtNavigator_INCLUDED
#define QtNavigator_INCLUDED

#include <stack>
#include <QtGui>

#include "QtWidget.h"

class QtNavigable;
class QtNavigatorPanel;

class QtNavigator : public QtWidget
{
    Q_OBJECT

public:
    QtNavigator(QWidget* pParent = 0);
    ~QtNavigator();
    
    void push(QtNavigable* pNavigable);

private slots:
    void expose(QtNavigable* pNavigable);

private:
    QtNavigatorPanel*           _pNavigatorPanel;
    QStackedWidget*             _pStackedWidget;
    QVBoxLayout*                _pNavigatorLayout;
    std::stack<QtNavigable*>    _navigableStack;
};





#endif

