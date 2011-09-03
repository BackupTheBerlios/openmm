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
//    _pSignalMapper = new QSignalMapper(this);
//    connect(_pSignalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonPushed()));
}


void
QtNavigatorPanel::push(View* pView, const std::string name)
{
    QtNavigatorPanelButton* pButton = new QtNavigatorPanelButton(pView);
    pButton->_pView = pView;
    pButton->setText(QString::fromStdString(name));
//    connect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
    connect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
    _pButtonLayout->addWidget(pButton);
    _buttonStack.push(pButton);
}


void
QtNavigatorPanel::pop(View* pView)
{
    while(!_buttonStack.empty() && _buttonStack.top()->_pView != pView) {
        QtNavigatorPanelButton* pButton = _buttonStack.top();
//        disconnect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
        disconnect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
        _pButtonLayout->removeWidget(pButton);
        delete pButton;
        _buttonStack.pop();
    }
}


void
QtNavigatorPanel::buttonPushed()
{
    QtNavigatorPanelButton* pButton = static_cast<QtNavigatorPanelButton*>(QObject::sender());
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator panel button pushed: " + pButton->getNavigable()->getBrowserTitle().toStdString());
//    Omm::Util::Log::instance()->plugin().debug("Qt navigator panel button pushed: " + pButton->getNavigable()->getBrowserTitle().toStdString());
    pop(pButton->_pView);
    _pNavigatorView->expose(pButton->_pView);
//    emit selectedView(pButton->_pView);
    
}


NavigatorViewImpl::NavigatorViewImpl(View* pView, View* pParent) :
QWidget(static_cast<QWidget*>(pParent? pParent->getNativeView() : 0)),
ViewImpl(pView, this)
{
    _pNavigatorPanel = new QtNavigatorPanel(this);
    _pStackedWidget = new QStackedWidget(this);
    _pNavigatorLayout = new QVBoxLayout(this);
    _pNavigatorLayout->addWidget(_pNavigatorPanel);
    _pNavigatorLayout->addWidget(_pStackedWidget);

//    connect(_pNavigatorPanel, SIGNAL(selectedView(QtNavigable*)), this, SLOT(expose(QtNavigable*)));
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
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " ...");
//    Omm::Util::Log::instance()->plugin().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString());

//        Omm::Av::Log::instance()->upnpav().debug("Qt navigator add widget: " + Poco::NumberFormatter::format(pNavigable->getView()));
    QWidget* pWidget = static_cast<QWidget*>(pView->getNativeView());
    _pStackedWidget->addWidget(pWidget);
    _pStackedWidget->setCurrentWidget(pWidget);
//        Omm::Av::Log::instance()->upnpav().debug("Qt navigator add widget finished.");
    _pNavigatorPanel->push(pView, name);
//    _navigableStack.push(pNavigable);
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator showing widget ...");
    pWidget->show();
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " finished.");
}


void
NavigatorViewImpl::expose(View* pView)
{
//    Omm::Av::Log::instance()->upnpav().debug("Qt navigator expose: " + pNavigable->getBrowserTitle().toStdString());
//    Omm::Util::Log::instance()->plugin().debug("Qt navigator expose: " + pNavigable->getBrowserTitle().toStdString());

    QWidget* pWidget = static_cast<QWidget*>(pView->getNativeView());
    pWidget->show();
    _pStackedWidget->removeWidget(pWidget);

//    while(!_navigableStack.empty() && _navigableStack.top() != pNavigable) {
//        QtNavigable* pPoppedNavigable = _navigableStack.top();
//        if (pPoppedNavigable->getView()) {
//            _pStackedWidget->removeWidget(pPoppedNavigable->getView());
//        }
//        // FIXME: crash when deleting popped navigable
////        delete pPoppedNavigable;
////        _navigableStack.pop();
//    }
//    if (!_navigableStack.empty()) {
//        pNavigable->show();
//    }
}


} // namespace Gui
} // namespace Omm
