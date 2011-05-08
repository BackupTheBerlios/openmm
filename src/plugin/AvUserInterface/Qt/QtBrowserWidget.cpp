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

#include <qt4/QtGui/qtreeview.h>
#include <qt4/QtGui/qboxlayout.h>

#include "QtBrowserWidget.h"


QtCrumbButton* QtCrumbButton::_pLastCrumbButton = 0;

QtCrumbButton::QtCrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent, QtCrumbButton* parentButton)
:
QWidget(parent),
_parentLayout(parent->layout()),
_browserView(browserView),
_index(index),
_child(0),
_parent(parentButton)
{
    QString label;
    if (index == QModelIndex()) {
        label = "Menu";
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
        _parent = _pLastCrumbButton;
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


QtCrumbButton::~QtCrumbButton()
{
    delete _button;
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


QtCrumbPanel::QtCrumbPanel(QWidget* parent) :
QWidget(parent)
{
    _pCrumbButtonLayout = new QHBoxLayout;
    _pCrumbButtonLayout->setAlignment(Qt::AlignLeft);
    _pCrumbButtonLayout->setSpacing(0);
    setLayout(_pCrumbButtonLayout);
}


QtCrumbPanel::~QtCrumbPanel()
{
    delete _pCrumbButtonLayout;
}


QtBrowserWidget::QtBrowserWidget(QWidget* parent, QtAvInterface* pAvInterface) :
QFrame(parent),
_pAvInterface(pAvInterface)
{
    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget browser view ...");
    _pBrowserView = new QTreeView(this);
    _pListItem = new QtListItem(_pBrowserView);
    _pBrowserView->setItemDelegate(_pListItem);
    _pBrowserModel = new QtBrowserModel(pAvInterface);
    _pBrowserView->setModel(_pBrowserModel);
    _pBrowserView->setUniformRowHeights(true);
    _pBrowserView->setAlternatingRowColors(true);
    _pBrowserView->setHeaderHidden(true);
    _pBrowserView->setRootIsDecorated(false);
    _pBrowserView->setItemsExpandable(false);

    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget crumb panel ...");
    _pCrumbPanel = new QtCrumbPanel(this);
    _pCrumbButton = new QtCrumbButton(_pBrowserView, QModelIndex(), _pCrumbPanel);

    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget layout...");
    _pLayout = new QVBoxLayout;
    _pLayout->addWidget(_pCrumbPanel);
    _pLayout->addWidget(_pBrowserView);
    setLayout(_pLayout);

    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget signal connections ...");

    connect(_pBrowserView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(browserItemActivated(const QModelIndex&)));
    connect(_pBrowserView, SIGNAL(pressed(const QModelIndex&)),
            this, SLOT(browserItemSelected(const QModelIndex&)));

    Omm::Av::Log::instance()->upnpav().debug("finished ctor qt browser widget.");
}


QtBrowserWidget::~QtBrowserWidget()
{
    delete _pCrumbPanel;
    delete _pBrowserView;
    delete _pListItem;
    delete _pBrowserModel;
    delete _pLayout;
}


void
QtBrowserWidget::goBack()
{
    QtCrumbButton* pButton = _pCrumbButton->_pLastCrumbButton->_parent;
    if (pButton) {
        pButton->buttonPressed();
    }
}


QModelIndex
QtBrowserWidget::getCurrentIndex()
{
    return _pBrowserView->currentIndex();
}


void
QtBrowserWidget::setCurrentIndex(QModelIndex index)
{
    _pBrowserView->setCurrentIndex(index);
}


void
QtBrowserWidget::browserItemActivated(const QModelIndex& index)
{
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
    if (index.parent() == QModelIndex()) {
        _pCurrentServerRootObject = object;
    }
    if (object->isContainer()) {
//        new QtCrumbButton(_browserWidget._browserView, index, _browserWidget._breadCrump);
        new QtCrumbButton(_pBrowserView, index, _pCrumbButton);
    }
    else {
        _pAvInterface->mediaObjectSelected(object);
        _pAvInterface->playPressed();
//        _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPause));
//        _pPlayButton->setEnabled(true);
//        _pStopButton->setEnabled(true);
//        _playToggle = false;
//        _pForwardButton->setEnabled(true);
//        _pBackButton->setEnabled(true);
    }
}


void
QtBrowserWidget::browserItemSelected(const QModelIndex& index)
{
    Omm::Av::ControllerObject* object = static_cast<Omm::Av::ControllerObject*>(index.internalPointer());
    if (object == 0) {
        return;
    }
    if (index.parent() == QModelIndex()) {
        _pCurrentServerRootObject = object;
    }

    if (object->isContainer()) {
        new QtCrumbButton(_pBrowserView, index, _pCrumbButton);
    }
    else {
        _pAvInterface->mediaObjectSelected(object);
//        _pPlayButton->setIcon(_pBrowserWidget->style()->standardIcon(QStyle::SP_MediaPlay));
//        _pPlayButton->setEnabled(true);
//        _playToggle = true;
    }
}


void
QtBrowserWidget::beginAddServer(int position)
{
    _pBrowserModel->beginAddServer(position);
}


void
QtBrowserWidget::endAddServer()
{
    _pBrowserModel->endAddServer();
}


void
QtBrowserWidget::beginRemoveServer(int position)
{
    _pBrowserModel->beginRemoveServer(position);
    Omm::Av::ControllerObject* pServerRootObject = _pAvInterface->serverRootObject(position);
    if (_pCurrentServerRootObject && pServerRootObject == _pCurrentServerRootObject) {
        _pCrumbButton->buttonPressed();
    }
}


void
QtBrowserWidget::endRemoveServer()
{
    _pBrowserModel->endRemoveServer();
}