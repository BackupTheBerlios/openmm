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

#ifndef QtNavigatorPanel_INCLUDED
#define QtNavigatorPanel_INCLUDED

#include <stack>
#include <QtGui>

#include "NavigatorImpl.h"

namespace Omm {
namespace Gui {


class QtNavigatorPanelButton : public QPushButton
{
public:
    QtNavigatorPanelButton(View* pView);

//    void paintEvent(QPaintEvent* event);

    View*    _pView;
};


class QtNavigatorPanel : public QWidget
{
    Q_OBJECT

    friend class NavigatorViewImpl;

public:
    QtNavigatorPanel(NavigatorViewImpl* pNavigatorView = 0);

    void push(View* pView, const std::string name);
    void pop(View* pView);

signals:
    void popSignal();
    void popToRootSignal();

private slots:
    void popSlot();
    void popToRootSlot();
    void rightButtonPushedSlot();
    void buttonPushed();
    void textEdited(const QString& text);
    void editingFinished();
    long unsigned int buttonCount();
    View* topView();

private:
    std::stack<QtNavigatorPanelButton*>     _buttonStack;
    QHBoxLayout*                            _pPanelLayout;
    QWidget*                                _pButtonWidget;
    QHBoxLayout*                            _pButtonLayout;
    QPushButton*                            _pRightButton;
    QLineEdit*                              _pSearchWidget;
    QFileIconProvider*                      _pIconProvider;
    NavigatorViewImpl*                      _pNavigatorView;
};


} // namespace Gui
} // namespace Omm

#endif
