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

#ifndef CONTROLLERGUI_H
#define CONTROLLERGUI_H

#include <QtGui>

#include "UpnpBrowserModel.h"
#include "UpnpRendererListModel.h"
#include "ui_ControllerGui.h"


class CrumbButton : public QWidget
{
    Q_OBJECT

public:
    CrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent = 0);
    ~CrumbButton();
    
    void setChild(CrumbButton* child) { m_child = child; }
    
    static CrumbButton* m_lastCrumbButton;
    
private slots:
    void buttonPressed();
    
private:
    void deleteChildren();
    
    QLayout*           m_parentLayout;
    QHBoxLayout*       m_boxLayout;
    QPushButton*       m_button;
    QAbstractItemView* m_browserView;
    const QModelIndex  m_index;
    CrumbButton*       m_child;
};


class ControllerGui : public QFrame, public UserInterface
{
    Q_OBJECT

public:
    ControllerGui(QWidget *parent = 0);
    
    virtual int eventLoop();
    
    virtual void initGui();
    virtual void showMainWindow();
    
    
    
    void setBrowserTreeItemModel(QAbstractItemModel* model);
    void setRendererListItemModel(QAbstractItemModel* model);
    
    QItemSelectionModel *getBrowserTreeSelectionModel() { return ui.m_browserView->selectionModel(); }
//     QItemSelectionModel *getRendererListSelectionModel() { return ui.m_rendererListView->selectionModel(); }
    
signals:
//     void playButtonPressed();
//     void stopButtonPressed();
//     void pauseButtonPressed();
//     void sliderMoved(int);
//     void volSliderMoved(int);
    
//     void activated(const QModelIndex& index);
    
public slots:
    void setSlider(int max, int val);
    void setVolumeSlider(int max, int val);
    
private slots:
    void rendererSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void browserItemActivated(const QModelIndex& index);
    /*
        QAbstractSlider emits signal valueChanged() when the slider was
        once moved and some time later (a new track is loaded), the range
        changes. This triggers a Seek in the MediaRenderer to the position
        of the last Seek (in the previous track). The following two slots
        make QAbstractSlider only emit valueChanged() when triggered by
        a user action.
        This could be considered a bug and not a feature ...
    */
    void checkSliderMoved(int value);
    void setSliderMoved(int value);
    
private:
    virtual void deviceAdded(Device* pDevice) {}
    virtual void rendererAdded(MediaRendererController* pRenderer) {}
    virtual void serverAdded(MediaServerController* pRenderer) {}
    
    virtual void deviceRemoved(Device* pDevice) {}
    virtual void rendererRemoved(MediaRendererController* pRenderer) {}
    virtual void serverRemoved(MediaServerController* pRenderer) {}
    

//     void sliderChange(QAbstractSlider::SliderChange change);
    UpnpBrowserModel*                                   m_upnpBrowserModel;
    UpnpRendererListModel*                              m_upnpRendererListModel;
    
    Ui::ControllerGui ui;
    bool m_sliderMoved;
};

#endif //CONTROLLERGUI_H