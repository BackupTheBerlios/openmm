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

#include<Omm/UpnpAvLogger.h>

#include "QtNavigator.h"


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
    _pSignalMapper = new QSignalMapper(this);
//    connect(_pSignalMapper, SIGNAL(mapped(QString)), this, SLOT(buttonPushed()));
}


void
QtNavigatorPanel::push(QtNavigable* pNavigable)
{
    QtNavigatorPanelButton* pButton = new QtNavigatorPanelButton(pNavigable);
    pButton->setText(pNavigable->getBrowserTitle());
    _pButtonLayout->addWidget(pButton);
//    connect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
    connect(pButton, SIGNAL(pressed()), this, SLOT(buttonPushed()));
    _buttonStack.push(pButton);
}


void
QtNavigatorPanel::pop(QtNavigable* pNavigable)
{
    while(!_buttonStack.empty() && _buttonStack.top()->getNavigable() != pNavigable) {
        QtNavigatorPanelButton* pButton = _buttonStack.top();
        disconnect(pButton, SIGNAL(pressed()), _pSignalMapper, SLOT (map()));
        _pButtonLayout->removeWidget(pButton);
        delete pButton;
        _buttonStack.pop();
    }
}


void
QtNavigatorPanel::buttonPushed()
{
    QtNavigatorPanelButton* pButton = static_cast<QtNavigatorPanelButton*>(QObject::sender());
    Omm::Av::Log::instance()->upnpav().debug("Qt navigator panel button pushed: " + pButton->getNavigable()->getBrowserTitle().toStdString());
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
    Omm::Av::Log::instance()->upnpav().debug("Qt navigator push: " + pNavigable->getBrowserTitle().toStdString());
    
    if (pNavigable->getWidget()) {
        _pStackedWidget->addWidget(pNavigable->getWidget());
    }
    _pNavigatorPanel->push(pNavigable);
    _navigableStack.push(pNavigable);
    pNavigable->show();
}


void
QtNavigator::expose(QtNavigable* pNavigable)
{
    Omm::Av::Log::instance()->upnpav().debug("Qt navigator expose: " + pNavigable->getBrowserTitle().toStdString());

    while(!_navigableStack.empty() && _navigableStack.top() != pNavigable) {
        QtNavigable* pPoppedNavigable = _navigableStack.top();
        if (pPoppedNavigable->getWidget()) {
            _pStackedWidget->removeWidget(pPoppedNavigable->getWidget());
        }
        delete pPoppedNavigable;
        _navigableStack.pop();
    }
    if (!_navigableStack.empty()) {
        pNavigable->show();
    }
}


//void
//QtDeviceGroup::pushBrowser(QtNavigable* pBrowser)
//{
////    new QtCrumbButton(_pDeviceListView, QModelIndex(), _pCrumbButton);
//    // FIXME: this only works for first level.
//    _pCrumbButton = new QtCrumbButton(_pDeviceListView, QModelIndex(), _pCrumbPanel);
//    _pStackedWidget->addWidget(pBrowser);
//    _stack.push(pBrowser);
//}
//
//
//void
//QtDeviceGroup::popBrowser()
//{
//    _pStackedWidget->removeWidget(_stack.top());
//    _stack.pop();
//}