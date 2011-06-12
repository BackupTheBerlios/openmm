/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef QtAvInterface_INCLUDED
#define QtAvInterface_INCLUDED

#include <QtGui>

#include <Omm/UpnpAvController.h>
#include <Omm/Sys.h>

Q_DECLARE_METATYPE(std::string);

class QtVisual;
class QtAvInterface;
class QtBrowserWidget;
class QtPlayerRack;
class QtControlPanel;


class QtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtMainWindow(QWidget* pCentralWidget);

//    virtual void keyPressEvent(QKeyEvent* event);
};


class QtEventFilter : public QObject
{
public:
    QtEventFilter(QtAvInterface* pAvInterface);

private:
    virtual bool eventFilter(QObject* object, QEvent* event);

    QtAvInterface*      _pAvInterface;
//    map<int, Event::EventT> m_eventMap;
};


class QtAvInterface : public QObject, public Omm::Av::AvUserInterface
{
    Q_OBJECT

    friend class QtBrowserWidget;
    friend class QtPlayerRack;
    friend class QtEventFilter;
    
public:
    QtAvInterface();
//     QtAvInterface(int argc = 0);
//     QtAvInterface(int argc, char** argv);
    virtual ~QtAvInterface();
    
    virtual int eventLoop();
    
    virtual void initGui();
    virtual void showMainWindow();
    virtual Omm::Sys::Visual* getVisual();

    virtual void beginNetworkActivity();
    virtual void endNetworkActivity();

    bool menuVisible();
    bool isFullscreen();

public slots:
    virtual void showMenu(bool show);
    virtual void setFullscreen(bool fullscreen);
    virtual void resize(int width, int height);
    virtual void showPlayerRack(bool show);
    virtual void showControlPanel(bool show);
    
signals:
    void nowPlaying(const QString& title, const QString& artist, const QString& album);
    void startNetworkActivity();
    void stopNetworkActivity();
        // volume slider
    void volSliderMoved(int value);

private slots:
    void skipForwardButtonPressed();
    void skipBackwardButtonPressed();

    void rendererSelected(Omm::Av::RendererView* pRenderer);
    
private:
    virtual void beginAddServer(int position);
    virtual void endAddServer(int position);
    virtual void beginRemoveServer(int position);
    virtual void endRemoveServer(int position);
    virtual void beginAddRenderer(int position);
    virtual void endAddRenderer(int position);
    virtual void beginRemoveRenderer(int position);
    virtual void endRemoveRenderer(int position);

    void setupUnixSignalHandlers();
    static void unixSignalHandler(int);

//    virtual void newPosition(int duration, int position);
//    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album);
    virtual void newVolume(const int volume);

    int                                 _argc;
//    QString                             _defaultStyleSheet;
    QString                             _fullscreenStyleSheet;

    QApplication*                       _pApp;
    QtEventFilter*                      _pEventFilter;
    QMainWindow*                        _pMainWindow;
    QStackedWidget*                     _pMainWidget;
    QtBrowserWidget*                    _pBrowserWidget;
    QtPlayerRack*                       _pPlayerRack;
    QtControlPanel*                     _pControlPanel;
    QtVisual*                           _pVisual;

    bool                                _menuVisible;
    bool                                _playerRackVisible;
    bool                                _fullscreen;
};

#endif
