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

#ifndef QtUpnpApplication_INCLUDED
#define QtUpnpApplication_INCLUDED

#include <Omm/Upnp.h>
#include <Omm/UpnpApplication.h>
#include <Omm/Sys.h>


class QApplication;
class QtMainWindow;
class QStackedWidget;
class QtVisual;
class QtBrowserWidget;
class QtPlayerRack;
class QtControlPanel;
class QtEventFilter;
class QtControllerWidget;


class QtApplication : public Omm::UpnpApplication
{
public:
    QtApplication();
    virtual ~QtApplication();

    virtual int eventLoop();

private:
    virtual void init();
    
    virtual Omm::Controller* createController();
    virtual void addController();
    virtual void removeController();

    int                                 _argc;
//    QString                             _fullscreenStyleSheet;

    QApplication*                       _pApp;
    QtEventFilter*                      _pEventFilter;
    QMainWindow*                        _pMainWindow;
    QStackedWidget*                     _pMainWidget;
    QtControllerWidget*                 _pControllerWidget;
    QtBrowserWidget*                    _pBrowserWidget;
    QtPlayerRack*                       _pPlayerRack;
    QtControlPanel*                     _pControlPanel;
    QtVisual*                           _pVisual;

    bool                                _menuVisible;
    bool                                _playerRackVisible;
    bool                                _fullscreen;
};


#endif
