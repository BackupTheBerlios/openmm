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

#ifndef QtMediaObject_INCLUDED
#define QtMediaObject_INCLUDED

#include <Omm/UpnpAvCtlObject.h>
#include <Omm/Util.h>

#include "QtWidget.h"
#include "QtWidgetList.h"
#include "QtNavigable.h"

// NOTE: QtNavigable could go into CtlMediaObject as a generic Navigable, with a generic Navigator
class QtMediaObject : public QtSimpleListWidget, public QtNavigable, public Omm::Util::ListWidgetFactory
{
    friend class QtMediaServer;
    friend class QtMediaServerGroup;

public:
    QtMediaObject();
    ~QtMediaObject();
    
    // QtNavigable interface
    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();

    // QtWidgetFactory interface
    virtual Omm::Util::ListWidget* createWidget();

public slots:
    virtual void configure();
    virtual void unconfigure();

private:
    Omm::Av::CtlMediaObject*    _pObject;
    QtWidgetList*               _pContainerView;
};


#endif

