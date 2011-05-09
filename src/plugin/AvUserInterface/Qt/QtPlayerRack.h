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

#ifndef QtPlayerRack_INCLUDED
#define	QtPlayerRack_INCLUDED

#include <QDockWidget>
#include <QListView>
#include <QVBoxLayout>


class QtRendererListModel;
class QtAvInterface;

class QtPlayerRack : public QDockWidget
{
    Q_OBJECT

public:
    QtPlayerRack(QtAvInterface* pAvInterface, QWidget* pParent = 0);
    ~QtPlayerRack();

public slots:
    void beginAddRenderer(int position);
    void endAddRenderer();
    void beginRemoveRenderer(int position);
    void endRemoveRenderer();

private slots:
    void rendererSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
    
private:
    QtAvInterface*              _pAvInterface;
    QListView*                  _pPlayerListView;

    QtRendererListModel*        _pRendererListModel;
};


#endif

