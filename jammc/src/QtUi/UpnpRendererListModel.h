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

#include <Jamm/UpnpController.h>

class UpnpRendererListModel : public QAbstractItemModel
{
    Q_OBJECT
        
public:
//     UpnpRendererListModel(Jamm::Container<RendererView>* pRenderers, QObject *parent = 0);
    UpnpRendererListModel(Jamm::Av::UpnpAvUserInterface* pUserInterface, QObject *parent = 0);
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
    
    void beginAddRenderer(int position);
    void beginRemoveRenderer(int position);
    void endAddRenderer();
    void endRemoveRenderer();
    
signals:
    void setCurrentIndex(const QModelIndex & index);
    
private:
//     Jamm::Container<RendererView>*   m_pRenderers;
    Jamm::Av::UpnpAvUserInterface*   m_pUserInterface;
    QTextCodec*                      m_charEncoding;
};

#endif

