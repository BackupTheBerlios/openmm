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

#ifndef CONTROLLERGUI_H
#define CONTROLLERGUI_H

#include <QtGui>

#include <Omm/UpnpAvController.h>

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
    
    void setChild(CrumbButton* child) { _child = child; }
    
    static CrumbButton* _lastCrumbButton;
    
private slots:
    void buttonPressed();
    
private:
    void deleteChildren();
    
    QLayout*           _parentLayout;
    QHBoxLayout*       _boxLayout;
    QPushButton*       _button;
    QAbstractItemView* _browserView;
    const QModelIndex  _index;
    CrumbButton*       _child;
};


class QtAvInterface : public QObject, public Omm::Av::UpnpAvUserInterface
{
    Q_OBJECT

    friend class UpnpRendererListModel;
    friend class UpnpBrowserModel;
    
public:
    QtAvInterface(int argc = 0);
    QtAvInterface(int argc, char** argv);
    
    virtual int eventLoop();
    
    virtual void initGui();
    virtual void showMainWindow();
    // TODO: hideMainWindow();

    
    
//     void setBrowserTreeItemModel(QAbstractItemModel* model);
//     void setRendererListItemModel(QAbstractItemModel* model);
    
//     QItemSelectionModel *getBrowserTreeSelectionModel() { return ui._browserView->selectionModel(); }
//     QItemSelectionModel *getRendererListSelectionModel() { return ui._rendererListView->selectionModel(); }
    

    
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
    virtual void endAddRenderer(int position);
    virtual void endAddServer(int position);
    virtual void endRemoveRenderer(int position);
    virtual void endRemoveServer(int position);
    
signals:
    void sliderMoved(int value);
    
private:
    
//     void sliderChange(QAbstractSlider::SliderChange change);
    UpnpBrowserModel*       _pBrowserModel;
    UpnpRendererListModel*  _pRendererListModel;
    
    QApplication            _app;
    QFrame                  _widget;
    Ui::ControllerGui       ui;
    bool                    _sliderMoved;
};

#endif //CONTROLLERGUI_H
