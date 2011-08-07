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

#ifndef QtMediaServer_INCLUDED
#define QtMediaServer_INCLUDED

#include <QtGui>

#include <Omm/UpnpAvController.h>
#include <Omm/UpnpAvCtlServer.h>
#include <Omm/UpnpAvCtlObject.h>
#include <Omm/Util.h>

#include "QtNavigable.h"
#include "QtWidget.h"
#include "QtMediaObject.h"


class QtMediaObject;
class QtMediaServerWidget;
class QtWidgetList;


class QtMediaServer : public QtNavigable, public Omm::Av::CtlMediaServer
{
    friend class QtMediaServerWidget;

public:
    QtMediaServer();
    ~QtMediaServer();

    void setDeviceWidget(QtMediaServerWidget* pWidget);
    QtMediaServerWidget* getDeviceWidget();
    
    // QtNavigable interface
    virtual QString getBrowserTitle();
//    virtual QWidget* getWidget();

private:
    virtual void initController();
//    virtual void selected();

    QtMediaServerWidget*            _pMediaServerWidget;
//    QtWidgetList*                   _pMediaContainerWidget;

    QTextCodec*                     _charEncoding;
    QFileIconProvider*              _iconProvider;
};


class QtMediaServerWidget : public QtSimpleListWidget
{
    Q_OBJECT

    friend class QtMediaServer;
    friend class QtMediaServerGroup;

public:
    QtMediaServerWidget(QtMediaServer* pMediaServer = 0);

public slots:
    virtual void configure();
    virtual void unconfigure();

private:
    QtMediaServer*                  _pMediaServer;
};


#endif

