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

#include "QtPlayerRack.h"
#include "QtAvInterface.h"
#include "QtRendererListModel.h"


QtPlayerRack::QtPlayerRack(QtAvInterface* pAvInterface, QWidget* pParent) :
QDockWidget(pParent),
_pAvInterface(pAvInterface)
{
    _pPlayerListView = new QListView(this);
    setWidget(_pPlayerListView);
//    setFeatures(QDockWidget::AllDockWidgetFeatures);
    setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWindowTitle("Player Rack");

    _pRendererListModel = new QtRendererListModel(pAvInterface);
    _pPlayerListView->setModel(_pRendererListModel);

    connect(_pRendererListModel, SIGNAL(setCurrentIndex(QModelIndex)),
            _pPlayerListView, SLOT(setCurrentIndex(QModelIndex)));
    connect(_pPlayerListView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(rendererSelectionChanged(const QItemSelection&, const QItemSelection&)));
}


QtPlayerRack::~QtPlayerRack()
{
    delete _pPlayerListView;
}


void
QtPlayerRack::rendererSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    if (selected.count() > 1) {
        return;
    }
    if (selected.empty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();

    Omm::Av::AvRendererView* selectedRenderer = static_cast<Omm::Av::AvRendererView*>(index.internalPointer());
    if (selectedRenderer) {
        _pAvInterface->rendererSelected(selectedRenderer);
    }
}


void
QtPlayerRack::beginAddRenderer(int position)
{
    _pRendererListModel->beginAddRenderer(position);
}


void
QtPlayerRack::endAddRenderer()
{
    _pRendererListModel->endAddRenderer();
}


void
QtPlayerRack::beginRemoveRenderer(int position)
{
    _pRendererListModel->beginRemoveRenderer(position);
}


void
QtPlayerRack::endRemoveRenderer()
{
    _pRendererListModel->endRemoveRenderer();
}
