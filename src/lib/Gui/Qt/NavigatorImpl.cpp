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

#include <Poco/NumberFormatter.h>

#include "NavigatorImpl.h"
#include "Gui/Navigator.h"

namespace Omm {
namespace Gui {


class QtNavigatorPanelButton : public QPushButton
{
public:
    QtNavigatorPanelButton(View* pView);

    View*    _pView;
};


QtNavigatorPanelButton::QtNavigatorPanelButton(View* pView) :
_pView(pView)
{
}



QtNavigatorPanel::QtNavigatorPanel(NavigatorViewImpl* pNavigatorView) :
QWidget(pNavigatorView),
_pNavigatorView(pNavigatorView)
{
    _pButtonLayout = new QHBoxLayout(this);
}


void
QtNavigatorPanel::push(View* pView, const std::string name)
{
    QtNavigatorPanelButton* pButton = new QtNavigatorPanelButton(pView);
    pButton->_pView = pView;
    pButton->setText(QString::fromStdString(name));
    connect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
    _pButtonLayout->addWidget(pButton);
    _buttonStack.push(pButton);
}


void
QtNavigatorPanel::pop(View* pView)
{
    while(!_buttonStack.empty() && _buttonStack.top()->_pView != pView) {
        QtNavigatorPanelButton* pButton = _buttonStack.top();
        _pNavigatorView->popView(pButton->_pView);
        disconnect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
        _pButtonLayout->removeWidget(pButton);
        delete pButton;
        _buttonStack.pop();
    }
    _pNavigatorView->exposeView(pView);
}


void
QtNavigatorPanel::buttonPushed()
{
    QtNavigatorPanelButton* pButton = static_cast<QtNavigatorPanelButton*>(QObject::sender());
    pop(pButton->_pView);
}


NavigatorViewImpl::NavigatorViewImpl(View* pView) 
{
    _pNavigatorPanel = new QtNavigatorPanel(this);
    _pStackedWidget = new QStackedWidget(this);
    _pNavigatorLayout = new QVBoxLayout(this);
    _pNavigatorLayout->addWidget(_pNavigatorPanel);
    _pNavigatorLayout->addWidget(_pStackedWidget);

    initViewImpl(pView, this);
}


NavigatorViewImpl::~NavigatorViewImpl()
{
    delete _pNavigatorLayout;
    delete _pStackedWidget;
    delete _pNavigatorPanel;
}


void
NavigatorViewImpl::pushView(View* pView, const std::string name)
{
    QWidget* pWidget = static_cast<QWidget*>(pView->getNativeView());
    _pStackedWidget->addWidget(pWidget);
    _pStackedWidget->setCurrentWidget(pWidget);
    _pNavigatorPanel->push(pView, name);
    pWidget->show();
//    pWidget->resize(_pStackedWidget->size());
//    pView->resize(700, 400);
}


void
NavigatorViewImpl::popView(View* pView)
{
    QWidget* pWidget = static_cast<QWidget*>(pView->getNativeView());
    _pStackedWidget->removeWidget(pWidget);
}


void
NavigatorViewImpl::exposeView(View* pView)
{
    QWidget* pWidget = static_cast<QWidget*>(pView->getNativeView());
    _pStackedWidget->setCurrentWidget(pWidget);
    pWidget->show();
}


} // namespace Gui
} // namespace Omm
