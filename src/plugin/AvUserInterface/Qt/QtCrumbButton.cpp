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
#include "QtCrumbButton.h"


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
        label = ">";
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
    _pCrumbButtonLayout->setMargin(0);
    _pCrumbButtonLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(_pCrumbButtonLayout);
}


QtCrumbPanel::~QtCrumbPanel()
{
    delete _pCrumbButtonLayout;
}
