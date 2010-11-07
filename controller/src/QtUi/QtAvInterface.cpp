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
_indicateDuration(250)
{
    _symbolRenderer = new QSvgRenderer(this);
}


QtActivityIndicator::~QtActivityIndicator()
{
    delete _symbolRenderer;
}


void
QtActivityIndicator::startActivity()
{
    _symbolRenderer->load(QString(":/images/circle_purple.svg"));
    update();
    // NOTE: this timer only works when started from a Qt event loop thread.
    // with actions triggered by gui elements, this is ok. User QThread::exec() ?
    QTimer::singleShot(_indicateDuration, this, SLOT(stopActivity()));
}


void
QtActivityIndicator::stopActivity()
{
    _symbolRenderer->load(QString(":/images/circle_grey.svg"));
    update();
}


void
QtActivityIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    _symbolRenderer->render(&painter);
}


QtMainWindow::QtMainWindow(QWidget* pCentralWidget)
{
    setGeometry(0, 0, 800, 480);
    setCentralWidget(pCentralWidget);
}


QtAvInterface::QtAvInterface(int argc) :
_app(argc, (char**)0),
_sliderMoved(false),
_pCurrentServer(0)
{
}


QtAvInterface::QtAvInterface(int argc, char** argv) :
_app(argc, argv),
_sliderMoved(false),
_pServerCrumbButton(0),
_pCurrentServer(0)
{
}


QtAvInterface::~QtAvInterface()
{
    delete _pMainWindow;
    delete _pBrowserWidget;
    delete _pRendererWidget;
}


void
QtAvInterface::initGui()
{
    _pBrowserWidget = new QTabWidget;
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
    _rendererWidget._seekForwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSeekForward));
    _rendererWidget._seekForwardButton->setText("");
    _rendererWidget._seekBackwardButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaSeekBackward));
    _rendererWidget._seekBackwardButton->setText("");

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
    
    connect(_rendererWidget._playButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(_rendererWidget._stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
//     connect(ui._pauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()));
    connect(_rendererWidget._volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_rendererWidget._seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(_rendererWidget._seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
//     connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSlider(int, int)));
//     connect(ui._seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
//     connect(ui._seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    
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
}


int
QtAvInterface::eventLoop()
{
    return _app.exec();
}


void
QtAvInterface::showMainWindow()
{
    _pMainWindow->show();
}


void
QtAvInterface::beginNetworkActivity()
{
    std::clog << "beginNetworkActivity" << std::endl;
    emit startNetworkActivity();
}


void
QtAvInterface::endNetworkActivity()
{
    std::clog << "endNetworkActivity" << std::endl;
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
//     std::clog << "QtAvInterface::browserItemActivated()" << std::endl;
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
//     std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
//     std::clog << "title: " << object->getTitle() << std::endl;
    if (index.parent() == QModelIndex()) {
        _pCurrentServer = object;
    }
    if (object->isContainer()) {
        new QtCrumbButton(_browserWidget._browserView, index, _browserWidget._breadCrump);
    }
    else {
        mediaObjectSelected(object);
        playPressed();
    }
}


void
QtAvInterface::browserItemSelected(const QModelIndex& index)
{
//     std::clog << "QtAvInterface::browserItemSelected()" << std::endl;
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
//     std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
//     std::clog << "title: " << object->getTitle() << std::endl;
    if (index.parent() == QModelIndex()) {
        _pCurrentServer = object;
    }

    if (object->isContainer()) {
        new QtCrumbButton(_browserWidget._browserView, index, _browserWidget._breadCrump);
    }
    else {
        mediaObjectSelected(object);
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
//         std::clog << "UpnpController::rendererSelectionChanged() nothing selected" << std::endl;
//         _pSelectedRenderer = 0;
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    Omm::Av::RendererView* selectedRenderer = static_cast<Omm::Av::RendererView*>(index.internalPointer());
    
    if (selectedRenderer == 0) {
        return;
    }
    rendererSelected(selectedRenderer);
    
    setVolumeSlider(100, 50);
}


void
QtAvInterface::setSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_rendererWidget._seekSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "QtAvInterface::setSlider() to:" << max << val;
    _rendererWidget._seekSlider->setRange(0, max>=0?max:0);
    _rendererWidget._seekSlider->setSliderPosition(val);
}


void
QtAvInterface::setVolumeSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_rendererWidget._volumeSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "QtAvInterface::setSlider() to:" << max << val;
    _rendererWidget._volumeSlider->setRange(0, max>=0?max:0);
    _rendererWidget._volumeSlider->setSliderPosition(val);
}


void
QtAvInterface::playButtonPressed()
{
    playPressed();
}


void
QtAvInterface::stopButtonPressed()
{
    stopPressed();
}


void
QtAvInterface::pauseButtonPressed()
{
    pausePressed();
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


POCO_BEGIN_MANIFEST(Omm::Av::AvUserInterface)
POCO_EXPORT_CLASS(QtAvInterface)
POCO_END_MANIFEST
