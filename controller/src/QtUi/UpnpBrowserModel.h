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

#ifndef UPNPBROWSERMODEL_H
#define UPNPBROWSERMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QTextCodec>
#include <QFileIconProvider>

#include <Omm/UpnpAvTypes.h>
#include <Omm/UpnpController.h>


class UpnpBrowserModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    UpnpBrowserModel(Omm::Av::UpnpAvUserInterface* pUserInterface, QObject *parent = 0);
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
    
    Omm::Av::ControllerObject* getObject(const QModelIndex &index) const;
    
    QIcon icon(const QModelIndex &index) const;
    
    void beginAddServer(int position);
    void beginRemoveServer(int position);
    void endAddServer();
    void endRemoveServer();
    
private:
//     Omm::Container<ServerController>*      _pServers;
    Omm::Av::UpnpAvUserInterface*          _pUserInterface;
    QTextCodec*                             _charEncoding;
    QFileIconProvider*                      _iconProvider;
//     bool                  _lazyRowCount;
};

#endif // UPNPBROWSERMODEL_H

