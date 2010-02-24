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
#include <Poco/ClassLibrary.h>
#include <Jamm/UpnpController.h>

#include "UpnpBrowserModel.h"
#include "UpnpRendererListModel.h"
#include "ui_ControllerGui.h"

// TODO: left and right arrows
// TODO: tab for toggling between browser and renderer
// TODO: return in broswer should toggle play/stop (pause)


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


class ControllerGui : public QObject, public Jamm::Av::UpnpAvUserInterface
{
    Q_OBJECT

    friend class UpnpRendererListModel;
    friend class UpnpBrowserModel;
    
public:
    ControllerGui();
    ControllerGui(int argc, char** argv);
    
    virtual int eventLoop();
    
    virtual void initGui();
    virtual void showMainWindow();
    // TODO: hideMainWindow();

    
    
//     void setBrowserTreeItemModel(QAbstractItemModel* model);
//     void setRendererListItemModel(QAbstractItemModel* model);
    
//     QItemSelectionModel *getBrowserTreeSelectionModel() { return ui.m_browserView->selectionModel(); }
//     QItemSelectionModel *getRendererListSelectionModel() { return ui.m_rendererListView->selectionModel(); }
    

    
public slots:
    void setSlider(int max, int val);
    void setVolumeSlider(int max, int val);
    
private slots:
    void playButtonPressed();
    void stopButtonPressed();
    void pauseButtonPressed();
    void positionSliderMoved(int position);
    void volumeSliderMoved(int value);
    
//     void activated(const QModelIndex& index);
    
    void rendererSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void browserItemActivated(const QModelIndex& index);
    void browserItemSelected(const QModelIndex& index);
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
    
// signals:
    virtual void beginAddRenderer(int position);
    virtual void beginAddServer(int position);
    virtual void beginRemoveRenderer(int position);
    virtual void beginRemoveServer(int position);
    virtual void endAddRenderer();
    virtual void endAddServer();
    virtual void endRemoveRenderer();
    virtual void endRemoveServer();
    
    virtual void beginAddDevice(int position);
    virtual void beginRemoveDevice(int position);
    virtual void endAddDevice();
    virtual void endRemoveDevice();
    
signals:
    void sliderMoved(int value);
    
private:
    
//     void sliderChange(QAbstractSlider::SliderChange change);
    UpnpBrowserModel*       m_pBrowserModel;
    UpnpRendererListModel*  m_pRendererListModel;
    
    QApplication            m_app;
    QFrame                  m_widget;
    Ui::ControllerGui       ui;
    bool                    m_sliderMoved;
};

#endif //CONTROLLERGUI_H