/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include <QtDebug>

#include "ControllerGui.h"

CrumbButton* CrumbButton::m_lastCrumbButton;

CrumbButton::CrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent)
:
QWidget(parent),
m_parentLayout(parent->layout()),
m_browserView(browserView),
m_index(index),
m_child(NULL)
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
//     sizePolicy.setHeightForWidth(m_browserRootButton->sizePolicy().hasHeightForWidth());
    
    m_boxLayout = new QHBoxLayout(this);
    m_boxLayout->setSpacing(0);
    m_boxLayout->setMargin(0);
    m_boxLayout->setContentsMargins(0, 0, 0, 0);
    m_button = new QPushButton(label, this);
    m_boxLayout->addWidget(m_button);
//     m_button->setSizePolicy(sizePolicy);
    m_button->setFlat(true);
    m_button->setCheckable(false);
    m_button->setAutoDefault(false);
    
    if (m_lastCrumbButton) {
        m_lastCrumbButton->setChild(this);
    }
    m_lastCrumbButton = this;
    if (m_parentLayout) {
        m_parentLayout->addWidget(this);
    }
    connect(m_button, SIGNAL(pressed()), this, SLOT(buttonPressed()));
    m_browserView->setRootIndex(index);
    show();
//     else {  // TODO: want to select the first item without activating it
//         m_browserView->selectionModel()->setCurrentIndex(index.child(0, 0), QItemSelectionModel::NoUpdate);
//         m_browserView->setCurrentIndex(index.child(0, 0));
//     }
}


void
CrumbButton::buttonPressed()
{
    m_browserView->setRootIndex(m_index);
    if (m_child) {
        m_browserView->scrollTo(m_child->m_index, QAbstractItemView::PositionAtTop);
        m_browserView->setCurrentIndex(m_child->m_index);
    }
    m_lastCrumbButton = this;
    deleteChildren();
}


void
CrumbButton::deleteChildren()
{
    if (m_child) {
        m_child->deleteChildren();
        m_child->hide();
        if (m_parentLayout) {
            m_parentLayout->removeWidget(m_child);
        }
        delete m_child;
        m_child = NULL;
    }
}


CrumbButton::~CrumbButton()
{
    delete m_button;
}


ControllerGui::ControllerGui() :
m_app(0, 0),
m_widget(),
m_sliderMoved(false)
{
}

ControllerGui::ControllerGui(int argc, char** argv) :
m_app(argc, argv),
m_widget(),
m_sliderMoved(false)
{
}


void
ControllerGui::initGui()
{
    ui.setupUi(&m_widget);
    ui.m_playButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaPlay));
    ui.m_playButton->setText("");
    ui.m_stopButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaStop));
    ui.m_stopButton->setText("");
    ui.m_pauseButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaPause));
    ui.m_pauseButton->setText("");
    ui.m_skipForwardButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui.m_skipForwardButton->setText("");
    ui.m_skipBackwardButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui.m_skipBackwardButton->setText("");
    ui.m_seekForwardButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.m_seekForwardButton->setText("");
    ui.m_seekBackwardButton->setIcon(m_widget.style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.m_seekBackwardButton->setText("");
    
    ui.m_browserView->setUniformRowHeights(true);
    
    ui.m_breadCrumpLayout->setAlignment(Qt::AlignLeft);
    ui.m_breadCrumpLayout->setSpacing(0);
    new CrumbButton(ui.m_browserView, QModelIndex(), ui.m_breadCrump);
    
    m_widget.setWindowTitle("JammC");
    
//     m_pBrowserModel = new UpnpBrowserModel(m_pServers);
//     m_pRendererListModel = new UpnpRendererListModel(m_pRenderers);
    m_pRendererListModel = new UpnpRendererListModel(this);
    m_pBrowserModel = new UpnpBrowserModel(this);
    
    ui.m_rendererListView->setModel(m_pRendererListModel);
    ui.m_browserView->setModel(m_pBrowserModel);
    
    connect(ui.m_playButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(ui.m_stopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
    connect(ui.m_pauseButton, SIGNAL(pressed()), this, SLOT(pauseButtonPressed()));
    connect(ui.m_volumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(ui.m_seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(ui.m_seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
//     connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSlider(int, int)));
//     connect(ui.m_seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
//     connect(ui.m_seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    
    connect(m_pRendererListModel, SIGNAL(setCurrentIndex(QModelIndex)),
            ui.m_rendererListView, SLOT(setCurrentIndex(QModelIndex)));
    connect(ui.m_rendererListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
    connect(ui.m_browserView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(browserItemActivated(const QModelIndex&)));
    connect(ui.m_browserView, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(browserItemSelected(const QModelIndex&)));
}


int
ControllerGui::eventLoop()
{
    return m_app.exec();
}


void
ControllerGui::showMainWindow()
{
    m_widget.show();
}

void
ControllerGui::checkSliderMoved(int value)
{
    if (m_sliderMoved) {
        m_sliderMoved = false;
        emit sliderMoved(value);
    }
}


void
ControllerGui::setSliderMoved(int)
{
    m_sliderMoved = true;
}


void
ControllerGui::browserItemActivated(const QModelIndex& index)
{
    std::clog << "ControllerGui::browserItemActivated()" << std::endl;
    Jamm::Av::MediaObject* object = static_cast<Jamm::Av::MediaObject*>(index.internalPointer());
    if (object == NULL) {
        return;
    }
    std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
    std::clog << "title: " << object->getTitle() << std::endl;
    std::clog << "res: " << object->getProperty("res") << std::endl;
    
    if (object->isContainer()) {
        new CrumbButton(ui.m_browserView, index, ui.m_breadCrump);
    }
    else {
        mediaObjectSelected(object);
        playPressed();
    }
}


void
ControllerGui::browserItemSelected(const QModelIndex& index)
{
    std::clog << "ControllerGui::browserItemSelected()" << std::endl;
    Jamm::Av::MediaObject* object = static_cast<Jamm::Av::MediaObject*>(index.internalPointer());
    if (object == NULL) {
        return;
    }
    std::clog << "type: " << (object->isContainer() ? "container" : "item") << std::endl;
    std::clog << "title: " << object->getTitle() << std::endl;

    if (object->isContainer()) {
        new CrumbButton(ui.m_browserView, index, ui.m_breadCrump);
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
        std::clog << "UpnpController::rendererSelectionChanged() nothing selected" << std::endl;
//         m_pSelectedRenderer = NULL;
        return;
    }
    
    QModelIndex index = selected.indexes().first();
    
    Jamm::Av::RendererView* selectedRenderer = static_cast<Jamm::Av::RendererView*>(index.internalPointer());
    
    if (selectedRenderer == NULL) {
        return;
    }
    rendererSelected(selectedRenderer);
//     m_pSelectedRenderer = selectedRenderer;
    
    std::clog << "UpnpController::rendererSelectionChanged() row:" << index.row() << std::endl;
    
    setVolumeSlider(100, 50);
}


void
ControllerGui::setSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (ui.m_seekSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "ControllerGui::setSlider() to:" << max << val;
    ui.m_seekSlider->setRange(0, max>=0?max:0);
    ui.m_seekSlider->setSliderPosition(val);
}


void
ControllerGui::setVolumeSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (ui.m_volumeSlider->isSliderDown()) {
        return;
    }
//     qDebug() << "ControllerGui::setSlider() to:" << max << val;
    ui.m_volumeSlider->setRange(0, max>=0?max:0);
    ui.m_volumeSlider->setSliderPosition(val);
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
    m_pRendererListModel->beginAddRenderer(position);
}


void
ControllerGui::beginAddServer(int position)
{
    m_pBrowserModel->beginAddServer(position);
}


void
ControllerGui::beginRemoveRenderer(int position)
{
    m_pRendererListModel->beginRemoveRenderer(position);
}


void
ControllerGui::beginRemoveServer(int position)
{
    m_pBrowserModel->beginRemoveServer(position);
}


void
ControllerGui::endAddRenderer()
{
    m_pRendererListModel->endAddRenderer();
}


void
ControllerGui::endAddServer()
{
    m_pBrowserModel->endAddServer();
}


void
ControllerGui::endRemoveRenderer()
{
    m_pRendererListModel->endRemoveRenderer();
}


void
ControllerGui::endRemoveServer()
{
    m_pBrowserModel->endRemoveServer();
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

POCO_BEGIN_MANIFEST(Jamm::Av::UpnpAvUserInterface)
POCO_EXPORT_CLASS(ControllerGui)
POCO_END_MANIFEST
