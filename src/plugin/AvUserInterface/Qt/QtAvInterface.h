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


//class QtPlayerRackButton : public QPushButton
//{
//public:
//    QtPlayerRackButton(QWidget* pParent = 0);
//
//    void setPlayerName(const std::string& name);
//    void setTitleName(const std::string& name);
//
//private:
//    void setLabel();
//
//    std::string     _playerName;
//    std::string     _titleName;
//};
//

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
//    // position slider
//    void sliderMoved(int value);
//    void setSlider(int duration, int pos);
//    // volume slider
//    void volSliderMoved(int value);

    void nowPlaying(const QString& title, const QString& artist, const QString& album);
    void startNetworkActivity();
    void stopNetworkActivity();

private slots:
//    void setSeekSlider(int max, int val);
//    void setVolumeSlider(int val);
//    void setTrackInfo(const QString& title, const QString& artist, const QString& album);
    
//    void playButtonPressed();
//    void stopButtonPressed();
    void skipForwardButtonPressed();
    void skipBackwardButtonPressed();
//    void positionSliderMoved(int position);
//    void volumeSliderMoved(int value);

    void rendererSelected(Omm::Av::RendererView* pRenderer);
//    /*
//        QAbstractSlider emits signal valueChanged() when the slider was
//        once moved and some time later (a new track is loaded), the range
//        changes. This triggers a Seek in the MediaRenderer to the position
//        of the last Seek (in the previous track). The following two slots
//        make QAbstractSlider only emit valueChanged() when triggered by
//        a user action.
//        This could be considered a bug and not a feature ...
//    */
//    void checkSliderMoved(int value);
//    void setSliderMoved(int value);
    
private:
    virtual void beginAddServer(int position);
    virtual void endAddServer(int position);
    virtual void beginRemoveServer(int position);
    virtual void endRemoveServer(int position);
    virtual void beginAddRenderer(int position);
    virtual void endAddRenderer(int position);
    virtual void beginRemoveRenderer(int position);
    virtual void endRemoveRenderer(int position);

//    virtual void newPosition(int duration, int position);
//    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album);
//    virtual void newVolume(const int volume);
    
    int                                 _argc;
//    QString                             _defaultStyleSheet;
    QString                             _fullscreenStyleSheet;

    QApplication*                       _pApp;
    QtEventFilter*                      _pEventFilter;
    QMainWindow*                        _pMainWindow;
    QStackedWidget*                     _pMainWidget;
    QtBrowserWidget*                    _pBrowserWidget;
    QtPlayerRack*                       _pPlayerRack;
//    QToolBar*                           _pControlPanel;
    QtControlPanel*                     _pControlPanel;
    QtVisual*                           _pVisual;

//    bool                                _sliderMoved;
//    bool                                _playToggle;
    bool                                _menuVisible;
    bool                                _playerRackVisible;
    bool                                _fullscreen;

//    QPushButton*                        _pBackButton;
//    QPushButton*                        _pPlayButton;
//    QPushButton*                        _pStopButton;
//    QPushButton*                        _pForwardButton;
//
//    QSlider*                            _pVolumeSlider;
//    QSlider*                            _pSeekSlider;
//
//    QtPlayerRackButton*                 _pPlayerRackButton;
};

#endif
