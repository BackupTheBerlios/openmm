/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#include "QtDeviceGroupWidget.h"
#include "QtDeviceGroup.h"


QtDeviceGroupWidget::QtDeviceGroupWidget(QtDeviceGroup* pDeviceGroupModel) :
_pDeviceGroupModel(pDeviceGroupModel)
{
    _pDeviceListView = new QTreeView;
//    _pDeviceListItem = new QtDeviceListItem(_pDeviceListView);
//    _pDeviceListView->setItemDelegate(_pDeviceListItem);
    _pDeviceListView->setModel(_pDeviceGroupModel);
    _pDeviceListView->setUniformRowHeights(true);
    _pDeviceListView->setHeaderHidden(true);
    _pDeviceListView->setRootIsDecorated(false);
    _pDeviceListView->setItemsExpandable(false);

    _pLayout = new QVBoxLayout(this);
    _pLayout->addWidget(_pDeviceListView);

    // activated() is return, click or double click, selected() is click or double click on it.
    connect(_pDeviceListView, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(selectedModelIndex(const QModelIndex&)));
}


QString
QtDeviceGroupWidget::getBrowserTitle()
{
    return ">";
}


QWidget*
QtDeviceGroupWidget::getWidget()
{
    return this;
}


void
QtDeviceGroupWidget::selectDevice(Omm::Device* pDevice, int index)
{

}


void
QtDeviceGroupWidget::selectedModelIndex(const QModelIndex& index)
{
    Omm::Device* pDevice = static_cast<Omm::Device*>(index.internalPointer());
//    _pDeviceGroupModel->selectDevice(pDevice);
}

