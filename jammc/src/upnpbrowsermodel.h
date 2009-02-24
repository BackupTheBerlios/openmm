/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef UPNPBROWSERMODEL_H
#define UPNPBROWSERMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QTextCodec>
#include <QFileIconProvider>

#include "upnpsyncmediabrowser.h"


class UpnpBrowserModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    UpnpBrowserModel(QObject *parent = 0);
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
    
    UpnpObject* getObject(const QModelIndex &index) const {
        return index.isValid() ? static_cast<UpnpObject*>(index.internalPointer()) : m_root;
    }
    QIcon icon(const QModelIndex &index) const;
    
public slots:
    void serverAddedRemoved(UpnpServer* server, bool add);
    
private:
    UpnpObject*           m_root;
//     bool                  m_lazyRowCount;
    QTextCodec*           m_charEncoding;
    QFileIconProvider*    m_iconProvider;
};

#endif // UPNPBROWSERMODEL_H

