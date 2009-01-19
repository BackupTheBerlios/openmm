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
#ifndef CONTROLLERGUI_H
#define CONTROLLERGUI_H

#include "ui_controllergui.h"

#include <QtGui>

class ControllerGui : public QTabWidget
{
    Q_OBJECT

public:
    ControllerGui(QWidget *parent = 0);
    
    void setBrowserTreeItemModel(QAbstractItemModel* model);
    
    QItemSelectionModel *getBrowserTreeSelectionModel() { return ui.m_browserTreeView->selectionModel(); }

signals:
    void playButtonPressed();
    void stopButtonPressed();
    void pauseButtonPressed();
    void sliderMoved(int);
    
public slots:
    void setSlider(int max, int val);
    
private slots:
    void rendererAddedRemoved(QString uuid, QString name, bool add);
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
//     void sliderChange(QAbstractSlider::SliderChange change);
    
    Ui::ControllerGui ui;
    bool m_sliderMoved;
};

#endif //CONTROLLERGUI_H