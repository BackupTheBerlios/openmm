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
#include <QTimer>

#include <Omm/Upnp.h>

#include "QtAvInterface.h"

QtCrumbButton* QtCrumbButton::_pLastCrumbButton = 0;

QtCrumbButton::QtCrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent)
:
QWidget(parent),
_parentLayout(parent->layout()),
_browserView(browserView),
_index(index),
_child(0)
{
    QString label;
    if (index == QModelIndex()) {
        label = "Media";
    }
    else {
        label = index.data(Qt::DisplayRole).toString();
    }
    
//     QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
//     sizePolicy.setHorizontalStretch(0);
//     sizePolicy.setVerticalStretch(0);
//     sizePolicy.setHeightForWidth(_browserRootButton->sizePolicy().hasHeightForWidth());
    
    _boxLayout = new QHBoxLayout(this);
    _boxLayout->setSpacing(0);
    _boxLayout->setMargin(0);
    _boxLayout->setContentsMargins(0, 0, 0, 0);
    _button = new QPushButton(label, this);
    _boxLayout->addWidget(_button);
//     _button->setSizePolicy(sizePolicy);
    _button->setFlat(true);
    _button->setCheckable(false);
    _button->setAutoDefault(false);
    
    if (_pLastCrumbButton) {
        _pLastCrumbButton->setChild(this);
    }
    _pLastCrumbButton = this;
    if (_parentLayout) {
        _parentLayout->addWidget(this);
    }
    connect(_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
    _browserView->setRootIndex(index);
    show();
//     else {  // TODO: want to select the first item without activating it
//         _browserView->selectionModel()->setCurrentIndex(index.child(0, 0), QItemSelectionModel::NoUpdate);
//         _browserView->setCurrentIndex(index.child(0, 0));
//     }
}


void
QtCrumbButton::buttonPressed()
{
    _browserView->setRootIndex(_index);
    if (_child) {
        _browserView->scrollTo(_child->_index, QAbstractItemView::PositionAtTop);
        _browserView->setCurrentIndex(_child->_index);
    }
    _pLastCrumbButton = this;
    deleteChildren();
}


void
QtCrumbButton::deleteChildren()
{
    if (_child) {
        _child->deleteChildren();
        _child->hide();
        if (_parentLayout) {
            _parentLayout->removeWidget(_child);
        }
        delete _child;
        _child = 0;
    }
}


QtCrumbButton::~QtCrumbButton()
{
    delete _button;
}


QtActivityIndicator::QtActivityIndicator(QWidget* parent, Qt::WindowFlags f) :
QWidget(parent, f),
_indicateDuration(250),
_activityInProgress(false),
_indicatorOn(false)
{
    _symbolRenderer = new QSvgRenderer(this);
    _symbolRenderer->load(QString(":/images/circle_grey.svg"));
    update();
    // NOTE: if Qt::WA_OpaquePaintEvent is set, repaint() is faster,
    // but root window shines through transparent parts of the image
//     setAttribute(Qt::WA_OpaquePaintEvent, true);
    _offTimer.setInterval(_indicateDuration);
    _offTimer.setSingleShot(true);
    connect(&_offTimer, SIGNAL(timeout()), this, SLOT(stopIndicator()));
}


QtActivityIndicator::~QtActivityIndicator()
{
    delete _symbolRenderer;
}


void
QtActivityIndicator::startActivity()
{
    setActivityInProgress(true);
    if (!indicatorOn()) {
        setIndicatorOn(true);
        _symbolRenderer->load(QString(":/images/circle_purple.svg"));
//         update();
        repaint();
        Omm::Av::Log::instance()->upnpav().trace("INDICATOR TURNED ON");
        // QWidget::update() is called asynchronously and gets therefore executed later sometimes.
        // Qt docs of slot Widget::update():
        // This function does not cause an immediate repaint; instead it schedules a paint event for processing when Qt returns
        // to the main event loop. This permits Qt to optimize for more speed and less flicker than a call to repaint() does.
        // we can set Qt::WA_OpaquePaintEvent as widget attribute
        // so we use QWidget::repaint() instead:
    }
    else {
        Omm::Av::Log::instance()->upnpav().trace("indicator already on, do nothing");
    }
}


void
QtActivityIndicator::stopActivity()
{
    // NOTE: starting this timer only works when started from a Qt event loop thread.
    // with actions triggered by gui elements, this is ok. User QThread::exec() ?
    setActivityInProgress(false);
    if (indicatorOn() && !_offTimer.isActive()) {
        Omm::Av::Log::instance()->upnpav().trace("turn off indicator after short delay ...");
        _offTimer.start();
    }
    else {
        Omm::Av::Log::instance()->upnpav().trace("indicator already off or timer running, do nothing");
    }
}


void
QtActivityIndicator::stopIndicator()
{
    if (!activityInProgress() && indicatorOn()) {
        _symbolRenderer->load(QString(":/images/circle_grey.svg"));
//         update();
        repaint();
        setIndicatorOn(false);
        Omm::Av::Log::instance()->upnpav().trace("INDICATOR TURNED OFF, no activity in progress anymore");
    }
    else {
        Omm::Av::Log::instance()->upnpav().trace("turn off indicator ignored, activity still in progress or indicator already off");
    }
}


void
QtActivityIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    _symbolRenderer->render(&painter);
}


void
QtActivityIndicator::setActivityInProgress(bool set)
{
    QMutexLocker locker(&_activityInProgressLock);
    if (set) {
        Omm::Av::Log::instance()->upnpav().trace("flag \"activity in progress\" set to true");
    }
    else {
        Omm::Av::Log::instance()->upnpav().trace("flag \"activity in progress\" set to false");
    }
    _activityInProgress = set;
}


bool
QtActivityIndicator::activityInProgress()
{
    QMutexLocker locker(&_activityInProgressLock);
    return _activityInProgress;
}


void
QtActivityIndicator::setIndicatorOn(bool set)
{
    QMutexLocker locker(&_indicatorOnLock);
    if (set) {
        Omm::Av::Log::instance()->upnpav().trace("flag \"indicator on\" set to true");
    }
    else {
        Omm::Av::Log::instance()->upnpav().trace("flag \"indicator on\" set to false");
    }
    _indicatorOn = set;
}


bool
QtActivityIndicator::indicatorOn()
{
    QMutexLocker locker(&_indicatorOnLock);
    return _indicatorOn;
}


QtMainWindow::QtMainWindow(QWidget* pCentralWidget)
{
    setCentralWidget(pCentralWidget);
    resize(800, 480);
}


QtVisual::QtVisual()
{
    _pWidget = new QWidget;
}


QtVisual::~QtVisual()
{
    delete _pWidget;
}


void
QtVisual::show()
{
    _pWidget->show();
}


void
QtVisual::hide()
{
    _pWidget->hide();
}


void*
QtVisual::getWindow()
{
    return (void*)_pWidget->winId();
}


Omm::Sys::Visual::VisualType
QtVisual::getType()
{
#ifdef __LINUX__
    return Omm::Sys::Visual::VTX11;
#else
    return Omm::Sys::Visual::VTNone;
#endif
}


QtAvInterface::QtAvInterface() :
_argc(0),
_pApp(new QApplication(_argc, 0)),
_pServerCrumbButton(0),
_pCurrentServer(0),
_sliderMoved(false),
_playToggle(true),
_pVisual(0)
{
}


QtAvInterface::~QtAvInterface()
{
    delete _pMainWindow;
    delete _pBrowserWidget;
    delete _pRendererWidget;
    delete _pApp;
    delete _pVisual;
}


void
QtAvInterface::initGui()
{
//     _pBrowserWidget = new QTabWidget;
    _pBrowserWidget = new QFrame;
    _browserWidget.setupUi(_pBrowserWidget);
    
    _browserWidget._browserView->setUniformRowHeights(true);
    
    _browserWidget._breadCrumpLayout->setAlignment(Qt::AlignLeft);
    _browserWidget._breadCrumpLayout->setSpacing(0);

    _pRendererWidget = new QDockWidget;
    _rendererWidget.setupUi(_pRendererWidget);
    _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
    _rendererWidget._playButton->setText("");
    _rendererWidget._stopButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaStop));
    _rendererWidget._stopButton->setText("");
//     _rendererWidget._pauseButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaPause));
//     _rendererWidget._pauseButton->setText("");
    _rendererWidget._skipForwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSkipForward));
    _rendererWidget._skipForwardButton->setText("");
    _rendererWidget._skipBackwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    _rendererWidget._skipBackwardButton->setText("");
//     _rendererWidget._seekForwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSeekForward));
//     _rendererWidget._seekForwardButton->setText("");
//     _rendererWidget._seekBackwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSeekBackward));
//     _rendererWidget._seekBackwardButton->setText("");

    _pActivityIndicator = new QtActivityIndicator(_rendererWidget._networkActivity);
    _rendererWidget._activityLayout->addWidget(_pActivityIndicator);
    _pActivityIndicator->show();
    
    _pServerCrumbButton = new QtCrumbButton(_browserWidget._browserView, QModelIndex(), _browserWidget._breadCrump);
    
    _pMainWindow = new QtMainWindow(_pBrowserWidget);
    _pMainWindow->addDockWidget(Qt::RightDockWidgetArea, _pRendererWidget);
    _pMainWindow->setWindowTitle("OMM Controller");
    
    _pRendererListModel = new QtRendererListModel(this);
    _pBrowserModel = new QtBrowserModel(this);
    
    _rendererWidget._rendererListView->setModel(_pRendererListModel);
    _browserWidget._browserView->setModel(_pBrowserModel);
    _browserWidget._browserView->setColumnWidth(0, 200);

    _pVisual= new QtVisual;

    connect(_rendererWidget._playButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(_rendererWidget._stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
//     connect(_rendererWidget._pauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()));
    connect(_rendererWidget._skipForwardButton, SIGNAL(pressed()), this, SLOT(skipForwardButtonPressed()));
    connect(_rendererWidget._skipBackwardButton, SIGNAL(pressed()), this, SLOT(skipBackwardButtonPressed()));
    connect(_rendererWidget._volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_rendererWidget._seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(_rendererWidget._seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSeekSlider(int, int)));
    connect(this, SIGNAL(volSliderMoved(int)), this, SLOT(setVolumeSlider(int)));
    
    connect(_pRendererListModel, SIGNAL(setCurrentIndex(QModelIndex)),
            _rendererWidget._rendererListView, SLOT(setCurrentIndex(QModelIndex)));
    connect(_rendererWidget._rendererListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(_browserWidget._browserView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(browserItemActivated(const QModelIndex&)));
    connect(_browserWidget._browserView, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(browserItemSelected(const QModelIndex&)));
    connect(this, SIGNAL(startNetworkActivity()),
            _pActivityIndicator, SLOT(startActivity()));
    connect(this, SIGNAL(stopNetworkActivity()),
            _pActivityIndicator, SLOT(stopActivity()));
    connect(this, SIGNAL(nowPlaying(const QString&, const QString&, const QString&)),
            this, SLOT(setTrackInfo(const QString&, const QString&, const QString&)));

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
QtAvInterface::browserItemActivated(const QModelIndex& index)
{
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
    if (index.parent() == QModelIndex()) {
        _pCurrentServer = object;
    }
    if (object->isContainer()) {
        new QtCrumbButton(_browserWidget._browserView, index, _browserWidget._breadCrump);
    }
    else {
        mediaObjectSelected(object);
        playPressed();
        _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
        _rendererWidget._playButton->setEnabled(true);
        _playToggle = false;
        _rendererWidget._skipForwardButton->setEnabled(true);
        _rendererWidget._skipBackwardButton->setEnabled(true);
    }
}


void
QtAvInterface::browserItemSelected(const QModelIndex& index)
{
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
    if (index.parent() == QModelIndex()) {
        _pCurrentServer = object;
    }
    
    if (object->isContainer()) {
        new QtCrumbButton(_browserWidget._browserView, index, _browserWidget._breadCrump);
    }
    else {
        mediaObjectSelected(object);
        _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
        _rendererWidget._playButton->setEnabled(true);
        _playToggle = true;
    }
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
        _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
        _rendererWidget._playButton->setEnabled(true);
        _playToggle = false;
        _rendererWidget._stopButton->setEnabled(true);
    }
}


void
QtAvInterface::setSeekSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_rendererWidget._seekSlider->isSliderDown()) {
        return;
    }
    _rendererWidget._seekSlider->setRange(0, max>=0?max:0);
    _rendererWidget._seekSlider->setSliderPosition(val);
}


void
QtAvInterface::setVolumeSlider(int val)
{
    // don't set slider position when user drags the slider
    if (_rendererWidget._volumeSlider->isSliderDown()) {
        return;
    }
    _rendererWidget._volumeSlider->setRange(0, 100);
    _rendererWidget._volumeSlider->setSliderPosition(val);
}


void
QtAvInterface::setTrackInfo(const QString& title, const QString& artist, const QString& album)
{
    _rendererWidget._title->setText(title);
    _rendererWidget._artist->setText(artist);
    _rendererWidget._album->setText(album);
}


void
QtAvInterface::playButtonPressed()
{
    if (_playToggle) {
        playPressed();
        // TODO: only toggle play button to pause, if media is really playing
        _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
        _playToggle = false;
    }
    else {
        pausePressed();
        _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
        _playToggle = true;
    }
    _rendererWidget._stopButton->setEnabled(true);
    _rendererWidget._skipForwardButton->setEnabled(true);
    _rendererWidget._skipBackwardButton->setEnabled(true);
}


void
QtAvInterface::stopButtonPressed()
{
    stopPressed();
    _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
    _playToggle = true;
    _rendererWidget._stopButton->setEnabled(false);
    _rendererWidget._skipForwardButton->setEnabled(false);
    _rendererWidget._skipBackwardButton->setEnabled(false);
}


// void
// QtAvInterface::pauseButtonPressed()
// {
//     pausePressed();
// }


void
QtAvInterface::skipForwardButtonPressed()
{
    Omm::Av::Log::instance()->upnpav().debug("skipping to next track in browser list ...");
    QModelIndex current = _browserWidget._browserView->currentIndex();
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
                    _browserWidget._browserView->setCurrentIndex(next);
                    mediaObjectSelected(pNextObject);
                    playPressed();
                    _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
                    _playToggle = false;
                    _rendererWidget._stopButton->setEnabled(true);
                }
            }
        } while(next.isValid() && current == next);
    }
}


void
QtAvInterface::skipBackwardButtonPressed()
{
    Omm::Av::Log::instance()->upnpav().debug("skipping to previous track in browser list ...");
    QModelIndex current = _browserWidget._browserView->currentIndex();
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
                    _browserWidget._browserView->setCurrentIndex(previous);
                    mediaObjectSelected(pPreviousObject);
                    playPressed();
                    _rendererWidget._playButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
                    _playToggle = false;
                    _rendererWidget._stopButton->setEnabled(true);
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
    _pBrowserModel->beginAddServer(position);
}


void
QtAvInterface::beginRemoveRenderer(int position)
{
    _pRendererListModel->beginRemoveRenderer(position);
}


void
QtAvInterface::beginRemoveServer(int position)
{
    _pBrowserModel->beginRemoveServer(position);
    Omm::Av::ControllerObject* pServerObject = serverRootObject(position);
    if (_pCurrentServer && pServerObject == _pCurrentServer) {
        _pServerCrumbButton->buttonPressed();
    }
}


void
QtAvInterface::endAddRenderer(int position)
{
    _pRendererListModel->endAddRenderer();
}


void
QtAvInterface::endAddServer(int position)
{
    _pBrowserModel->endAddServer();
}


void
QtAvInterface::endRemoveRenderer(int position)
{
    _pRendererListModel->endRemoveRenderer();
}


void
QtAvInterface::endRemoveServer(int position)
{
    _pBrowserModel->endRemoveServer();
}


void
QtAvInterface::newPosition(int duration, int position)
{
    emit setSlider(duration, position);
}


void
QtAvInterface::newTrack(const std::string& title, const std::string& artist, const std::string& album)
{
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
