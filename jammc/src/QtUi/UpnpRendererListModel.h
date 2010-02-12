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

#ifndef UPNPRENDERERLISTMODEL_H
#define UPNPRENDERERLISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QTextCodec>

#include <vector>
using namespace std;

#include <Jamm/Upnp.h>
// #include "UpnpController.h"

// class UpnpController;

class UpnpRendererListModel : public QAbstractItemModel
{
    Q_OBJECT
        
public:
    UpnpRendererListModel(/*UpnpController* mediaController,*/ QObject *parent = 0);
    ~UpnpRendererListModel();
    
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
public slots:
    void beginAddRenderer(int position);
    void beginRemoveRenderer(int position);
    void endAddRenderer();
    void endRemoveRenderer();
    
//     rendererAdded(MediaRendererController* pRenderer);
//     rendererRemoved(MediaRendererController* pRenderer);
//     void rendererAddedRemoved(Jamm::Device* renderer, bool add);
    
private:
//     UpnpController*       m_mediaController;
    QTextCodec*           m_charEncoding;
    // TODO: use a pointer to the renderer device instead of the uuid string
    //       (should be the same as in BrowserModel, where a string takes
    //        too much memory compared to a pointer)
//     vector<Jamm::Device*>       m_rendererList;
};

#endif

