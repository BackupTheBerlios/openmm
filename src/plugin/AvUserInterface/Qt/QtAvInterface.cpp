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

#include <Poco/ClassLibrary.h>

#include "QtAvInterface.h"
#include "QtBrowserWidget.h"


QtMainWindow::QtMainWindow(QWidget* pCentralWidget)
{
    setCentralWidget(pCentralWidget);
    resize(800, 480);
}


//void
//QtMainWindow::keyPressEvent(QKeyEvent* event)
//{
//    Omm::Av::Log::instance()->upnpav().debug("key pressed: " + event->text().toStdString() + ", key no: " + Poco::NumberFormatter::format(event->key()));
//}


QtVisual::QtVisual(QWidget* pParent)
{
    _pWidget = new QLabel(pParent);
    _pWidget->setAutoFillBackground(true);
    QPalette pal = _pWidget->palette();
    pal.setColor(QPalette::Window, Qt::black);
    _pWidget->setPalette(pal);
    _pWidget->setAlignment(Qt::AlignCenter);

    connect(this, SIGNAL(signalShowImage(const std::string&)), this, SLOT(slotShowImage(const std::string&)), Qt::QueuedConnection);
}


QtVisual::~QtVisual()
{
    delete _pWidget;
}


void
QtVisual::show()
{
    emit showMenu(false);
}


void
QtVisual::hide()
{
    emit showMenu(true);
}


void*
QtVisual::getWindow()
{
#ifdef __LINUX__
    _x11Window = _pWidget->winId();
    return &_x11Window;
#else
    return 0;
#endif
}


Omm::Sys::Visual::VisualType
QtVisual::getType()
{
    // QtVisual is multi-platform, and type of visual is platform dependent.
#ifdef __LINUX__
    return Omm::Sys::Visual::VTX11;
#elif __DARWIN__
    return Omm::Sys::Visual::VTMacOSX;
#elif __WINDOWS__
    return Omm::Sys::Visual::VTWin;
#else
    return Omm::Sys::Visual::VTNone;
#endif
}


void
QtVisual::renderImage(const std::string& imageData)
{
    emit signalShowImage(imageData);
}


void
QtVisual::blank()
{
    _pWidget->clear();
}


void
QtVisual::slotShowImage(const std::string& imageData)
{
    QPixmap pixmap;
    pixmap.loadFromData((uchar*)imageData.c_str(), (uint)imageData.size());
    _pWidget->setPixmap(pixmap.scaled(getWidth(), getHeight(), Qt::KeepAspectRatio));
}


QtPlayerRackButton::QtPlayerRackButton(QWidget* pParent) :
QPushButton(pParent)
{
    setMinimumWidth(250);
    setCheckable(true);
}


void
QtPlayerRackButton::setPlayerName(const std::string& name)
{
    _playerName = name;
    setLabel();
}


void
QtPlayerRackButton::setTitleName(const std::string& name)
{
    _titleName = name;
    setLabel();
}


void
QtPlayerRackButton::setLabel()
{
    if (_titleName == "") {
        setText(_playerName.c_str());
    }
    else {
        setText((_playerName + " - " + _titleName).c_str());
    }
}


QtEventFilter::QtEventFilter(QtAvInterface* avInterface) :
_pAvInterface(avInterface)
{
}


bool
QtEventFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = (QKeyEvent*)event;
        Omm::Av::Log::instance()->upnpav().debug("key pressed: " + keyEvent->text().toStdString() + ", key no: " + Poco::NumberFormatter::format(keyEvent->key()));
//        if (Controler::instance()->getCurrentPage()->hasEventType(m_eventMap.find(k->key())->second)) {
//            Controler::instance()->queueEvent(new Event(m_eventMap.find(k->key())->second));
//            // don't forward the event to the Qt event loop.
//            return true;
//        }
        // menu
        if (keyEvent->text() == "m" || keyEvent->key() == 16777301) {
            _pAvInterface->showMenu(!_pAvInterface->menuVisible());
            return true;
        }
        // fullscreen
        else if (keyEvent->text() == "x") {
            _pAvInterface->setFullscreen(!_pAvInterface->isFullscreen());
            return true;
        }
        // power
        else if (keyEvent->key() == 16777399) {
            return true;
        }
        // vol up
        else if (((keyEvent->modifiers() & Qt::AltModifier) && keyEvent->key() == 16777235) || keyEvent->key() == 16777330) {
            Omm::Av::Log::instance()->upnpav().debug("vol up key");
            int oldValue = _pAvInterface->_pVolumeSlider->value();
            _pAvInterface->volumeChanged(++oldValue);
            return true;
        }
        // vol down
        else if (((keyEvent->modifiers() & Qt::AltModifier) && keyEvent->key() == 16777237) || keyEvent->key() == 16777328) {
            Omm::Av::Log::instance()->upnpav().debug("vol down key");
            int oldValue = _pAvInterface->_pVolumeSlider->value();
            _pAvInterface->volumeChanged(--oldValue);
            return true;
        }
        // chan up
        else if (((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == 16777235) || keyEvent->key() == 16777238) {
            _pAvInterface->skipForwardButtonPressed();
            return true;
        }
        // chan down
        else if (((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == 16777237) || keyEvent->key() == 16777239) {
            _pAvInterface->skipBackwardButtonPressed();
            return true;
        }
        // back
        else if (keyEvent->key() == 16777219) {
            _pAvInterface->_pBrowserWidget->goBack();
//            QtCrumbButton* pButton = _pAvInterface->_pServerCrumbButton->_pLastCrumbButton->_parent;
//            if (pButton) {
//                pButton->buttonPressed();
//            }
            return true;
        }
        // play
        else if (keyEvent->text() == "p") {
            _pAvInterface->playPressed();
            return true;
        }
        // stop
        else if (keyEvent->text() == "s") {
            _pAvInterface->stopPressed();
            return true;
        }
        // skip fwd
        else if (keyEvent->text() == "f") {
            _pAvInterface->skipForwardButtonPressed();
            return true;
        }
        // skip back
        else if (keyEvent->text() == "b") {
            _pAvInterface->skipBackwardButtonPressed();
            return true;
        }
        // mute
        else if (keyEvent->text() == " " || keyEvent->key() == 16777329) {
            return true;
        }
    }
    // standard event processing in the Qt event loop.
    return false;
}


QtAvInterface::QtAvInterface() :
_argc(0),
_sliderMoved(false),
_playToggle(true),
_fullscreen(false)
{
}


QtAvInterface::~QtAvInterface()
{
    delete _pMainWindow;
    delete _pMainWidget;
    delete _pBrowserWidget;
    delete _pApp;
    delete _pVisual;
    delete _pControlPanel;
    delete _pBackButton;
    delete _pPlayButton;
    delete _pStopButton;
    delete _pForwardButton;
    delete _pPlayerRackButton;
    delete _pVolumeSlider;
    delete _pSeekSlider;
    delete _pEventFilter;
}


void
QtAvInterface::initGui()
{
    Omm::Av::Log::instance()->upnpav().debug("init qt gui ...");

    qRegisterMetaType<std::string>();

//    _defaultStyleSheet = _pApp->styleSheet();
//    Omm::Av::Log::instance()->upnpav().debug("default style sheet: " + _defaultStyleSheet.toStdString());
//    _defaultStyleSheet +=
//            "* {font-size: 12px}";
//    _defaultStyleSheet +=
//            "* {font-size: 9pt}";
    _fullscreenStyleSheet =
            "*         { font-size: 32pt } \
             QTreeView { background-color: black;\
                         selection-color: white; \
                         selection-background-color: darkblue; } \
             QTreeView { color: white }";

    _pApp = new QApplication(_argc, 0);
    _pEventFilter = new QtEventFilter(this);
    _pApp->installEventFilter(_pEventFilter);
//    _pApp->setStyleSheet(_defaultStyleSheet);
    
    _pMainWidget = new QStackedWidget;
    _pMainWindow = new QtMainWindow(_pMainWidget);

    _pVisual = new QtVisual(_pMainWindow);
    _pVisual->_pWidget->setParent(_pMainWidget);

    _pBrowserWidget = new QtBrowserWidget(_pMainWindow, this);

    _pPlayerRack = new QDockWidget;
    _playerRack.setupUi(_pPlayerRack);
//    _pPlayerRack->setFeatures(QDockWidget::AllDockWidgetFeatures);

    _pMainWidget->addWidget(_pVisual->_pWidget);
    _pMainWidget->addWidget(_pBrowserWidget);
    _pMainWidget->setCurrentWidget(_pBrowserWidget);
    _menuVisible = true;

    _pMainWindow->addDockWidget(Qt::RightDockWidgetArea, _pPlayerRack);
    _pMainWindow->setWindowTitle("OMM");
    
    _pRendererListModel = new QtRendererListModel(this);
    _playerRack._playerListView->setModel(_pRendererListModel);

    _pControlPanel = new QToolBar("ControlPanel", _pMainWindow);
    _pBackButton = new QPushButton(_pMainWindow->style()->standardIcon(QStyle::SP_MediaSkipBackward), "", _pControlPanel);
    _pPlayButton = new QPushButton(_pMainWindow->style()->standardIcon(QStyle::SP_MediaPlay), "", _pControlPanel);
    _pStopButton = new QPushButton(_pMainWindow->style()->standardIcon(QStyle::SP_MediaStop), "", _pControlPanel);
    _pForwardButton = new QPushButton(_pMainWindow->style()->standardIcon(QStyle::SP_MediaSkipForward), "", _pControlPanel);
    _pControlPanel->addWidget(_pBackButton);
    _pControlPanel->addWidget(_pPlayButton);
    _pControlPanel->addWidget(_pStopButton);
    _pControlPanel->addWidget(_pForwardButton);

    _pActivityIndicator = new QtActivityIndicator(_pControlPanel);
    _pControlPanel->addWidget(_pActivityIndicator);

    _pVolumeSlider = new QSlider(Qt::Horizontal, _pControlPanel);
    _pVolumeSlider->setTracking(true);
    _pVolumeSlider->setSingleStep(5);
    _pSeekSlider = new QSlider(Qt::Horizontal, _pControlPanel);
    _pSeekSlider->setSingleStep(10);
    _pSeekSlider->setPageStep(25);

    _pControlPanel->addWidget(_pVolumeSlider);
    _pControlPanel->addWidget(_pSeekSlider);

    _pPlayerRackButton = new QtPlayerRackButton(_pControlPanel);
    _pControlPanel->addWidget(_pPlayerRackButton);

    _pControlPanel->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    _pMainWindow->addToolBar(Qt::BottomToolBarArea, _pControlPanel);

    connect(_pPlayButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(_pStopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
    connect(_pForwardButton, SIGNAL(pressed()), this, SLOT(skipForwardButtonPressed()));
    connect(_pBackButton, SIGNAL(pressed()), this, SLOT(skipBackwardButtonPressed()));

    connect(_pRendererListModel, SIGNAL(setCurrentIndex(QModelIndex)),
            _playerRack._playerListView, SLOT(setCurrentIndex(QModelIndex)));
    connect(_playerRack._playerListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));

    connect(_pVolumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSeekSlider(int, int)));
    connect(this, SIGNAL(volSliderMoved(int)), this, SLOT(setVolumeSlider(int)));

    connect(this, SIGNAL(startNetworkActivity()),
            _pActivityIndicator, SLOT(startActivity()));
    connect(this, SIGNAL(stopNetworkActivity()),
            _pActivityIndicator, SLOT(stopActivity()));
    connect(this, SIGNAL(nowPlaying(const QString&, const QString&, const QString&)),
            this, SLOT(setTrackInfo(const QString&, const QString&, const QString&)));

    connect(_pVisual, SIGNAL(showMenu(bool)), this, SLOT(showMenu(bool)));
    connect(_pPlayerRackButton, SIGNAL(toggled(bool)), this, SLOT(showPlayerRack(bool)));
//    _pPlayerRackButton->addAction(_pPlayerRack->toggleViewAction());

// TODO: starting of local servers should go in controller application.

//    Omm::Util::PluginLoader<Omm::Av::AbstractMediaObject> pluginLoader;
//    Omm::Av::AbstractMediaObject* pContainerPlugin;
//    std::string containerPlugin("server-webradio");
//    std::string home = Omm::Util::Home::getHomePath();
//    std::string name("Web Radio");
//    try {
//        pContainerPlugin = pluginLoader.load(containerPlugin);
//    }
//    catch(Poco::NotFoundException) {
//        std::cerr << "Error could not find server plugin: " << containerPlugin << std::endl;
//        return;
//    }
//    std::clog << "container plugin: " << containerPlugin << " loaded successfully" << std::endl;
//    pContainerPlugin->setOption("basePath", home + "/.omm/webradio.conf");
//    pContainerPlugin->setTitle(name);
//
//    Omm::Av::AvServer* pServer = new Omm::Av::AvServer;
//    pServer->setRoot(pContainerPlugin);
//    pServer->setFriendlyName(name);
//    Omm::Icon* pIcon = new Omm::Icon(32, 32, 8, "image/png", "device.png");
//    pServer->addIcon(pIcon);
//
//    addLocalServer(pServer);
//    startLocalServers();

    Omm::Av::Log::instance()->upnpav().debug("finished init qt gui.");
}


int
QtAvInterface::eventLoop()
{
    return _pApp->exec();
}


void
QtAvInterface::showMainWindow()
{
    _pMainWindow->show();
}


Omm::Sys::Visual*
QtAvInterface::getVisual()
{
    return _pVisual;
}


void
QtAvInterface::beginNetworkActivity()
{
//    Omm::Av::Log::instance()->upnpav().debug("begin network activity");
    emit startNetworkActivity();
}


void
QtAvInterface::endNetworkActivity()
{
//    Omm::Av::Log::instance()->upnpav().debug("end network activity");
    emit stopNetworkActivity();
}


bool
QtAvInterface::menuVisible()
{
    return _menuVisible;
}


bool
QtAvInterface::isFullscreen()
{
    return _fullscreen;
}


void
QtAvInterface::showMenu(bool show)
{
    _menuVisible = show;
    if (show) {
        _pMainWidget->setCurrentWidget(_pBrowserWidget);
    }
    else {
        _pMainWidget->setCurrentWidget(_pVisual->_pWidget);
    }
}


void
QtAvInterface::setFullscreen(bool fullscreen)
{
    _fullscreen = fullscreen;
    if (fullscreen) {
        _pControlPanel->hide();
        showPlayerRack(false);
        _pMainWindow->setCursor(QCursor(Qt::BlankCursor));
        _pApp->setStyleSheet(_fullscreenStyleSheet);
//        _browserWidget._browserView->setAlternatingRowColors(false);
    }
    else {
        _pControlPanel->show();
        if (_pPlayerRackButton->isChecked()) {
            showPlayerRack(true);
        }
        else {
            showPlayerRack(false);
        }
        _pMainWindow->setCursor(QCursor(Qt::ArrowCursor));
//        _pApp->setStyleSheet(_defaultStyleSheet);
        _pApp->setStyleSheet("");
//        _browserWidget._browserView->setAlternatingRowColors(true);
    }
}


void
QtAvInterface::resize(int width, int height)
{
    _pMainWindow->resize(width, height);
    _pVisual->setWidth(width);
    _pVisual->setHeight(height);
}


void
QtAvInterface::showPlayerRack(bool show)
{
    if (show) {
        _pPlayerRack->show();
    }
    else {
        _pPlayerRack->hide();
    }
}


void
QtAvInterface::showControlPanel(bool show)
{
    if (show) {
        _pControlPanel->show();
    }
    else {
        _pControlPanel->hide();
    }
}


void
QtAvInterface::checkSliderMoved(int value)
{
    if (_sliderMoved) {
        _sliderMoved = false;
        emit sliderMoved(value);
    }
}


void
QtAvInterface::setSliderMoved(int)
{
    _sliderMoved = true;
}


void
QtAvInterface::rendererSelectionChanged(const QItemSelection& selected,
                                         const QItemSelection& /*deselected*/)
{
    if (selected.count() > 1) {
        return;
    }
    if (selected.empty()) {
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    Omm::Av::RendererView* selectedRenderer = static_cast<Omm::Av::RendererView*>(index.internalPointer());
    
    if (selectedRenderer == 0) {
        return;
    }
    rendererSelected(selectedRenderer);
    if (isPlaying(selectedRenderer)) {
        _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
        _pPlayButton->setEnabled(true);
        _playToggle = false;
        _pStopButton->setEnabled(true);
    }
    _pPlayerRackButton->setPlayerName(selectedRenderer->getName());
//    _pPlayerRackButton->setText(selectedRenderer->getName().c_str());
}


void
QtAvInterface::setSeekSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_pSeekSlider->isSliderDown()) {
        return;
    }
    _pSeekSlider->setRange(0, max>=0?max:0);
    _pSeekSlider->setSliderPosition(val);
}


void
QtAvInterface::setVolumeSlider(int val)
{
    // don't set slider position when user drags the slider
    if (_pVolumeSlider->isSliderDown()) {
        return;
    }
    _pVolumeSlider->setRange(0, 100);
    _pVolumeSlider->setSliderPosition(val);
}


void
QtAvInterface::setTrackInfo(const QString& title, const QString& artist, const QString& album)
{
//    _rendererWidget._title->setText(title);
//    _rendererWidget._artist->setText(artist);
//    _rendererWidget._album->setText(album);
}


void
QtAvInterface::playButtonPressed()
{
    if (_playToggle) {
        playPressed();
        // TODO: only toggle play button to pause, if media is really playing
        _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
        _playToggle = false;
    }
    else {
        pausePressed();
        _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
        _playToggle = true;
    }
    _pStopButton->setEnabled(true);
    _pForwardButton->setEnabled(true);
    _pBackButton->setEnabled(true);
}


void
QtAvInterface::stopButtonPressed()
{
    stopPressed();
    _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
    _playToggle = true;
    _pStopButton->setEnabled(false);
    _pForwardButton->setEnabled(false);
    _pBackButton->setEnabled(false);
}


void
QtAvInterface::skipForwardButtonPressed()
{
    Omm::Av::Log::instance()->upnpav().debug("skipping to next track in browser list ...");
    QModelIndex current = _pBrowserWidget->getCurrentIndex();
//    QModelIndex current = _browserWidget._browserView->currentIndex();
    if (current.isValid()) {
        Omm::Av::ControllerObject* pCurrentObject = static_cast<Omm::Av::ControllerObject*>(current.internalPointer());
        Omm::Av::Log::instance()->upnpav().debug("current title is: " + pCurrentObject->getTitle());
        QModelIndex next;
        do {
            next = current.sibling(current.row() + 1, 0);
            if (next.isValid()) {
                Omm::Av::ControllerObject* pNextObject = static_cast<Omm::Av::ControllerObject*>(next.internalPointer());
                if (pNextObject->isContainer()) {
                    current = next;
                }
                else {
                    Omm::Av::Log::instance()->upnpav().debug("next title is: " + pNextObject->getTitle());
                    _pBrowserWidget->setCurrentIndex(next);
//                    _browserWidget._browserView->setCurrentIndex(next);
                    mediaObjectSelected(pNextObject);
                    playPressed();
                    _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
                    _playToggle = false;
                    _pStopButton->setEnabled(true);
                }
            }
        } while(next.isValid() && current == next);
    }
}


void
QtAvInterface::skipBackwardButtonPressed()
{
    Omm::Av::Log::instance()->upnpav().debug("skipping to previous track in browser list ...");
    QModelIndex current = _pBrowserWidget->getCurrentIndex();
//    QModelIndex current = _browserWidget._browserView->currentIndex();
    if (current.isValid()) {
        Omm::Av::ControllerObject* pCurrentObject = static_cast<Omm::Av::ControllerObject*>(current.internalPointer());
        Omm::Av::Log::instance()->upnpav().debug("current title is: " + pCurrentObject->getTitle());
        QModelIndex previous;
        do {
            previous = current.sibling(current.row() - 1, 0);
            if (previous.isValid()) {
                Omm::Av::ControllerObject* pPreviousObject = static_cast<Omm::Av::ControllerObject*>(previous.internalPointer());
                if (pPreviousObject->isContainer()) {
                    current = previous;
                }
                else {
                    Omm::Av::Log::instance()->upnpav().debug("previous title is: " + pPreviousObject->getTitle());
                    _pBrowserWidget->setCurrentIndex(previous);
//                    _browserWidget._browserView->setCurrentIndex(previous);
                    mediaObjectSelected(pPreviousObject);
                    playPressed();
                    _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
                    _playToggle = false;
                    _pStopButton->setEnabled(true);
                }
            }
        } while(previous.isValid() && current == previous);
    }
}


void
QtAvInterface::positionSliderMoved(int position)
{
    positionMoved(position);
}


void
QtAvInterface::volumeSliderMoved(int value)
{
    volumeChanged(value);
}


void
QtAvInterface::beginAddRenderer(int position)
{
    _pRendererListModel->beginAddRenderer(position);
}


void
QtAvInterface::beginAddServer(int position)
{
    _pBrowserWidget->beginAddServer(position);
}


void
QtAvInterface::beginRemoveRenderer(int position)
{
    _pRendererListModel->beginRemoveRenderer(position);
}


void
QtAvInterface::beginRemoveServer(int position)
{
    _pBrowserWidget->beginRemoveServer(position);
}


void
QtAvInterface::endAddRenderer(int position)
{
    _pRendererListModel->endAddRenderer();
}


void
QtAvInterface::endAddServer(int position)
{
    _pBrowserWidget->endAddServer();
}


void
QtAvInterface::endRemoveRenderer(int position)
{
    _pRendererListModel->endRemoveRenderer();
}


void
QtAvInterface::endRemoveServer(int position)
{
    _pBrowserWidget->endRemoveServer();
}


void
QtAvInterface::newPosition(int duration, int position)
{
    emit setSlider(duration, position);
}


void
QtAvInterface::newTrack(const std::string& title, const std::string& artist, const std::string& album)
{
    _pPlayerRackButton->setTitleName(title);
    
    emit nowPlaying(QString(title.c_str()), QString(artist.c_str()), QString(album.c_str()));
}


void
QtAvInterface::newVolume(const int volume)
{
    emit volSliderMoved(volume);
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AvUserInterface)
POCO_EXPORT_CLASS(QtAvInterface)
POCO_END_MANIFEST
#endif
