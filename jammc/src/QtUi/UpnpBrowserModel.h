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

#ifndef UPNPBROWSERMODEL_H
#define UPNPBROWSERMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QTextCodec>
#include <QFileIconProvider>

#include <Jamm/Upnp.h>
#include <Jamm/UpnpAvTypes.h>

#include "../UpnpController.h"
#include "../UpnpAvCtrlImpl.h"

class UpnpBrowserModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    UpnpBrowserModel(UpnpAvUserInterface* pUserInterface, QObject *parent = 0);
    ~UpnpBrowserModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    void fetchMore (const QModelIndex & parent);
    bool canFetchMore (const QModelIndex & parent) const;
    
    Jamm::Av::MediaObject* getObject(const QModelIndex &index) const;
    
    QIcon icon(const QModelIndex &index) const;
    
    void beginAddServer(int position);
    void beginRemoveServer(int position);
    void endAddServer();
    void endRemoveServer();
    
private:
//     Jamm::Container<ServerController>*      m_pServers;
    UpnpAvUserInterface*                    m_pUserInterface;
    QTextCodec*                             m_charEncoding;
    QFileIconProvider*                      m_iconProvider;
//     bool                  m_lazyRowCount;
};

#endif // UPNPBROWSERMODEL_H

