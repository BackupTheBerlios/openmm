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

#include "QtMediaRendererControlPanel.h"


QtNowPlaying::QtNowPlaying(QWidget* pParent) :
QPushButton(pParent)
{
    setMinimumWidth(250);
    setCheckable(true);
}


void
QtNowPlaying::setPlayerName(const std::string& name)
{
    _playerName = name;
    setLabel();
}


void
QtNowPlaying::setTitleName(const std::string& name)
{
    _titleName = name;
    setLabel();
}


void
QtNowPlaying::setLabel()
{
    if (_titleName == "") {
        setText(_playerName.c_str());
    }
    else {
        setText((_playerName + " - " + _titleName).c_str());
    }
}


QtMediaRendererControlPanel::QtMediaRendererControlPanel() :
QToolBar("ControlPanel")
{
    _pBackButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "", this);
    _pPlayButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "", this);
    _pStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaStop), "", this);
    _pForwardButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "", this);

    addWidget(_pBackButton);
    addWidget(_pPlayButton);
    addWidget(_pStopButton);
    addWidget(_pForwardButton);

    _pVolumeSlider = new QSlider(Qt::Horizontal, this);
    _pVolumeSlider->setTracking(true);
    _pVolumeSlider->setSingleStep(5);
    _pSeekSlider = new QSlider(Qt::Horizontal, this);
    _pSeekSlider->setSingleStep(10);
    _pSeekSlider->setPageStep(25);

    addWidget(_pVolumeSlider);
    addWidget(_pSeekSlider);

    _pPlayerRackButton = new QtNowPlaying(this);
    addWidget(_pPlayerRackButton);

    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    connect(_pPlayButton, SIGNAL(pressed()), this, SIGNAL(playButtonPressed()));
    connect(_pStopButton, SIGNAL(pressed()), this, SIGNAL(stopButtonPressed()));
//    connect(_pForwardButton, SIGNAL(pressed()), _pAvInterface, SLOT(skipForwardButtonPressed()));
//    connect(_pBackButton, SIGNAL(pressed()), _pAvInterface, SLOT(skipBackwardButtonPressed()));
//
    connect(_pVolumeSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(volumeSliderMoved(int)));

    connect(_pSeekSlider, SIGNAL(sliderMoved(int)), this, SIGNAL(positionSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));

//    connect(this, SIGNAL(posSliderMoved(int)), this, SLOT(positionSliderMoved(int)));
//    connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSeekSlider(int, int)));
//    connect(this, SIGNAL(volSliderMoved(int)), this, SLOT(setVolumeSlider(int)));
//
//    connect(_pAvInterface, SIGNAL(startNetworkActivity()), _pActivityIndicator, SLOT(startActivity()));
//    connect(_pAvInterface, SIGNAL(stopNetworkActivity()), _pActivityIndicator, SLOT(stopActivity()));
//
//    connect(_pAvInterface, SIGNAL(nowPlaying(const QString&, const QString&, const QString&)),
//            this, SLOT(setTrackInfo(const QString&, const QString&, const QString&)));
}


QtMediaRendererControlPanel::~QtMediaRendererControlPanel()
{
    delete _pBackButton;
    delete _pPlayButton;
    delete _pStopButton;
    delete _pForwardButton;
    delete _pPlayerRackButton;
    delete _pVolumeSlider;
    delete _pSeekSlider;
}


void
QtMediaRendererControlPanel::setPlayerName(const std::string& name)
{
    _pPlayerRackButton->setPlayerName(name);
}


//void
//QtMediaRendererControlPanel::playButtonPressed()
//{
//    if (_playToggle) {
//        if (_pAvInterface) {
//            _pAvInterface->playPressed();
//        }
//        else {
////            _pApplication->playPressed();
//        }
//        // TODO: only toggle play button to pause, if media is really playing
//        _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
//        _playToggle = false;
//    }
//    else {
//        if (_pAvInterface) {
//            _pAvInterface->pausePressed();
//        }
//        else {
////            _pApplication->pausePressed();
//        }
//        _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
//        _playToggle = true;
//    }
//    _pStopButton->setEnabled(true);
//    _pForwardButton->setEnabled(true);
//    _pBackButton->setEnabled(true);
//}


//void
//QtMediaRendererControlPanel::stopButtonPressed()
//{
//    if (_pAvInterface) {
//        _pAvInterface->stopPressed();
//    }
//    else {
////        _pApplication->stopPressed();
//    }
//    _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
//    _playToggle = true;
//    _pStopButton->setEnabled(false);
//    _pForwardButton->setEnabled(false);
//    _pBackButton->setEnabled(false);
//}


void
QtMediaRendererControlPanel::setSeekSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_pSeekSlider->isSliderDown()) {
        return;
    }
    _pSeekSlider->setRange(0, max>=0?max:0);
    _pSeekSlider->setSliderPosition(val);
}


void
QtMediaRendererControlPanel::setVolumeSlider(int val)
{
    // don't set slider position when user drags the slider
    if (_pVolumeSlider->isSliderDown()) {
        return;
    }
    _pVolumeSlider->setRange(0, 100);
    _pVolumeSlider->setSliderPosition(val);
}


void
QtMediaRendererControlPanel::setTrackInfo(const QString& title, const QString& artist, const QString& album)
{
//    _rendererWidget._title->setText(title);
//    _rendererWidget._artist->setText(artist);
//    _rendererWidget._album->setText(album);
}


//void
//QtMediaRendererControlPanel::positionSliderMoved(int position)
//{
//    if (_pAvInterface) {
//        _pAvInterface->positionMoved(position);
//    }
//    else {
////        _pApplication->positionMoved(position);
//    }
//}


//void
//QtMediaRendererControlPanel::volumeSliderMoved(int value)
//{
//    if (_pAvInterface) {
//        _pAvInterface->volumeChanged(value);
//    }
//    else {
////        _pApplication->volumeChanged(value);
//    }
//}


void
QtMediaRendererControlPanel::checkSliderMoved(int value)
{
    if (_sliderMoved) {
        _sliderMoved = false;
        emit positionSliderMoved(value);
    }
}


void
QtMediaRendererControlPanel::setSliderMoved(int)
{
    _sliderMoved = true;
}