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

#ifndef QtBrowserWidget_INCLUDED
#define	QtBrowserWidget_INCLUDED

#include <QFrame>

#include "QtAvInterface.h"
#include "QtBrowserModel.h"


class QtCrumbButton : public QWidget
{
    Q_OBJECT

    friend class QtBrowserWidget;

public:
    QtCrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent = 0, QtCrumbButton* parentButton = 0);
    ~QtCrumbButton();

    void setChild(QtCrumbButton* child) { _child = child; }
    void setParent(QtCrumbButton* parent) { _parent = parent; }

    static QtCrumbButton* _pLastCrumbButton;

private slots:
    void buttonPressed();

private:
    void deleteChildren();

    QLayout*            _parentLayout;
    QHBoxLayout*        _boxLayout;
    QPushButton*        _button;
    QAbstractItemView*  _browserView;
    const QModelIndex   _index;
    QtCrumbButton*      _child;
    QtCrumbButton*      _parent;
};


class QtCrumbPanel : public QWidget
{
    Q_OBJECT
    
public:
    QtCrumbPanel(QWidget* parent);
    ~QtCrumbPanel();

private:
    QHBoxLayout*    _pCrumbButtonLayout;
};


class QtBrowserWidget : public QFrame
{
    Q_OBJECT

    friend class QtAvInterface;

public:
    QtBrowserWidget(QWidget* parent, QtAvInterface* pAvInterface);
    ~QtBrowserWidget();

    void goBack();
    QModelIndex getCurrentIndex();
    void setCurrentIndex(QModelIndex index);
    void beginAddServer(int position);
    void endAddServer();
    void beginRemoveServer(int position);
    void endRemoveServer();

private slots:
    void browserItemActivated(const QModelIndex& index);
    void browserItemSelected(const QModelIndex& index);

private:
    Omm::Av::MediaObjectView*      _pCurrentServerRootObject;
    QtAvInterface*                  _pAvInterface;

    QVBoxLayout*                    _pLayout;
    QtBrowserModel*                 _pBrowserModel;
    QtCrumbPanel*                   _pCrumbPanel;
    QtCrumbButton*                  _pCrumbButton;
    QTreeView*                      _pBrowserView;
    QtListItem*                     _pListItem;
};


#endif

