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

#include "QtDeviceGroup.h"
#include "QtCrumbButton.h"
#include "QtBrowserModel.h"

QtDeviceGroup::QtDeviceGroup()
{
//    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget browser view ...");
    _pBrowserView = new QTreeView(this);
//    _pListItem = new QtListItem(_pBrowserView);
//    _pBrowserView->setItemDelegate(_pListItem);
//    _pBrowserModel = new QtBrowserModel(pAvInterface);
//    _pBrowserView->setModel(pQtDeviceGroup->_pBrowserModel);
    _pBrowserView->setUniformRowHeights(true);
    _pBrowserView->setHeaderHidden(true);
    _pBrowserView->setRootIsDecorated(false);
    _pBrowserView->setItemsExpandable(false);

//    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget crumb panel ...");
    _pCrumbPanel = new QtCrumbPanel(this);
    _pCrumbButton = new QtCrumbButton(_pBrowserView, QModelIndex(), _pCrumbPanel);

//    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget layout...");
    _pLayout = new QVBoxLayout;
    _pLayout->addWidget(_pCrumbPanel);
    _pLayout->addWidget(_pBrowserView);
    setLayout(_pLayout);

//    Omm::Av::Log::instance()->upnpav().debug("ctor qt browser widget signal connections ...");

//    connect(_pBrowserView, SIGNAL(activated(const QModelIndex&)),
//            this, SLOT(browserItemActivated(const QModelIndex&)));
//    connect(_pBrowserView, SIGNAL(pressed(const QModelIndex&)),
//            this, SLOT(browserItemSelected(const QModelIndex&)));

//    Omm::Av::Log::instance()->upnpav().debug("finished ctor qt browser widget.");
}

