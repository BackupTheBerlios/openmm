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

#include "controllergui.h"

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


ControllerGui::ControllerGui(QWidget* parent)
: QFrame(parent),
m_sliderMoved(false)
{
    ui.setupUi(this);
    ui.m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.m_playButton->setText("");
    ui.m_stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui.m_stopButton->setText("");
    ui.m_pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui.m_pauseButton->setText("");
    ui.m_skipForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui.m_skipForwardButton->setText("");
    ui.m_skipBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui.m_skipBackwardButton->setText("");
    ui.m_seekForwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.m_seekForwardButton->setText("");
    ui.m_seekBackwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui.m_seekBackwardButton->setText("");
    
    // TODO: add "state logic" to button group, with disabling/enabling ...
/*    QButtonGroup* m_buttonGroup = new QButtonGroup(ui.m_controllerGroup);
    m_buttonGroup->addButton(ui.m_playButton);
    m_buttonGroup->addButton(ui.m_stopButton);
    m_buttonGroup->addButton(ui.m_pauseButton);
    m_buttonGroup->addButton(ui.m_skipForwardButton);
    m_buttonGroup->addButton(ui.m_skipBackwardButton);
    m_buttonGroup->addButton(ui.m_seekForwardButton);
    m_buttonGroup->addButton(ui.m_seekBackwardButton);*/
    
    // Forward some signals from GUI Elements
    connect(ui.m_playButton, SIGNAL(pressed()), SIGNAL(playButtonPressed()));
    connect(ui.m_stopButton, SIGNAL(pressed()), SIGNAL(stopButtonPressed()));
    connect(ui.m_pauseButton, SIGNAL(pressed()), SIGNAL(pauseButtonPressed()));
    connect(ui.m_seekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(ui.m_seekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));
    connect(ui.m_volumeSlider, SIGNAL(sliderMoved(int)), SIGNAL(volSliderMoved(int)));
    
    connect(ui.m_browserView, SIGNAL(activated(const QModelIndex&)), SIGNAL(activated(const QModelIndex&)));
    connect(ui.m_browserView, SIGNAL(activated(const QModelIndex&)), this, SLOT(browserItemActivated(const QModelIndex&)));
//     connect(ui.m_browserRootButton, SIGNAL(pressed()), this, SLOT(browserRootButtonPressed()));
    
    ui.m_browserView->setUniformRowHeights(true);
    
    ui.m_breadCrumpLayout->setAlignment(Qt::AlignLeft);
    ui.m_breadCrumpLayout->setSpacing(0);
    new CrumbButton(ui.m_browserView, QModelIndex(), ui.m_breadCrump);
    
    setWindowTitle("JammC");
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
ControllerGui::setBrowserTreeItemModel(QAbstractItemModel* model)
{
    ui.m_browserView->setModel(model);
}


void
ControllerGui::browserItemActivated(const QModelIndex& index)
{
    qDebug() << "ControllerGui::onActivated";
    if (index.model()->hasChildren(index)) {
        new CrumbButton(ui.m_browserView, index, ui.m_breadCrump);
    }
}


void
ControllerGui::setRendererListItemModel(QAbstractItemModel* model)
{
    ui.m_rendererListView->setModel(model);
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
