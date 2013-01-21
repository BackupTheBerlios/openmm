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

#ifndef QtMediaContainer_INCLUDED
#define QtMediaContainer_INCLUDED

#include <QtGui>

#include <Omm/UpnpAvCtlObject.h>
#include <Omm/Util.h>

#include "QtNavigable.h"

class QtMediaServerWidget;

class QtMediaContainer : public QtNavigable
{
    friend class QtMediaServerWidget;
    friend class QtMediaServer;
    friend class QtMediaServerGroup;

public:
    QtMediaContainer();
    ~QtMediaContainer();

    // QtNavigable interface
    virtual QString getBrowserTitle();
    virtual void show();

private:
    Omm::Av::CtlMediaObject*    _pObject;
    QtMediaServerWidget*      _pServerWidget;
    QModelIndex                  _modelIndex;
};


#endif

