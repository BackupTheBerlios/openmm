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

#ifndef QtStdMediaServer_INCLUDED
#define QtStdMediaServer_INCLUDED

#include <QtGui>

#include <Omm/UpnpAvController.h>

#include "QtNavigable.h"
#include "QtMediaObject.h"


class QtStdMediaContainerItem;
class QtStdMediaServerWidget;


class QtStdMediaServer : public QAbstractItemModel, public QtNavigable, public Omm::Av::CtlMediaServer
{
    Q_OBJECT

    friend class QtStdMediaServerWidget;

public:
    QtStdMediaServer();
    ~QtStdMediaServer();

    // QtNavigable interface
    virtual QString getBrowserTitle();
    virtual QWidget* getWidget();
    
    // QAbstractItemModel
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
    
    QtMediaObject* getObject(const QModelIndex &index) const;

    QIcon icon(const QModelIndex &index) const;

private slots:
    void selectedModelIndex(const QModelIndex& index);

private:
    virtual void initController();
    virtual void selected();
    void selectedMediaObject(Omm::Av::CtlMediaObject* pObject);

    QtStdMediaServerWidget*            _pMediaServerWidget;
    QtStdMediaContainerItem*           _pMediaContainerItem;

    QTextCodec*                        _charEncoding;
    QFileIconProvider*                 _iconProvider;
};


class QtStdMediaServerWidget : public QTreeView
{
    Q_OBJECT

public:
    QtStdMediaServerWidget(QtStdMediaServer* pMediaServer);

private slots:
    void selectedModelIndex(const QModelIndex& index);

private:
    QtStdMediaServer*                  _pMediaServer;
};



#endif

