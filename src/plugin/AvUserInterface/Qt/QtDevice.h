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

#ifndef QtDevice_INCLUDED
#define QtDevice_INCLUDED

#include <QtGui>
#include <Omm/UpnpAvController.h>

#include "QtNavigable.h"

class QtMediaContainerItem;
class QtMediaServerModel;


class QtMediaServer : public QtNavigable, public Omm::Av::CtlMediaServer
{
public:
    QtMediaServer();
    ~QtMediaServer();

    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();
    
private:
    virtual void initController();
    virtual void selected();

    QtMediaServerModel*             _pMediaServerModel;
    QTreeView*                      _pMediaServerListView;
    QtMediaContainerItem*           _pMediaContainerItem;
};


#endif

