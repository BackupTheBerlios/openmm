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

#ifndef QtAvInterface_INCLUDED
#define QtAvInterface_INCLUDED

#include <QtGui>
#include <QtSvg/QSvgRenderer>

#include <Omm/UpnpAvController.h>

#include "QtBrowserModel.h"
#include "QtRendererListModel.h"
#include "ui_QtAvInterface.h"


class QtCrumbButton : public QWidget
{
    Q_OBJECT

    friend class QtAvInterface;
public:
    QtCrumbButton(QAbstractItemView* browserView, const QModelIndex& index, QWidget* parent = 0);
    ~QtCrumbButton();
    
    void setChild(QtCrumbButton* child) { _child = child; }
    
    static QtCrumbButton* _pLastCrumbButton;
    
private slots:
    void buttonPressed();
    
private:
    void deleteChildren();
    
    QLayout*           _parentLayout;
    QHBoxLayout*       _boxLayout;
    QPushButton*       _button;
    QAbstractItemView* _browserView;
    const QModelIndex  _index;
    QtCrumbButton*       _child;
};


class QtActivityIndicator : public QWidget
{
    Q_OBJECT
    
public:
    QtActivityIndicator(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~QtActivityIndicator();
    
public slots:
    void activity(bool set);

private:
    void paintEvent(QPaintEvent *event);

    QSvgRenderer* _symbolRenderer;
    bool          _toggle;
};


class QtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtMainWindow(QWidget* pCentralWidget);
};


class QtAvInterface : public QObject, public Omm::Av::AvUserInterface
{
    Q_OBJECT

    friend class QtRendererListModel;
    friend class QtBrowserModel;
    
public:
    QtAvInterface(int argc = 0);
    QtAvInterface(int argc, char** argv);
    virtual ~QtAvInterface();
    
    virtual int eventLoop();
    
    virtual void initGui();
    virtual void showMainWindow();
    virtual void beginNetworkActivity();
    virtual void endNetworkActivity();
    
    
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
    void networkActivity(bool set);
    
private:
    
//     void sliderChange(QAbstractSlider::SliderChange change);
    QtBrowserModel*                     _pBrowserModel;
    QtRendererListModel*                _pRendererListModel;
    
    QApplication                        _app;
    QMainWindow*                        _pMainWindow;
    QFrame*                             _pCentralWidget;
    Ui::ControllerGui                   ui;
    bool                                _sliderMoved;
    QtActivityIndicator*                _pActivityIndicator;
    QtCrumbButton*                      _pServerCrumbButton;
    Omm::Av::ControllerObject*          _pCurrentServer;
    static QtCrumbButton*               _pLastCrumbButton;
};

#endif //CONTROLLERGUI_H
