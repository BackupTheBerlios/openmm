/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
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
#include "upnpbrowsermodel.h"

#include <QtDebug>

UpnpBrowserModel::UpnpBrowserModel(QObject *parent)
: QAbstractItemModel(parent)
{
    m_charEncoding = QTextCodec::codecForName("UTF-8");
    m_root = new UpnpObject();
    m_root->m_fetchedAllChildren = true;
    m_lazyRowCount = true;
}


UpnpBrowserModel::~UpnpBrowserModel()
{
}


int
UpnpBrowserModel::rowCount(const QModelIndex &parent) const
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::rowCount() parent objectId:" << object->m_objectId.c_str() << "return rows:" << object->m_children.size();
    // model/view rule: never lie about rowCount -> first browse and determine the real number of children
    // TODO: see source of QDirModel and setLazyChildCount() for more information
    if (!parent.isValid() || m_lazyRowCount) {
        return object->m_children.size();
    }
    return object->fetchChildren();
}


int
UpnpBrowserModel::columnCount(const QModelIndex& /*parent*/) const
{;
    return 1;
}


bool
UpnpBrowserModel::hasChildren(const QModelIndex &parent) const
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::hasChildren() parent objectId:" << object->m_objectId.c_str();
    if (!parent.isValid()) {
        return (object->m_children.size() > 0);
    }
    return object->isContainer();
}


bool
UpnpBrowserModel::canFetchMore(const QModelIndex &parent) const
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::canFetchMore() parent objectId:" << object->m_objectId.c_str();
/*    if (object == m_root && m_root->m_children.size() == 0) {
        return false;
    }*/
    return (!object->m_fetchedAllChildren);
}


void
UpnpBrowserModel::fetchMore(const QModelIndex &parent)
{
    UpnpObject* object = getObject(parent);
    qDebug() << "UpnpBrowserModel::fetchMore() parent objectId:" << object->m_objectId.c_str();
    object->fetchChildren();
    qDebug() << "UpnpBrowserModel::fetchMore() number of children:" << object->m_children.size();
}


QVariant
UpnpBrowserModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.internalPointer() == NULL) {
        qWarning() << "UpnpBrowserModel::data() objectId reference is NULL:";
        return QVariant();
    }
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    UpnpObject* object = getObject(index);
    return m_charEncoding->toUnicode(object->getTitle().c_str());
}


QModelIndex
UpnpBrowserModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    UpnpObject* object = getObject(index);
//     qDebug() << "UpnpBrowserModel::parent() index objectId:" << object->m_objectId.c_str();
    if (!object->m_parent || object->m_parent == m_root) {
        return QModelIndex();
    }
    UpnpObject* grandp = object->m_parent->m_parent;
    if (!grandp) {
        return QModelIndex();
    }
    vector<UpnpObject*>::iterator row;
    row = find(grandp->m_children.begin(), grandp->m_children.end(), object->m_parent);
    if (row != grandp->m_children.end()) {
//         qDebug() << "UpnpBrowserModel::parent() return row:" << (row - grandp->m_children.begin());
        return createIndex(row - grandp->m_children.begin(), 0, (void*)(object->m_parent));
    }

    return QModelIndex();
}


QModelIndex
UpnpBrowserModel::index(int row, int column, const QModelIndex &parent) const
{
    // TODO: why is that needed? Without this, it fetches children only once when expanding an index.
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    UpnpObject* object = getObject(parent);
//     qDebug() << "UpnpBrowserModel::index() parent objectId:" << object->m_objectId.c_str() << "row:" << row;
    // if we can't deliver an index, because m_children.size()-1 < row
    // then fetchMore() is triggered -> return QModelIndex()
    if (row > int(object->m_children.size()) - 1) {
        return QModelIndex();
    }
    return createIndex(row, 0, (void*)(object->m_children[row]));
}


Qt::ItemFlags
UpnpBrowserModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant
UpnpBrowserModel::headerData(int /*section*/, Qt::Orientation /*orientation*/,
                             int /*role*/) const
{
//     if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
//     }
    return "";
}


void
UpnpBrowserModel::serverAddedRemoved(UpnpServer* server, bool add)
{
    qDebug() << "UpnpBrowserModel::serverAddedRemoved()" << (add?"add":"remove") << "server:" << server->getFriendlyName().c_str();
    
    if (add) {
        UpnpObject* d = new UpnpObject();
        d->m_objectId = "0";
        d->m_server = server;
        d->m_parent = m_root;
        m_root->m_children.push_back(d);
    }
    else {
    // TODO: delete the UpnpObject tree of server recursively to avoid a memory leak
        vector<UpnpObject*>::iterator i = m_root->m_children.begin();
        while ((*i)->m_server != server && i != m_root->m_children.end()) {
            ++i;
        }
        m_root->m_children.erase(i);
    }
    // TODO: do a more selective update of the BrowserModel and don't reset the whole model.
    reset();
}
