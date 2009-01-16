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
#include <QtGui>

#define SLIDER_RANGE 8
#define TICKINTERVAL 4

#include "controllergui.h"
#include "ui_settings.h"
#include "upnpcontroller.h"

#include "debug.h"

ControllerGui::ControllerGui(/*UpnpController *controller*/) :
/*m_controller(controller),*/ playButton(0), volumeLabel(0), /*nextEffect(0),*/ settingsDialog(0), ui(0)
{
//     m_controller = new UpnpControllerThread();

    setWindowTitle(" JammC ");
    setContextMenuPolicy(Qt::CustomContextMenu);
//     m_videoWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    QSize buttonSize(34, 28);
    
    QPushButton *openButton = new QPushButton(this);
//     QComboBox *rendererCombo = new QComboBox(this);
    QLabel *rendererLabel = new QLabel(this);
    
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    QPalette bpal;
    QColor arrowcolor = bpal.buttonText().color();
    if (arrowcolor == Qt::black)
        arrowcolor = QColor(80, 80, 80);
    bpal.setBrush(QPalette::ButtonText, arrowcolor);
    openButton->setPalette(bpal);
    
    rewindButton = new QPushButton(this);
    rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    
    forwardButton = new QPushButton(this);
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    forwardButton->setEnabled(false);
    
    playButton = new QPushButton(this);
    playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
    playButton->setIcon(playIcon);
    
    stopButton = new QPushButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setEnabled(false);
    
//     slider = new Phonon::SeekSlider(this);
//     slider->setMediaObject(&m_MediaObject);
    slider = new QSlider(Qt::Horizontal, this);
    volume = new QSlider(Qt::Horizontal, this);
    
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(8, 8, 8, 8);
    
    QHBoxLayout *selectlayout = new QHBoxLayout();
    QHBoxLayout *layout = new QHBoxLayout();
    
    info = new QLabel(this);
    info->setMinimumHeight(70);
    info->setAcceptDrops(false);
    info->setMargin(2);
    info->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    info->setLineWidth(2);
    info->setAutoFillBackground(true);
    
    QPalette palette;
    palette.setBrush(QPalette::WindowText, Qt::white);
#ifndef Q_WS_MAC
    openButton->setMinimumSize(47, buttonSize.height());
    rewindButton->setMinimumSize(buttonSize);
    forwardButton->setMinimumSize(buttonSize);
    playButton->setMinimumSize(buttonSize);
    stopButton->setMinimumSize(buttonSize);
#endif
    info->setStyleSheet("border-image:url(:/images/screen.png) ; border-width:3px");
    info->setPalette(palette);
    info->setText("<center>No media</center>");
    
    volume->setRange(0, 100);
    volume->setValue(100);
    volume->setMinimumWidth(40);
    
    selectlayout->addWidget(openButton);
//     selectlayout->addWidget(rendererCombo);
    selectlayout->addWidget(rendererLabel);
    selectlayout->addStretch();
    layout->addWidget(rewindButton);
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(forwardButton);
    
//     rendererCombo->addItem("foo", "barid");
    
    volumeLabel = new QLabel(this);
    volumeIcon = style()->standardPixmap(QStyle::SP_MediaVolume);
    mutedIcon = style()->standardPixmap(QStyle::SP_MediaVolumeMuted);
    volumeLabel->setPixmap(volumeIcon);
    volume->setFixedWidth(100);
    layout->addStretch();
    layout->addWidget(volumeLabel);
    layout->addWidget(volume);
    
    vLayout->addWidget(info);
//     initVideoWindow();
//     vLayout->addWidget(&m_videoWindow);
    QVBoxLayout *buttonPanelLayout = new QVBoxLayout();
//     m_videoWindow.hide();
    buttonPanelLayout->addLayout(selectlayout);
    buttonPanelLayout->addLayout(layout);
    
    timeLabel = new QLabel(this);
    progressLabel = new QLabel(this);
    QWidget *sliderPanel = new QWidget(this);
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(timeLabel);
    sliderLayout->addWidget(progressLabel);
    sliderLayout->setContentsMargins(0, 0, 0, 0);
    sliderPanel->setLayout(sliderLayout);
    
    buttonPanelLayout->addWidget(sliderPanel);
    buttonPanelLayout->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_MAC
    layout->setSpacing(4);
    buttonPanelLayout->setSpacing(0);
    info->setMinimumHeight(100);
    info->setFont(QFont("verdana", 15));
     // QStyle *flatButtonStyle = new QWindowsStyle;
    openButton->setFocusPolicy(Qt::NoFocus);
     // openButton->setStyle(flatButtonStyle);
     // playButton->setStyle(flatButtonStyle);
     // rewindButton->setStyle(flatButtonStyle);
     // forwardButton->setStyle(flatButtonStyle);
#endif
    QWidget *buttonPanelWidget = new QWidget(this);
    buttonPanelWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    buttonPanelWidget->setLayout(buttonPanelLayout);
    vLayout->addWidget(buttonPanelWidget);
    
    QHBoxLayout *labelLayout = new QHBoxLayout();
    
    vLayout->addLayout(labelLayout);
    setLayout(vLayout);
    
     // Create menu bar:
    fileMenu = new QMenu(this);
    QAction *openFileAction = fileMenu->addAction(tr("Open &File..."));
    QAction *openUrlAction = fileMenu->addAction(tr("Open &Location..."));
    
    fileMenu->addSeparator();
    QMenu *aspectMenu = fileMenu->addMenu(tr("&Aspect ratio"));
    QActionGroup *aspectGroup = new QActionGroup(aspectMenu);
    connect(aspectGroup, SIGNAL(triggered(QAction *)), this, SLOT(aspectChanged(QAction *)));
    aspectGroup->setExclusive(true);
    QAction *aspectActionAuto = aspectMenu->addAction("Auto");
    aspectActionAuto->setCheckable(true);
    aspectActionAuto->setChecked(true);
    aspectGroup->addAction(aspectActionAuto);
    QAction *aspectActionScale = aspectMenu->addAction("Scale");
    aspectActionScale->setCheckable(true);
    aspectGroup->addAction(aspectActionScale);
    QAction *aspectAction16_9 = aspectMenu->addAction("16/9");
    aspectAction16_9->setCheckable(true);
    aspectGroup->addAction(aspectAction16_9);
    QAction *aspectAction4_3 = aspectMenu->addAction("4/3");
    aspectAction4_3->setCheckable(true);
    aspectGroup->addAction(aspectAction4_3);
    
    QMenu *scaleMenu = fileMenu->addMenu(tr("&Scale mode"));
    QActionGroup *scaleGroup = new QActionGroup(scaleMenu);
    connect(scaleGroup, SIGNAL(triggered(QAction *)), this, SLOT(scaleChanged(QAction *)));
    scaleGroup->setExclusive(true);
    QAction *scaleActionFit = scaleMenu->addAction("Fit in view");
    scaleActionFit->setCheckable(true);
    scaleActionFit->setChecked(true);
    scaleGroup->addAction(scaleActionFit);
    QAction *scaleActionCrop = scaleMenu->addAction("Scale and crop");
    scaleActionCrop->setCheckable(true);
    scaleGroup->addAction(scaleActionCrop);
    
    fileMenu->addSeparator();
    QAction *settingsAction = fileMenu->addAction(tr("&Settings..."));
    
     // Setup signal connections:
    
    connect(rewindButton, SIGNAL(clicked()), this, SLOT(rewind()));
     //connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    openButton->setMenu(fileMenu);
    
    connect(playButton, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
    connect(volume, SIGNAL(valueChanged(int)), this, SLOT(setVolume(int)));
     //connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(showSettingsDialog()));
    connect(openUrlAction, SIGNAL(triggered(bool)), this, SLOT(openUrl()));
    connect(openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    
//     connect(m_videoWidget, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenu(const QPoint &)));
//     connect(&m_MediaObject, SIGNAL(metaDataChanged()), this, SLOT(updateInfo()));
//     connect(&m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime()));
//     connect(&m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime()));
//     connect(&m_MediaObject, SIGNAL(finished()), this, SLOT(finished()));
//     connect(&m_MediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(stateChanged(Phonon::State, Phonon::State)));
//     connect(&m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
    
    rewindButton->setEnabled(false);
    playButton->setEnabled(false);
    setAcceptDrops(true);
    
//     m_MediaObject.setTickInterval(250);
    
//     m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
//     Phonon::createPath(&m_MediaObject, m_videoWidget);
    
//     if (!filePath.isEmpty())
//         setFile(filePath);
    resize(minimumSizeHint());
    startControllerThread();
}

// void ControllerGui::stateChanged(Phonon::State newstate, Phonon::State oldstate)
// {
//     Q_UNUSED(oldstate);
//     
//     if (oldstate == Phonon::LoadingState) {
//         m_videoWindow.setVisible(m_MediaObject.hasVideo());
//         info->setVisible(!m_MediaObject.hasVideo());
//         QRect videoHintRect = QRect(QPoint(0, 0), m_videoWindow.sizeHint());
//         QRect newVideoRect = QApplication::desktop()->screenGeometry().intersected(videoHintRect);
//         if (m_MediaObject.hasVideo()){
//              // Flush event que so that sizeHint takes the
//              // recently shown/hidden m_videoWindow into account:
//             qApp->processEvents();
//             resize(sizeHint());
//         } else
//             resize(minimumSize());
//     }
//     
//     switch (newstate) {
//     case Phonon::ErrorState:
//         QMessageBox::warning(this, "Phonon Mediaplayer", m_MediaObject.errorString(), QMessageBox::Close);
//         if (m_MediaObject.errorType() == Phonon::FatalError) {
//             playButton->setEnabled(false);
//             rewindButton->setEnabled(false);
//         } else {
//             m_MediaObject.pause();
//         }
//         break;
//     case Phonon::PausedState:
//     case Phonon::StoppedState:
//         playButton->setIcon(playIcon);
//         if (m_MediaObject.currentSource().type() != Phonon::MediaSource::Invalid){
//             playButton->setEnabled(true);
//             rewindButton->setEnabled(true);
//         } else {
//             playButton->setEnabled(false);
//             rewindButton->setEnabled(false);
//         }
//         break;
//     case Phonon::PlayingState:
//         playButton->setEnabled(true);
//         playButton->setIcon(pauseIcon);
//         if (m_MediaObject.hasVideo())
//             m_videoWindow.show();
//              // Fall through
//     case Phonon::BufferingState:
//         rewindButton->setEnabled(true);
//         break;
//     case Phonon::LoadingState:
//         rewindButton->setEnabled(false);
//         break;
//     }
//     
// }

void ControllerGui::startControllerThread()
{
//     m_controllerThread = new UpnpControllerThread();
//     qRegisterMetaType<QString>("QString");
//     qRegisterMetaType<bool>("bool");
    qRegisterMetaType<string>("string");
    sleep(1); // need to wait for the thread to start!! (TODO: do this properly)
//     connect(m_controllerThread->getController(), SIGNAL(rendererAddedRemoved(QString, QString, bool)), this, SLOT(rendererAddRemove(QString, QString, bool))/*, Qt::QueuedConnection*/);
    connect(m_controllerThread.getController(), SIGNAL(rendererAddedRemoved(string, string, bool)), this, SLOT(rendererAddRemove(string, string, bool)));
//     connect(m_controllerThread, SIGNAL(rendererAddedRemoved(QString, QString, bool)), this, SLOT(rendererAddRemove(QString, QString, bool))/*, Qt::QueuedConnection*/);
    
}

void ControllerGui::rendererAddRemove(string name, string uuid, bool add)
{
//     QMutex m;
    if (add) // add renderer
    {
//         m.lock();
//         rendererCombo->addItem(name, uuid);
        rendererLabel->setText(name.c_str());
//         m.unlock();
        TRACE("ControllerGui::rendererAddRemove added device: %s", name/*.toStdString()*/.c_str());
    } else // remove renderer
    {
//         m.lock();
//         rendererCombo->removeItem(rendererCombo->findData(uuid));
//         m.unlock();
        TRACE("ControllerGui::rendererAddRemove removed device: %s", name/*.toStdString()*/.c_str());
    }
}

void ControllerGui::initSettingsDialog()
{
    settingsDialog = new QDialog(this);
    ui = new Ui_settings();
    ui->setupUi(settingsDialog);
    
    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrightness(int)));
    connect(ui->hueSlider, SIGNAL(valueChanged(int)), this, SLOT(setHue(int)));
    connect(ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(setSaturation(int)));
    connect(ui->contrastSlider , SIGNAL(valueChanged(int)), this, SLOT(setContrast(int)));
    connect(ui->aspectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAspect(int)));
    connect(ui->scalemodeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setScale(int)));
    
//     ui->brightnessSlider->setValue(int(m_videoWidget->brightness() * SLIDER_RANGE));
//     ui->hueSlider->setValue(int(m_videoWidget->hue() * SLIDER_RANGE));
/*    ui->saturationSlider->setValue(int(m_videoWidget->saturation() * SLIDER_RANGE));
    ui->contrastSlider->setValue(int(m_videoWidget->contrast() * SLIDER_RANGE));
    ui->aspectCombo->setCurrentIndex(m_videoWidget->aspectRatio());
    ui->scalemodeCombo->setCurrentIndex(m_videoWidget->scaleMode());
    connect(ui->effectButton, SIGNAL(clicked()), this, SLOT(configureEffect()));*/
    
#ifdef Q_WS_X11
     //Cross fading is not currently implemented in the GStreamer backend
    ui->crossFadeSlider->setVisible(false);
    ui->crossFadeLabel->setVisible(false);
    ui->crossFadeLabel1->setVisible(false);
    ui->crossFadeLabel2->setVisible(false);
    ui->crossFadeLabel3->setVisible(false);
#endif
//     ui->crossFadeSlider->setValue((int)(2 * m_MediaObject.transitionTime() / 1000.0f));
    
     // Insert audio devices:
/*    QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
    for (int i=0; i<devices.size(); i++){
        QString itemText = devices[i].name();
        if (!devices[i].description().isEmpty()) {
            itemText += QString::fromLatin1(" (%1)").arg(devices[i].description());
        }
        ui->deviceCombo->addItem(itemText);
        if (devices[i] == m_AudioOutput.outputDevice())
            ui->deviceCombo->setCurrentIndex(i);
    }*/
    
     // Insert audio effects:
    ui->audioEffectsCombo->addItem("<no effect>");
/*    QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
    Phonon::Effect *currEffect = currEffects.size() ? currEffects[0] : 0;
    QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
    for (int i=0; i<availableEffects.size(); i++){
        ui->audioEffectsCombo->addItem(availableEffects[i].name());
        if (currEffect && availableEffects[i] == currEffect->description())
            ui->audioEffectsCombo->setCurrentIndex(i+1);
    }*/
//     connect(ui->audioEffectsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(effectChanged()));
    
}

// void ControllerGui::effectChanged()
// {
//     int currentIndex = ui->audioEffectsCombo->currentIndex();
//     if (ui->audioEffectsCombo->currentIndex()) {
//         QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
//         Phonon::EffectDescription chosenEffect = availableEffects[ui->audioEffectsCombo->currentIndex() - 1];
//         
//         QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
//         Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
//         
//          // Deleting the running effect will stop playback, it is deleted when removed from path
//         if (nextEffect && !(currentEffect && (currentEffect->description().name() == nextEffect->description().name())))
//             delete nextEffect;
//         
//         nextEffect = new Phonon::Effect(chosenEffect);
//     }
//     ui->effectButton->setEnabled(currentIndex);
// }

void ControllerGui::showSettingsDialog()
{
    if (!settingsDialog)
        initSettingsDialog();
    
//     float oldBrightness = m_videoWidget->brightness();
//     float oldHue = m_videoWidget->hue();
//     float oldSaturation = m_videoWidget->saturation();
//     float oldContrast = m_videoWidget->contrast();
    float oldBrightness = 0.0;
    float oldHue = 0.0;
    float oldSaturation = 0.0;
    float oldContrast = 0.0;
    
//     Phonon::VideoWidget::AspectRatio oldAspect = m_videoWidget->aspectRatio();
//     Phonon::VideoWidget::ScaleMode oldScale = m_videoWidget->scaleMode();
//     int currentEffect = ui->audioEffectsCombo->currentIndex();
    settingsDialog->exec();
    
    if (settingsDialog->result() == QDialog::Accepted){
/*        m_MediaObject.setTransitionTime((int)(1000 * float(ui->crossFadeSlider->value()) / 2.0f));
        QList<Phonon::AudioOutputDevice> devices = Phonon::BackendCapabilities::availableAudioOutputDevices();
        m_AudioOutput.setOutputDevice(devices[ui->deviceCombo->currentIndex()]);
        QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
        QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();*/
        
/*        if (ui->audioEffectsCombo->currentIndex() > 0){
            Phonon::Effect *currentEffect = currEffects.size() ? currEffects[0] : 0;
            if (!currentEffect || currentEffect->description() != nextEffect->description()){
                foreach(Phonon::Effect *effect, currEffects) {
                    m_audioOutputPath.removeEffect(effect);
                    delete effect;
                }
                m_audioOutputPath.insertEffect(nextEffect);
            }
        } else {
            foreach(Phonon::Effect *effect, currEffects) {
                m_audioOutputPath.removeEffect(effect);
                delete effect;
                nextEffect = 0;
            }
        }*/
    } else {
         // Restore previous settings
/*        m_videoWidget->setBrightness(oldBrightness);
        m_videoWidget->setSaturation(oldSaturation);
        m_videoWidget->setHue(oldHue);
        m_videoWidget->setContrast(oldContrast);
        m_videoWidget->setAspectRatio(oldAspect);
        m_videoWidget->setScaleMode(oldScale);
        ui->audioEffectsCombo->setCurrentIndex(currentEffect);*/
    }
}

// void ControllerGui::initVideoWindow()
// {
//     QVBoxLayout *videoLayout = new QVBoxLayout();
//     videoLayout->addWidget(m_videoWidget);
//     videoLayout->setContentsMargins(0, 0, 0, 0);
//     m_videoWindow.setLayout(videoLayout);
//     m_videoWindow.setMinimumSize(100, 100);
// }

// void ControllerGui::updateEffect()
// {
//     for (int k=0 ; k< nextEffect->parameters().size() ; ++k) {
//         Phonon::EffectParameter param = nextEffect->parameters()[k];
//         switch(param.type()) {
//         case QVariant::Int:
//             {
//                 QSpinBox *spin = (QSpinBox*)propertyControllers.value(param.name());
//                 nextEffect->setParameterValue(param, spin->value());
//             }
//             break;
//         case QVariant::Double:
//             if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
//                 QSlider *slider = (QSlider*)propertyControllers.value(param.name());
//                 nextEffect->setParameterValue(param, (double)(slider->value() / (double)SLIDER_RANGE));
//             } else {
//                 QDoubleSpinBox *spin = (QDoubleSpinBox*)propertyControllers.value(param.name());
//                 nextEffect->setParameterValue(param, spin->value());
//             }
//             break;
//         case QVariant::Bool:
//             {
//                 QCheckBox *cb = (QCheckBox*)propertyControllers.value(param.name());
//                 nextEffect->setParameterValue(param, cb->isChecked());
//             }
//             break;
//         case QVariant::String:
//             {
//                 QLineEdit *edit = (QLineEdit*)propertyControllers.value(param.name());
//                 nextEffect->setParameterValue(param, edit->text());
//             }
//             break;
//         default:
//             break;
//         }
//     }
// }
// 
// void ControllerGui::configureEffect()
// {
//     QList<Phonon::Effect *> currEffects = m_audioOutputPath.effects();
//     QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
//     if (ui->audioEffectsCombo->currentIndex() > 0){
//         QList<Phonon::EffectDescription> availableEffects = Phonon::BackendCapabilities::availableAudioEffects();
//         Phonon::EffectDescription chosenEffect = availableEffects[ui->audioEffectsCombo->currentIndex() - 1];
//         
//         QDialog effectDialog;
//         effectDialog.setWindowTitle(tr("Configure effect"));
//         QVBoxLayout *topLayout = new QVBoxLayout(&effectDialog);
//         
//         QLabel *description = new QLabel("<b>Description:</b><br>" + chosenEffect.description(), &effectDialog);
//         description->setWordWrap(true);
//         topLayout->addWidget(description);
//         
//         QScrollArea *scrollArea = new QScrollArea(&effectDialog);
//         topLayout->addWidget(scrollArea);
//         
//         QWidget *scrollWidget = new QWidget(&effectDialog);
//         QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
//         scrollWidget->setMinimumWidth(320);
//         scrollArea->setWidget(scrollWidget);
//         
//         if (nextEffect) {
//             for (int k=0 ; k< nextEffect->parameters().size() ; ++k) {
//                 Phonon::EffectParameter param = nextEffect->parameters()[k];
//                 QHBoxLayout *hlayout = new QHBoxLayout();
//                 QString labelName = param.name();
//                 labelName[0] = labelName[0].toUpper();
//                 hlayout->addWidget(new QLabel("<b>" + labelName + ":</b> "));
//                 if (param.type() == QVariant::Int) {
//                     QSpinBox *spin = new QSpinBox(&effectDialog);
//                     spin->setMinimum(param.minimumValue().toInt());
//                     spin->setMaximum(param.maximumValue().toInt());
//                     QVariant currentValue = nextEffect->parameterValue(param);
//                     spin->setProperty("oldValue", currentValue.toInt());
//                     spin->setValue(currentValue.toInt());
//                     connect(spin, SIGNAL(valueChanged(int)), this, SLOT(updateEffect()));
//                     hlayout->addWidget(spin);
//                     propertyControllers.insert(param.name(), spin);
//                 } else if (param.type() == QVariant::Double) {
//                     if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
//                          //Special case values between -1 and 1.0 to use a slider for improved usability
//                         QSlider *slider = new QSlider(Qt::Horizontal, &effectDialog);
//                         slider->setMinimum(-SLIDER_RANGE);
//                         slider->setMaximum(SLIDER_RANGE);
//                         QVariant currentValue = nextEffect->parameterValue(param);
//                         slider->setProperty("oldValue", currentValue.toDouble());
//                         slider->setValue((int)(SLIDER_RANGE * currentValue.toDouble()));
//                         slider->setTickPosition(QSlider::TicksBelow);
//                         slider->setTickInterval(4);
//                         hlayout->addWidget(slider);
//                         connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateEffect()));
//                         propertyControllers.insert(param.name(), slider);
//                     } else {
//                         QDoubleSpinBox *spin = new QDoubleSpinBox(&effectDialog);
//                         spin->setSingleStep(0.1);
//                         spin->setMinimum(param.minimumValue().toDouble());
//                         spin->setMaximum(param.maximumValue().toDouble());
//                         QVariant currentValue = nextEffect->parameterValue(param);
//                         spin->setProperty("oldValue", currentValue);
//                         spin->setValue(currentValue.toDouble());
//                         connect(spin, SIGNAL(valueChanged(double)), this, SLOT(updateEffect()));
//                         hlayout->addWidget(spin);
//                         propertyControllers.insert(param.name(), spin);
//                     }
//                 } else if (param.type() == QVariant::Bool) {
//                     QCheckBox *cb = new QCheckBox(&effectDialog);
//                     QVariant currentValue = nextEffect->parameterValue(param);
//                     cb->setProperty("oldValue", currentValue);
//                     cb->setChecked(currentValue.toBool());
//                     connect(cb, SIGNAL(stateChanged(int)), this, SLOT(updateEffect()));
//                     hlayout->addWidget(cb);
//                     propertyControllers.insert(param.name(), cb);
//                 } else if (param.type() == QVariant::String) {
//                     QLineEdit *edit = new QLineEdit(&effectDialog);
//                     QVariant currentValue = nextEffect->parameterValue(param);
//                     edit->setProperty("oldValue", currentValue.toString());
//                     edit->setText(currentValue.toString());
//                     connect(edit, SIGNAL(returnPressed()), this, SLOT(updateEffect()));
//                     hlayout->addWidget(edit);
//                     propertyControllers.insert(param.name(), edit);
//                 }
//                 scrollLayout->addLayout(hlayout);
//             }
//             QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &effectDialog);
//             connect(bbox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), &effectDialog, SLOT(accept()));
//             connect(bbox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), &effectDialog, SLOT(reject()));
//             topLayout->addWidget(bbox);
//             
//             scrollWidget->adjustSize();
//             effectDialog.adjustSize();
//             
//             effectDialog.exec();
//             if (effectDialog.result() != QDialog::Accepted){
//                  // Revert any changes
//                 for (int k=0 ; k< nextEffect->parameters().size() ; ++k) {
//                     Phonon::EffectParameter param = nextEffect->parameters()[k];
//                     switch(param.type()) {
//                     case QVariant::Int:
//                         {
//                             QSpinBox *spin = (QSpinBox*)propertyControllers.value(param.name());
//                             nextEffect->setParameterValue(param, spin->property("oldValue").toInt());
//                         }
//                         break;
//                     case QVariant::Double:
//                         if (param.minimumValue() == -1.0 && param.maximumValue() == 1.0) {
//                             QSlider *slider = (QSlider*)propertyControllers.value(param.name());
//                             nextEffect->setParameterValue(param, slider->property("oldValue").toDouble());
//                         } else {
//                             QDoubleSpinBox *spin = (QDoubleSpinBox*)propertyControllers.value(param.name());
//                             nextEffect->setParameterValue(param, spin->property("oldValue").toDouble());
//                         }
//                         break;
//                     case QVariant::Bool:
//                         {
//                             QCheckBox *cb = (QCheckBox*)propertyControllers.value(param.name());
//                             nextEffect->setParameterValue(param, cb->property("oldValue").toBool());
//                         }
//                         break;
//                     case QVariant::String:
//                         {
//                             QLineEdit *edit = (QLineEdit*)propertyControllers.value(param.name());
//                             nextEffect->setParameterValue(param, edit->property("oldValue").toString());
//                         }
//                         break;
//                     default:
//                         break;
//                     }
//                 }
//             }
//             propertyControllers.clear();
//         }
//     }
// }

void ControllerGui::pause()
{
//     m_MediaObject.pause();
}

// void ControllerGui::handleDrop(QDropEvent *e)
// {
//     QList<QUrl> urls = e->mimeData()->urls();
//     if (e->proposedAction() == Qt::MoveAction){
//          // Just add to the que:
//         for (int i=0; i<urls.size(); i++)
//             m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
//     } else {
//          // Create new que:
//         m_MediaObject.clearQueue();
//         if (urls.size() > 0) {
//             QString fileName = urls[0].toLocalFile();
//             QDir dir(fileName);
//             if (dir.exists()) {
//                 dir.setFilter(QDir::Files);
//                 QStringList entries = dir.entryList();
//                 if (entries.size() > 0) {
//                     setFile(fileName + QDir::separator() +  entries[0]);
//                     for (int i=1; i< entries.size(); ++i)
//                         m_MediaObject.enqueue(fileName + QDir::separator() + entries[i]);
//                 }
//             } else {
//                 setFile(fileName);
//                 for (int i=1; i<urls.size(); i++)
//                     m_MediaObject.enqueue(Phonon::MediaSource(urls[i].toLocalFile()));
//             }
//         }
//     }
//     forwardButton->setEnabled(m_MediaObject.queue().size() > 0);
//     play();
// }

void ControllerGui::dropEvent(QDropEvent *e)
{
    if (e->mimeData()->hasUrls() && e->proposedAction() != Qt::LinkAction) {
        e->acceptProposedAction();
//         handleDrop(e);
    } else {
        e->ignore();
    }
}

void ControllerGui::dragEnterEvent(QDragEnterEvent *e)
{
    dragMoveEvent(e);
}

void ControllerGui::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        if (e->proposedAction() == Qt::CopyAction || e->proposedAction() == Qt::MoveAction){
            e->acceptProposedAction();
        }
    }
}

void ControllerGui::playPause()
{
/*    if (m_MediaObject.state() == Phonon::PlayingState)
        m_MediaObject.pause();
    else {
        if (m_MediaObject.currentTime() == m_MediaObject.totalTime())
            m_MediaObject.seek(0);
        m_MediaObject.play();
    }*/
}

void ControllerGui::play()
{
//     m_MediaObject.play();
}

void ControllerGui::setVolume(int volume)
{
    Q_UNUSED(volume);
/*    m_AudioOutput.setVolume(volume/100.0f);
    if (volumeLabel) {
        if (volume == 0)
            volumeLabel->setPixmap(mutedIcon);
        else
            volumeLabel->setPixmap(volumeIcon);
    }*/
}

void ControllerGui::setFile(const QString &fileName)
{
/*    setWindowTitle(fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1));
    m_MediaObject.setCurrentSource(Phonon::MediaSource(fileName));
    m_MediaObject.play();*/
}

void ControllerGui::openFile()
{
/*    QStringList fileNames = QFileDialog::getOpenFileNames(this);
    m_MediaObject.clearQueue();
    if (fileNames.size() > 0) {
        QString fileName = fileNames[0];
        setFile(fileName);
        for (int i=1; i<fileNames.size(); i++)
            m_MediaObject.enqueue(Phonon::MediaSource(fileNames[i]));
    }
    forwardButton->setEnabled(m_MediaObject.queue().size() > 0);*/
}

void ControllerGui::bufferStatus(int percent)
{
    if (percent == 0 || percent == 100)
        progressLabel->setText("");
    else {
        QString str("(");
        str += QString::number(percent);
        str += "%)";
        progressLabel->setText(str);
    }
}

void ControllerGui::setSaturation(int val)
{
//     m_videoWidget->setSaturation(val / qreal(SLIDER_RANGE));
}

void ControllerGui::setHue(int val)
{
//     m_videoWidget->setHue(val / qreal(SLIDER_RANGE));
}

void ControllerGui::setAspect(int val)
{
//     m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio(val));
}

void ControllerGui::setScale(int val)
{
//     m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleMode(val));
}

void ControllerGui::setBrightness(int val)
{
//     m_videoWidget->setBrightness(val / qreal(SLIDER_RANGE));
}

void ControllerGui::setContrast(int val)
{
//     m_videoWidget->setContrast(val / qreal(SLIDER_RANGE));
}

void ControllerGui::updateInfo()
{
    int maxLength = 30;
    QString font = "<font color=#ffeeaa>";
    QString fontmono = "<font family=\"monospace,courier new\" color=#ffeeaa>";
    
//     QMap <QString, QString> metaData = m_MediaObject.metaData();
    QMap <QString, QString> metaData /*= m_MediaObject.metaData()*/;
/*    QString trackArtist = metaData.value("ARTIST");
    if (trackArtist.length() > maxLength)
        trackArtist = trackArtist.left(maxLength) + "...";
    
    QString trackTitle = metaData.value("TITLE");
    int trackBitrate = metaData.value("BITRATE").toInt();*/
    
    QString fileName = "foo";
/*    if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) {
        fileName = m_MediaObject.currentSource().url().toString();
    } else {
        fileName = m_MediaObject.currentSource().fileName();
        fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);
        if (fileName.length() > maxLength)
            fileName = fileName.left(maxLength) + "...";
    }*/
    
//     QString title;
//     if (!trackTitle.isEmpty()) {
//         if (trackTitle.length() > maxLength)
//             trackTitle = trackTitle.left(maxLength) + "...";
//         title = "Title: " + font + trackTitle + "<br></font>";
//     } else if (!fileName.isEmpty()) {
//         if (fileName.length() > maxLength)
//             fileName = fileName.left(maxLength) + "...";
//         title = font + fileName + "</font>";
//         if (m_MediaObject.currentSource().type() == Phonon::MediaSource::Url) {
//             title.prepend("Url: ");
//         } else {
//             title.prepend("File: ");
//         }
//     }
//     
//     QString artist;
//     if (!trackArtist.isEmpty())
//         artist = "Artist:  " + font + trackArtist + "</font>";
//     
//     QString bitrate;
//     if (trackBitrate != 0)
//         bitrate = "<br>Bitrate:  " + font + QString::number(trackBitrate/1000) + "kbit</font>";
//     
//     info->setText(title + artist + bitrate);
}

void ControllerGui::updateTime()
{
//     long len = m_MediaObject.totalTime();
//     long pos = m_MediaObject.currentTime();
    long len = 0;
    long pos = 0;
    QString timeString;
    if (pos || len)
    {
        int sec = pos/1000;
        int min = sec/60;
        int hour = min/60;
        int msec = pos;
        
        QTime playTime(hour%60, min%60, sec%60, msec%1000);
        sec = len / 1000;
        min = sec / 60;
        hour = min / 60;
        msec = len;
        
        QTime stopTime(hour%60, min%60, sec%60, msec%1000);
        QString timeFormat = "m:ss";
        if (hour > 0)
            timeFormat = "h:mm:ss";
        timeString = playTime.toString(timeFormat);
        if (len)
            timeString += " / " + stopTime.toString(timeFormat);
    }
    timeLabel->setText(timeString);
}

void ControllerGui::rewind()
{
/*    m_MediaObject.setTickInterval(50);
    m_MediaObject.seek(0);*/
}

void ControllerGui::forward()
{
/*    QList<Phonon::MediaSource> queue = m_MediaObject.queue();
    if (queue.size() > 0) {
        m_MediaObject.setCurrentSource(queue[0]);
        forwardButton->setEnabled(queue.size() > 1);
        m_MediaObject.play();
    }*/
}

void ControllerGui::openUrl()
{
/*    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QString sourceURL = settings.value("location").toString();
    sourceURL = QInputDialog::getText(this, tr("Open Location"), tr("Please enter a valid address here:"), QLineEdit::Normal, sourceURL);
    if (!sourceURL.isEmpty()) {
        setWindowTitle(sourceURL.right(sourceURL.length() - sourceURL.lastIndexOf('/') - 1));
        m_MediaObject.setCurrentSource(Phonon::MediaSource(QUrl::fromEncoded(sourceURL.toUtf8())));
        m_MediaObject.play();
        settings.setValue("location", sourceURL);
    }*/
}

void ControllerGui::finished()
{
}

void ControllerGui::showContextMenu(const QPoint &p)
{
//     fileMenu->popup(m_videoWidget->isFullScreen() ? p : mapToGlobal(p));
}

void ControllerGui::scaleChanged(QAction *act)
{
/*    if (act->text() == "Scale and crop")
        m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
    else
        m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);*/
}

void ControllerGui::aspectChanged(QAction *act)
{
/*    if (act->text() == "16/9")
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
    else if (act->text() == "Scale")
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
    else if (act->text() == "4/3")
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
    else
        m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);*/
}