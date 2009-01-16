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

#include "upnpcontroller.h"

#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>
#include <QtCore/QHash>

class QThread;
class QPushButton;
class QLabel;
class QComboBox;
class QSlider;
class QTextEdit;
class QMenu;
class Ui_settings;

class ControllerGui : public QWidget
{
    Q_OBJECT
public:
    ControllerGui(/*UpnpController *controller*/);
    void startControllerThread();

public slots:
//     void rendererAddRemove(QString name, QString uuid, bool add);
    void rendererAddRemove(string name, string uuid, bool add);
    void openFile();
    void pause();
    void play();
    void rewind();
    void forward();
    void setVolume(int);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
//     void handleDrop(QDropEvent *e);
    void setFile(const QString &text);
//     void initVideoWindow();
    void initSettingsDialog();
//     void updateEffect();
    void updateInfo();
    void updateTime();
    void finished();
    void scaleChanged(QAction *);
    void aspectChanged(QAction *);
    void playPause();
    
private slots:
    void setAspect(int);
    void setScale(int);
    void setSaturation(int);
    void setContrast(int);
    void setHue(int);
    void setBrightness(int);
//     void stateChanged(Phonon::State newstate, Phonon::State oldstate);
//     void effectChanged();
    void showSettingsDialog();
    void showContextMenu(const QPoint &);
    void bufferStatus(int percent);
    void openUrl();
//     void configureEffect();
    
private:
    QIcon playIcon;
    QIcon pauseIcon;
    QPixmap volumeIcon;
    QPixmap mutedIcon;
    QMenu *fileMenu;
    QComboBox *rendererCombo;
    QLabel *rendererLabel;
    QPushButton *playButton;
    QPushButton *stopButton;
    QPushButton *rewindButton;
    QPushButton *forwardButton;
//     Phonon::SeekSlider *slider;
    QSlider *slider;
    QLabel *volumeLabel;
    QLabel *timeLabel;
    QLabel *progressLabel;
    QSlider *volume;
    QSlider *m_hueSlider;
    QSlider *m_satSlider;
    QSlider *m_contSlider;
    QLabel *info;
    long duration;
    QHash <QString, QWidget*> propertyControllers;
//     Phonon::Effect *nextEffect;
    QDialog *settingsDialog;
    Ui_settings *ui;
    
//     UpnpControllerThread *m_controllerThread;
    UpnpControllerThread m_controllerThread;
    
//     QWidget m_videoWindow;
//     Phonon::MediaObject m_MediaObject;
//     Phonon::AudioOutput m_AudioOutput;
//     Phonon::VideoWidget *m_videoWidget;
//     Phonon::Path m_audioOutputPath;
};

#endif //CONTROLLERGUI_H