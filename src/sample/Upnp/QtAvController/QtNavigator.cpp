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
// need a logger
#include<Omm/Util.h>
// FIXME: UPNP.AV logger works, but PLUGIN logger in Util.h doesn't
#include<Omm/UpnpAvLogger.h>

#include "QtNavigator.h"
#include "QtNavigable.h"
#include "QtNavigatorPrivate.h"


QtNavigatorPanelButton::QtNavigatorPanelButton(QtNavigable* pNavigable) :
_pNavigable(pNavigable)
{
}


QtNavigable*
QtNavigatorPanelButton::getNavigable()
{
    return _pNavigable;
}


QtNavigatorPanel::QtNavigatorPanel(QWidget* pParent) :
QWidget(pParent)
{
    _pButtonLayout = new QHBoxLayout(this);
//    _pSignalMapper = new QSignalMapper(this);
//    connect(this, SIGNAL(popSignal()), this, SLOT(pop()));
//    connect(_pSignalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonPushed()));
}


void
QtNavigatorPanel::push(QtNavigable* pNavigable)
{
    QtNavigatorPanelButton* pButton = new QtNavigatorPanelButton(pNavigable);
    pButton->setText(pNavigable->getBrowserTitle());
//    connect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
    connect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
    _pButtonLayout->addWidget(pButton);
    _buttonStack.push(pButton);
}


void
QtNavigatorPanel::pop(QtNavigable* pNavigable)
{
    while(!_buttonStack.empty() && _buttonStack.top()->getNavigable() != pNavigable) {
        QtNavigatorPanelButton* pButton = _buttonStack.top();
//        disconnect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
        disconnect(pButton, SIGNAL(pressed()), this, SLOT (buttonPushed()));
        _pButtonLayout->removeWidget(pButton);
        delete pButton;
        _buttonStack.pop();
    }
}


void
QtNavigatorPanel::buttonPushed()
{
    QtNavigatorPanelButton* pButton = static_cast<QtNavigatorPanelButton*>(QObject::sender());
    LOGNS(Omm::Av, upnpav, debug, "Qt navigator panel button pushed: " + pButton->getNavigable()->getBrowserTitle().toStdString());
//    LOGNS(Omm::Util, plugin, debug, "Qt navigator panel button pushed: " + pButton->getNavigable()->getBrowserTitle().toStdString());
    pop(pButton->getNavigable());
    emit selectedNavigable(pButton->getNavigable());
}


QtNavigator::QtNavigator(QWidget* pParent) :
QWidget(pParent)
{
    _pNavigatorPanel = new QtNavigatorPanel(this);
    _pStackedWidget = new QStackedWidget(this);
    _pNavigatorLayout = new QVBoxLayout(this);
    _pNavigatorLayout->addWidget(_pNavigatorPanel);
    _pNavigatorLayout->addWidget(_pStackedWidget);

    connect(_pNavigatorPanel, SIGNAL(selectedNavigable(QtNavigable*)), this, SLOT(expose(QtNavigable*)));
}


QtNavigator::~QtNavigator()
{
    delete _pNavigatorLayout;
    delete _pStackedWidget;
    delete _pNavigatorPanel;
}


void
QtNavigator::push(QtNavigable* pNavigable)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " ...");
//    LOGNS(Omm::Util, plugin, debug, "Qt navigator push: " + pNavigable->getBrowserTitle().toStdString());

    pNavigable->_pNavigator = this;
    if (pNavigable->getWidget()) {
        LOGNS(Omm::Av, upnpav, debug, "Qt navigator add widget: " + Poco::NumberFormatter::format(pNavigable->getWidget()));
        _pStackedWidget->addWidget(pNavigable->getWidget());
        _pStackedWidget->setCurrentWidget(pNavigable->getWidget());
        LOGNS(Omm::Av, upnpav, debug, "Qt navigator add widget finished.");
    }
    _pNavigatorPanel->push(pNavigable);
    _navigableStack.push(pNavigable);
    LOGNS(Omm::Av, upnpav, debug, "Qt navigator showing widget ...");
    pNavigable->show();
    LOGNS(Omm::Av, upnpav, debug, "Qt navigator push: " + pNavigable->getBrowserTitle().toStdString() + " finished.");
}


void
QtNavigator::expose(QtNavigable* pNavigable)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt navigator expose: " + pNavigable->getBrowserTitle().toStdString());
//    LOGNS(Omm::Util, plugin, debug, "Qt navigator expose: " + pNavigable->getBrowserTitle().toStdString());

    pNavigable->show();
    while(!_navigableStack.empty() && _navigableStack.top() != pNavigable) {
        QtNavigable* pPoppedNavigable = _navigableStack.top();
        if (pPoppedNavigable->getWidget()) {
            _pStackedWidget->removeWidget(pPoppedNavigable->getWidget());
        }
        // FIXME: crash when deleting popped navigable
//        delete pPoppedNavigable;
        _navigableStack.pop();
    }
//    if (!_navigableStack.empty()) {
//        pNavigable->show();
//    }
}
