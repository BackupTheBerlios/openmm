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

#include "QtMediaObject.h"
#include <Omm/UpnpAvLogger.h>


QtMediaObject::QtMediaObject()
{
//    _pContainerView = new QtWidgetList;
}


QtMediaObject::~QtMediaObject()
{

}


QString
QtMediaObject::getBrowserTitle()
{
    if (_pObject) {
        return QString::fromStdString(_pObject->getTitle());
    }
    else {
        Omm::Av::Log::instance()->upnpav().error("Qt media object failed to get object title (ignoring)");
    }
}


QWidget*
QtMediaObject::getWidget()
{
    return _pContainerView;
}


Omm::Util::Widget*
QtMediaObject::createWidget()
{
    return new QtMediaObject;
}


void
QtMediaObject::configure()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media object configure");
    
    if (!_pObject) {
        Omm::Av::Log::instance()->upnpav().error("Qt media object failed to configure object (ignoring)");
        return;
    }
    setLabel(_pObject->getTitle());
    if (_pObject->isContainer()) {
        _pObject->setWidgetFactory(this);
        _pContainerView->setModel(_pObject);
    }
}


void
QtMediaObject::unconfigure()
{
    setLabel("");
}
