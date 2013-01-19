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

#include <Omm/UpnpAvLogger.h>
#include <Omm/UpnpAvCtlServer.h>

#include "QtMediaObject.h"
#include "QtNavigator.h"


QtMediaObject::QtMediaObject() :
_lastFetched(0)
{
}


QtMediaObject::~QtMediaObject()
{
}


QString
QtMediaObject::getBrowserTitle()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object get widget browser title");

    if (_pObject) {
        return QString::fromStdString(_pObject->getTitle());
    }
    else {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to get object title (ignoring)");
    }
//    return QString::fromStdString(getTitle());
}


QWidget*
QtMediaObject::getWidget()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object get container widget");

    return _pContainerView;
}


int
QtMediaObject::totalItemCount()
{

    if (!_pObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to get total item count (ignoring)");
        return 0;
    }
    if (_pObject->isContainer()) {
//        LOGNS(Omm::Av, upnpav, debug, "Qt media object child count: " + Poco::NumberFormatter::format(_pObject->getChildCount()));
        LOGNS(Omm::Av, upnpav, debug, "Qt media object child count: " + Poco::NumberFormatter::format(_pObject->getChildCount()));
//        return getChildCount();
//        return _pObject->getChildCount();
        return _pObject->getChildCount();
    }
    return 0;
}


void
QtMediaObject::selectItem(int row)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object select item in row: " + Poco::NumberFormatter::format(row));

    if (!_pObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object null reference (ignoring)");
        return;
    }

    Omm::Av::CtlMediaObject* pChildObject = static_cast<Omm::Av::CtlMediaObject*>(_pObject->getChildForRow(row));
    if (!pChildObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media container cannot get child object (ignoring)");
        return;
    }

    if (pChildObject->isContainer()) {
        if (!getNavigator()) {
            LOGNS(Omm::Av, upnpav, error, "Qt child container of media object cannot be pushed (ignoring)");
            return;
        }
        QtMediaObject* pChildWidget = static_cast<QtMediaObject*>(getWidget(row));
        if (!pChildWidget) {
            LOGNS(Omm::Av, upnpav, error, "Qt media container cannot get child widget to push (ignoring)");
            return;
        }
        pChildWidget->_pContainerView = new QtWidgetList;
        getNavigator()->push(pChildWidget);
        pChildWidget->_pContainerView->setModel(pChildWidget);
    }
    else {
        _pObject->getServer()->selectMediaObject(pChildObject);
    }
}


bool
QtMediaObject::canFetchMore()
{
    return _lastFetched >= _pObject->getChildCount();
}


void
QtMediaObject::fetchMore(bool forward)
{
    _lastFetched += _pObject->fetchChildren();
}


int
QtMediaObject::fetch(int rowCount, bool forward)
{
    int fetched = _pObject->fetchChildren(rowCount);
    _lastFetched += fetched;
    return fetched;
}


int
QtMediaObject::lastFetched(bool forward)
{

    return (forward ? _lastFetched : 0);
}


ListWidget*
QtMediaObject::createWidget()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object create object widget");

    return new QtMediaObject;
}


ListWidget*
QtMediaObject::getWidget(int row)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object get object widget row: " + Poco::NumberFormatter::format(row));

    if (_pObject) {
        Omm::Av::CtlMediaObject* pChildObject = static_cast<Omm::Av::CtlMediaObject*>(_pObject->getChildForRow(row));
        // FIXME: had to comment this out, CtlMediaObject should contain no gui code
//        return pChildObject->getListWidget();
    }
    else {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to get object widget (ignoring)");
    }
}


void
QtMediaObject::attachWidget(int row, ListWidget* pWidget)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object attach widget row: " + Poco::NumberFormatter::format(row));
    if (!pWidget) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object cannot attach null widget (ignoring)");
        return;
    }
    if (!_pObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to attach object widget (ignoring)");
        return;
    }
    Omm::Av::CtlMediaObject* pChildObject = static_cast<Omm::Av::CtlMediaObject*>(_pObject->getChildForRow(row));
    if (!pChildObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to get child object (ignoring)");
        return;
    }
    // FIXME: had to comment this out, CtlMediaObject should contain no gui code
//    pChildObject->setListWidget(pWidget);
    QtMediaObject* pChildWidget = static_cast<QtMediaObject*>(pWidget);
    pChildWidget->_pObject = pChildObject;

    connect(pChildWidget, SIGNAL(showWidgetSignal()), pChildWidget, SLOT(show()));
    connect(pChildWidget, SIGNAL(hideWidgetSignal()), pChildWidget, SLOT(hide()));
    connect(pChildWidget, SIGNAL(configureWidget()), pChildWidget, SLOT(configure()));
    connect(pChildWidget, SIGNAL(unconfigureWidget()), pChildWidget, SLOT(unconfigure()));

    emit pChildWidget->configureWidget();
    emit pChildWidget->showWidgetSignal();
}


void
QtMediaObject::detachWidget(int row)
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object detach widget row: " + Poco::NumberFormatter::format(row));

    if (!_pObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to detach object widget (ignoring)");
        return;
    }
    Omm::Av::CtlMediaObject* pChildObject = static_cast<Omm::Av::CtlMediaObject*>(_pObject->getChildForRow(row));
    // FIXME: had to comment this out, CtlMediaObject should contain no gui code
//    QtMediaObject* pChildWidget = static_cast<QtMediaObject*>(pChildObject->getListWidget());
//
//    emit pChildWidget->hideWidgetSignal();
//    emit pChildWidget->unconfigureWidget();
//
//    disconnect(pChildWidget, SIGNAL(showWidgetSignal()), pChildWidget, SLOT(show()));
//    disconnect(pChildWidget, SIGNAL(hideWidgetSignal()), pChildWidget, SLOT(hide()));
//    disconnect(pChildWidget, SIGNAL(configureWidget()), pChildWidget, SLOT(configure()));
//    disconnect(pChildWidget, SIGNAL(unconfigureWidget()), pChildWidget, SLOT(unconfigure()));
//
//    pChildWidget->_pObject = 0;
    // FIXME: had to comment this out, CtlMediaObject should contain no gui code
//    pChildObject->setListWidget(0);
}


void
QtMediaObject::configure()
{
    LOGNS(Omm::Av, upnpav, debug, "Qt media object configure");

    if (!_pObject) {
        LOGNS(Omm::Av, upnpav, error, "Qt media object failed to configure object (ignoring)");
        return;
    }
    setLabel(_pObject->getTitle());
//    if (_pObject->isContainer()) {
//        _pObject->setWidgetFactory(this);
//        _pContainerView->setModel(_pObject);
//    }
}


void
QtMediaObject::unconfigure()
{
    setLabel("");
}
