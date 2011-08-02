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

#ifndef QtWidgetList_INCLUDED
#define QtWidgetList_INCLUDED

#include <QtGui>

class QtWidgetListModel;
class QtWidgetListView;

class QtWidgetListModel : public QAbstractItemModel
{
    Q_OBJECT

    friend class QtWidgetListView;
    
public:
    QtWidgetListModel();

    virtual QWidget* createWidget() { return 0; }
    virtual QWidget* getWidget(int row) { return 0; }
    virtual void attachWidget(int row, QWidget* pWidget) {}
    virtual void detachWidget(int row) {}

    void insertWidget(int row);
    void removeWidget(int row);

//signals:
private:
    QtWidgetListView*               _pView;
//    void insertRow(int row, bool begin);
//    virtual QWidget* createWidget(const QModelIndex& parent = QModelIndex()) { return 0; }
//    virtual QWidget* getWidget(const QModelIndex& index) { return 0; }
//    virtual void attachWidget(const QModelIndex& index, QWidget* pWidget) {}
//    virtual void detachWidget(const QModelIndex& index) {}
};


class QtWidgetListView
{
public:
    QtWidgetListView();
    
    void setModel(QtWidgetListModel* pModel);

    virtual void addWidget(QWidget* pWidget) {}
    virtual void insertWidget(int row) {}
    virtual void removeWidget(int row) {}

protected:
    QtWidgetListModel*              _pModel;
    std::vector<QWidget*>           _widgetPool;
};


class QtWidgetList : public QGraphicsView, public QtWidgetListView
{
    Q_OBJECT
        
public:
    QtWidgetList(QWidget* pParent = 0);
    virtual ~QtWidgetList();

    virtual void addWidget(QWidget* pWidget);
    virtual void insertWidget(int row);
    virtual void removeWidget(int row);

private:
    QGraphicsScene*                         _pGraphicsScene;
    std::vector<QGraphicsProxyWidget*>      _proxyWidgets;
    int                                     _widgetHeight;

//private slots:
//    void rowsAboutToBeInserted(const QModelIndex& parent, int start, int end);

//    void addWidget(QWidget* pWidget);
//    QWidget* getWidget();
//
//    void showWidget(QWidget* pWidget);
//
//    // QtDeviceGroup interface
//    virtual QWidget* getDeviceGroupWidget();

};


#endif

