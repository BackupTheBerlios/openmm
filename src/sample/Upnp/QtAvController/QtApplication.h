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

#ifndef QtApplication_INCLUDED
#define QtApplication_INCLUDED

#include <QtGui>

#include <Omm/Upnp.h>


class QtController;
class QtMainWindow;
class QStackedWidget;
class QtVisual;
class QtBrowserWidget;
class QtPlayerRack;
class QtControlPanel;
class QtEventFilter;


class QtApplication : public QObject
{
    Q_OBJECT

public:
    QtApplication();
    virtual ~QtApplication();

    virtual void setWindowTitle(const std::string& title);
    virtual void initApplication(int argc = 0, char** argv = 0);
    virtual int eventLoop();
    virtual void quit();

    void addToolBar(QToolBar* pToolBar);

signals:
    void doQuit();

private:
    int                                 _argc;

    QApplication*                       _pApp;
    QtEventFilter*                      _pEventFilter;
    QMainWindow*                        _pMainWindow;
    QStackedWidget*                     _pMainWidget;
    QtBrowserWidget*                    _pBrowserWidget;
    QtPlayerRack*                       _pPlayerRack;
    QtControlPanel*                     _pControlPanel;
    QtVisual*                           _pVisual;
    QtController*                       _pController;

    bool                                _menuVisible;
    bool                                _playerRackVisible;
    bool                                _fullscreen;
};


#endif
