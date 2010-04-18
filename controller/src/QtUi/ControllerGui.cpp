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

#include <QtDebug>
#include <Omm/UpnpController.h>

#include "ControllerGui.h"

CrumbButton* CrumbButton::_lastCrumbButton;

CrumbButton::CrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent)
:
QWidget(parent),
_parentLayout(parent->layout()),
_browserView(browserView),
_index(index),
_child(NULL)
{
    QString label;
    if (index == QModelIndex()) {
        label = "Servers";
    }
    else {
        label = index.data(Qt::DisplayRole).toString();
    }
    
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
    
    if (_lastCrumbButton) {
        _lastCrumbButton->setChild(this);
    }
    _lastCrumbButton = this;
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
CrumbButton::buttonPressed()
{
    _browserView->setRootIndex(_index);
    if (_child) {
        _browserView->scrollTo(_child->_index, QAbstractItemView::PositionAtTop);
        _browserView->setCurrentIndex(_child->_index);
    }
    _lastCrumbButton = this;
    deleteChildren();
}


void
CrumbButton::deleteChildren()
{
    if (_child) {
        _child->deleteChildren();
        _child->hide();
        if (_parentLayout) {
            _parentLayout->removeWidget(_child);
        }
        delete _child;
        _child = NULL;
    }
}


CrumbButton::~CrumbButton()
{
    delete _button;
}


ControllerGui::ControllerGui() :
_app(0, 0),
_widget(),
_sliderMoved(false)
{
}

ControllerGui::ControllerGui(int argc, char** argv) :
_app(argc, argv),
_widget(),
_sliderMoved(false)
{
}


void
ControllerGui::initGui()
{
    ui.setupUi(&_widget);
    ui._playButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaPlay));
    ui._playButton->setText("");
    ui._stopButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaStop));
    ui._stopButton->setText("");
    ui._pauseButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaPause));
    ui._pauseButton->setText("");
    ui._skipForwardButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui._skipForwardButton->setText("");
    ui._skipBackwardButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui._skipBackwardButton->setText("");
    ui._seekForwardButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui._seekForwardButton->setText("");
    ui._seekBackwardButton->setIcon(_widget.style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui._seekBackwardButton->setText("");
    
    ui._browserView->setUniformRowHeights(true);
    
    ui._breadCrumpLayout->setAlignment(Qt::AlignLeft);
    ui._breadCrumpLayout->setSpacing(0);
    new CrumbButton(ui._browserView, QModelIndex(), ui._breadCrump);
    
    _widget.setWindowTitle("OmmC");
    
//     _pBrowserModel = new UpnpBrowserModel(_pServers);
//     _pRendererListModel = new UpnpRendererListModel(_pRenderers);
    _pRendererListModel = new UpnpRendererListModel(this);
    _pBrowserModel = new UpnpBrowserModel(this);
    
    ui._rendererListView->setModel(_pRendererListModel);
    ui._browserView->setModel(_pBrowserModel);
    
    connect(ui._playButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(ui._stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
    connect(ui._pauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()));
    connect(ui._volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(ui._seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(ui._seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
//     connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSlider(int, int)));
//     connect(ui._seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
//     connect(ui._seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    
    connect(_pRendererListModel, SIGNAL(setCurrentIndex(QModelIndex)),
            ui._rendererListView, SLOT(setCurrentIndex(QModelIndex)));
    connect(ui._rendererListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(ui._browserView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(browserItemActivated(const QModelIndex&)));
    connect(ui._browserView, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(browserItemSelected(const QModelIndex&)));
}


int
ControllerGui::eventLoop()
{
    return _app.exec();
}


void
ControllerGui::showMainWindow()
{
    _widget.show();
}

void
ControllerGui::checkSliderMoved(int value)
{
    if (_sliderMoved) {
        _sliderMoved = false;
        emit sliderMoved(value);
    }
}


void
ControllerGui::setSliderMoved(int)
{
    _sliderMoved = true;
}


void
ControllerGui::browserItemActivated(const QModelIndex& index)
{
//     std::clog << "ControllerGui::browserItemActivated()" << std::endl;
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == NULL) {
        return;
    }
//     std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
//     std::clog << "title: " << object->getTitle() << std::endl;
    
    if (object->isContainer()) {
        new CrumbButton(ui._browserView, index, ui._breadCrump);
    }
    else {
        mediaObjectSelected(object);
        playPressed();
    }
}


void
ControllerGui::browserItemSelected(const QModelIndex& index)
{
//     std::clog << "ControllerGui::browserItemSelected()" << std::endl;
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == NULL) {
        return;
    }
//     std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
//     std::clog << "title: " << object->getTitle() << std::endl;

    if (object->isContainer()) {
        new CrumbButton(ui._browserView, index, ui._breadCrump);
    }
    else {
        mediaObjectSelected(object);
    }
}


void
ControllerGui::rendererSelectionChanged(const QItemSelection& selected,
                                         const QItemSelection& /*deselected*/)
{
    if (selected.count() > 1) {
        return;
    }
    if (selected.empty()) {
//         std::clog << "UpnpController::rendererSelectionChanged() nothing selected" << std::endl;
//         _pSelectedRenderer = NULL;
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    Omm::Av::RendererView* selectedRenderer = static_cast<Omm::Av::RendererView*>(index.internalPointer());
    
    if (selectedRenderer == NULL) {
        return;
    }
    rendererSelected(selectedRenderer);
//     _pSelectedRenderer = selectedRenderer;
    
//     std::clog << "UpnpController::rendererSelectionChanged() row:" << index.row() << std::endl;
    
    setVolumeSlider(100, 50);
}


void
ControllerGui::setSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (ui._seekSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "ControllerGui::setSlider() to:" << max << val;
    ui._seekSlider->setRange(0, max>=0?max:0);
    ui._seekSlider->setSliderPosition(val);
}


void
ControllerGui::setVolumeSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (ui._volumeSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "ControllerGui::setSlider() to:" << max << val;
    ui._volumeSlider->setRange(0, max>=0?max:0);
    ui._volumeSlider->setSliderPosition(val);
}


void
ControllerGui::playButtonPressed()
{
    playPressed();
}


void
ControllerGui::stopButtonPressed()
{
    stopPressed();
}


void
ControllerGui::pauseButtonPressed()
{
    pausePressed();
}


void
ControllerGui::positionSliderMoved(int position)
{
    positionMoved(position);
}


void
ControllerGui::volumeSliderMoved(int value)
{
    volumeChanged(value);
}


void
ControllerGui::beginAddRenderer(int position)
{
    _pRendererListModel->beginAddRenderer(position);
}


void
ControllerGui::beginAddServer(int position)
{
    _pBrowserModel->beginAddServer(position);
}


void
ControllerGui::beginRemoveRenderer(int position)
{
    _pRendererListModel->beginRemoveRenderer(position);
}


void
ControllerGui::beginRemoveServer(int position)
{
    _pBrowserModel->beginRemoveServer(position);
}


void
ControllerGui::endAddRenderer()
{
    _pRendererListModel->endAddRenderer();
}


void
ControllerGui::endAddServer()
{
    _pBrowserModel->endAddServer();
}


void
ControllerGui::endRemoveRenderer()
{
    _pRendererListModel->endRemoveRenderer();
}


void
ControllerGui::endRemoveServer()
{
    _pBrowserModel->endRemoveServer();
}


void
ControllerGui::beginAddDevice(int position)
{
}


void
ControllerGui::beginRemoveDevice(int position)
{
}


void
ControllerGui::endAddDevice()
{
}


void
ControllerGui::endRemoveDevice()
{
}

POCO_BEGIN_MANIFEST(Omm::Av::UpnpAvUserInterface)
POCO_EXPORT_CLASS(ControllerGui)
POCO_END_MANIFEST
