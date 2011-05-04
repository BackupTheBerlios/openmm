/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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
#include <QtSvg/QSvgRenderer>

#include <Omm/UpnpAvController.h>
#include <Omm/Sys.h>

#include "QtBrowserModel.h"
#include "QtRendererListModel.h"
#include "ui_QtBrowserWidget.h"
#include "ui_QtPlayerRack.h"

class QtCrumbButton : public QWidget
{
    Q_OBJECT

    friend class QtAvInterface;
public:
    QtCrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent = 0);
    ~QtCrumbButton();
    
    void setChild(QtCrumbButton* child) { _child = child; }
    
    static QtCrumbButton* _pLastCrumbButton;
    
private slots:
    void buttonPressed();
    
private:
    void deleteChildren();
    
    QLayout*            _parentLayout;
    QHBoxLayout*        _boxLayout;
    QPushButton*        _button;
    QAbstractItemView*  _browserView;
    const QModelIndex   _index;
    QtCrumbButton*      _child;
};


class QtActivityIndicator : public QWidget
{
    Q_OBJECT
    
public:
    QtActivityIndicator(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~QtActivityIndicator();
    
public slots:
    void startActivity();
    void stopActivity();
    
private slots:
    void stopIndicator();

private:
    void paintEvent(QPaintEvent *event);
    void setActivityInProgress(bool set);
    bool activityInProgress();
    void setIndicatorOn(bool set);
    bool indicatorOn();

    const int           _indicateDuration;
    QSvgRenderer*       _symbolRenderer;
    bool                _activityInProgress;
    QMutex              _activityInProgressLock;
    bool                _indicatorOn;
    QMutex              _indicatorOnLock;
    QTimer              _offTimer;
};


class QtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtMainWindow(QWidget* pCentralWidget);

//    virtual void keyPressEvent(QKeyEvent* event);
};


class QtVisual : public QObject, public Omm::Sys::Visual
{
    Q_OBJECT

    friend class QtAvInterface;

public:
    QtVisual(QWidget* pParent = 0);
    virtual ~QtVisual();

    virtual void show();
    virtual void hide();

    virtual void* getWindow();
    virtual VisualType getType();

signals:
    void showMenu(bool show);

private:
    QWidget*        _pWidget;
#ifdef __LINUX__
    Poco::UInt32    _x11Window;
#endif
};


class QtAvInterface;

// NOTE: QtEventFilter, see jam.2006-12-28/src/graphic/qt
class QtEventFilter : public QObject
{
public:
    QtEventFilter(QtAvInterface* avInterface);

private:
    virtual bool eventFilter(QObject* object, QEvent* event);

    QtAvInterface*      _pAvInterface;
//    map<int, Event::EventT> m_eventMap;
};


class QtAvInterface : public QObject, public Omm::Av::AvUserInterface
{
    Q_OBJECT

    friend class QtRendererListModel;
    friend class QtBrowserModel;
    
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
    
signals:
    // position slider
    void sliderMoved(int value);
    void setSlider(int duration, int pos);
    // volume slider
    void volSliderMoved(int value);
    void nowPlaying(const QString& title, const QString& artist, const QString& album);
    void startNetworkActivity();
    void stopNetworkActivity();

private slots:
    void setSeekSlider(int max, int val);
    void setVolumeSlider(int val);
    void setTrackInfo(const QString& title, const QString& artist, const QString& album);
    
    void playButtonPressed();
    void stopButtonPressed();
//     void pauseButtonPressed();
    void skipForwardButtonPressed();
    void skipBackwardButtonPressed();
    void positionSliderMoved(int position);
    void volumeSliderMoved(int value);
    
    void rendererSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void browserItemActivated(const QModelIndex& index);
    void browserItemSelected(const QModelIndex& index);
    /*
        QAbstractSlider emits signal valueChanged() when the slider was
        once moved and some time later (a new track is loaded), the range
        changes. This triggers a Seek in the MediaRenderer to the position
        of the last Seek (in the previous track). The following two slots
        make QAbstractSlider only emit valueChanged() when triggered by
        a user action.
        This could be considered a bug and not a feature ...
    */
    void checkSliderMoved(int value);
    void setSliderMoved(int value);
    
private:
    virtual void beginAddRenderer(int position);
    virtual void beginAddServer(int position);
    virtual void beginRemoveRenderer(int position);
    virtual void beginRemoveServer(int position);
    virtual void endAddRenderer(int position);
    virtual void endAddServer(int position);
    virtual void endRemoveRenderer(int position);
    virtual void endRemoveServer(int position);

    virtual void newPosition(int duration, int position);
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album);
    virtual void newVolume(const int volume);

    QtBrowserModel*                     _pBrowserModel;
    QtRendererListModel*                _pRendererListModel;
    
    int                                 _argc;
    QApplication*                       _pApp;
    QMainWindow*                        _pMainWindow;
    QStackedWidget*                     _pMainWidget;
    Ui::_browserWidget                  _browserWidget;
//    Ui::_rendererWidget                 _rendererWidget;
    Ui::_playerRack                     _playerRack;
    QToolBar*                           _pToolBar;
    QFrame*                             _pBrowserWidget;
    QDockWidget*                        _pPlayerRack;
    QtActivityIndicator*                _pActivityIndicator;
    QtCrumbButton*                      _pServerCrumbButton;
    static QtCrumbButton*               _pLastCrumbButton;
    Omm::Av::ControllerObject*          _pCurrentServer;
    bool                                _sliderMoved;
    bool                                _playToggle;
    bool                                _menuVisible;
    bool                                _fullscreen;
    QtVisual*                           _pVisual;

    QPushButton*                        _pBackButton;
    QPushButton*                        _pPlayButton;
    QPushButton*                        _pStopButton;
    QPushButton*                        _pForwardButton;

    QSlider*                            _pVolumeSlider;
    QSlider*                            _pSeekSlider;

    QtEventFilter*                      _pEventFilter;
};

#endif
