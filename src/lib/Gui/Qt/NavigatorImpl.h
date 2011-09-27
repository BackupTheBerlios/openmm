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

#ifndef NavigatorImpl_INCLUDED
#define NavigatorImpl_INCLUDED

#include <stack>
#include <QtGui>
#include "ViewImpl.h"


namespace Omm {
namespace Gui {

class QtNavigatorPanel;
class QtNavigatorPanelButton;

class NavigatorViewImpl : public QWidget, public ViewImpl
{
    friend class QtNavigatorPanel;
    
public:
    NavigatorViewImpl(View* pView);
    virtual ~NavigatorViewImpl();
    
    void pushView(View* pView, const std::string name);

private:
    void popView(View* pView);
    void exposeView(View* pView);

    QtNavigatorPanel*           _pNavigatorPanel;
    QStackedWidget*             _pStackedWidget;
    QVBoxLayout*                _pNavigatorLayout;
    std::stack<View*>           _views;
};


class QtNavigatorPanel : public QWidget
{
    Q_OBJECT

    friend class NavigatorViewImpl;
    
public:
    QtNavigatorPanel(NavigatorViewImpl* pNavigatorView = 0);

    void push(View* pView, const std::string name);
    void pop(View* pView);

private slots:
    void buttonPushed();

private:
    std::stack<QtNavigatorPanelButton*>     _buttonStack;
    QHBoxLayout*                            _pButtonLayout;
    QFileIconProvider*                      _pIconProvider;
    NavigatorViewImpl*                      _pNavigatorView;
};


} // namespace Gui
} // namespace Omm


#endif

