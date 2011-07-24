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

#ifndef QtMediaServerModel_INCLUDED
#define QtMediaServerModel_INCLUDED

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QTextCodec>
#include <QFileIconProvider>
#include <QStyledItemDelegate>

#include <Omm/UpnpAvController.h>

#include "QtMediaObject.h"


class QtListItem : public QStyledItemDelegate
{
public:
    QtListItem(QObject* parent = 0);

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


class QtMediaServerModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    QtMediaServerModel(Omm::Av::CtlMediaObject* pRootObject, QObject *parent = 0);
    ~QtMediaServerModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
//    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
//    void fetchMore (const QModelIndex & parent);
//    bool canFetchMore (const QModelIndex & parent) const;
    
    QtMediaObject* getObject(const QModelIndex &index) const;
    
    QIcon icon(const QModelIndex &index) const;
    
private:
    Omm::Av::CtlMediaObject*                _pRootObject;
    QTextCodec*                             _charEncoding;
    QFileIconProvider*                      _iconProvider;
};

#endif

