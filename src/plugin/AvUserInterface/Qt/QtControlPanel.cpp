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

#include "QtControlPanel.h"
#include "QtAvInterface.h"
#include "QtActivityIndicator.h"


QtPlayerRackButton::QtPlayerRackButton(QWidget* pParent) :
QPushButton(pParent)
{
    setMinimumWidth(250);
    setCheckable(true);
}


void
QtPlayerRackButton::setPlayerName(const std::string& name)
{
    _playerName = name;
    setLabel();
}


void
QtPlayerRackButton::setTitleName(const std::string& name)
{
    _titleName = name;
    setLabel();
}


void
QtPlayerRackButton::setLabel()
{
    if (_titleName == "") {
        setText(_playerName.c_str());
    }
    else {
        setText((_playerName + " - " + _titleName).c_str());
    }
}


QtControlPanel::QtControlPanel(QtAvInterface* pAvInterface, QWidget* pParent) :
QToolBar("ControlPanel", pParent),
_pAvInterface(pAvInterface)
{
    _pBackButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "", this);
    _pPlayButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "", this);
    _pStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaStop), "", this);
    _pForwardButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "", this);

    addWidget(_pBackButton);
    addWidget(_pPlayButton);
    addWidget(_pStopButton);
    addWidget(_pForwardButton);

    _pActivityIndicator = new QtActivityIndicator(this);
    addWidget(_pActivityIndicator);

    _pVolumeSlider = new QSlider(Qt::Horizontal, this);
    _pVolumeSlider->setTracking(true);
    _pVolumeSlider->setSingleStep(5);
    _pSeekSlider = new QSlider(Qt::Horizontal, this);
    _pSeekSlider->setSingleStep(10);
    _pSeekSlider->setPageStep(25);

    addWidget(_pVolumeSlider);
    addWidget(_pSeekSlider);

    _pPlayerRackButton = new QtPlayerRackButton(this);
    addWidget(_pPlayerRackButton);

    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    connect(_pPlayButton, SIGNAL(pressed()), this, SLOT(playButtonPressed()));
    connect(_pStopButton, SIGNAL(pressed()), this, SLOT(stopButtonPressed()));
    connect(_pForwardButton, SIGNAL(pressed()), this, SLOT(skipForwardButtonPressed()));
    connect(_pBackButton, SIGNAL(pressed()), this, SLOT(skipBackwardButtonPressed()));

    connect(_pVolumeSlider, SIGNAL(sliderMoved(int)), this, SLOT(volumeSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(valueChanged(int)), this, SLOT(checkSliderMoved(int)));
    connect(_pSeekSlider, SIGNAL(actionTriggered(int)), this, SLOT(setSliderMoved(int)));

    connect(this, SIGNAL(sliderMoved(int)), this, SLOT(positionSliderMoved(int)));
    connect(this, SIGNAL(setSlider(int, int)), this, SLOT(setSeekSlider(int, int)));
    connect(this, SIGNAL(volSliderMoved(int)), this, SLOT(setVolumeSlider(int)));

    connect(this, SIGNAL(startNetworkActivity()),
            _pActivityIndicator, SLOT(startActivity()));
    connect(this, SIGNAL(stopNetworkActivity()),
            _pActivityIndicator, SLOT(stopActivity()));
    connect(this, SIGNAL(nowPlaying(const QString&, const QString&, const QString&)),
            this, SLOT(setTrackInfo(const QString&, const QString&, const QString&)));

    connect(_pPlayerRackButton, SIGNAL(toggled(bool)), _pAvInterface, SLOT(showPlayerRack(bool)));
}


QtControlPanel::~QtControlPanel()
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
QtControlPanel::setPlayerName(const std::string& name)
{
    _pPlayerRackButton->setPlayerName(name);
}


void
QtControlPanel::playButtonPressed()
{
    if (_playToggle) {
        _pAvInterface->playPressed();
        // TODO: only toggle play button to pause, if media is really playing
        _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        _playToggle = false;
    }
    else {
        _pAvInterface->pausePressed();
        _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        _playToggle = true;
    }
    _pStopButton->setEnabled(true);
    _pForwardButton->setEnabled(true);
    _pBackButton->setEnabled(true);
}


void
QtControlPanel::stopButtonPressed()
{
    _pAvInterface->stopPressed();
    _pPlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    _playToggle = true;
    _pStopButton->setEnabled(false);
    _pForwardButton->setEnabled(false);
    _pBackButton->setEnabled(false);
}


void
QtControlPanel::setSeekSlider(int max, int val)
{
    // don't set slider position when user drags the slider
    if (_pSeekSlider->isSliderDown()) {
        return;
    }
    _pSeekSlider->setRange(0, max>=0?max:0);
    _pSeekSlider->setSliderPosition(val);
}


void
QtControlPanel::setVolumeSlider(int val)
{
    // don't set slider position when user drags the slider
    if (_pVolumeSlider->isSliderDown()) {
        return;
    }
    _pVolumeSlider->setRange(0, 100);
    _pVolumeSlider->setSliderPosition(val);
}


void
QtControlPanel::setTrackInfo(const QString& title, const QString& artist, const QString& album)
{
//    _rendererWidget._title->setText(title);
//    _rendererWidget._artist->setText(artist);
//    _rendererWidget._album->setText(album);
}


void
QtControlPanel::positionSliderMoved(int position)
{
    _pAvInterface->positionMoved(position);
}


void
QtControlPanel::volumeSliderMoved(int value)
{
    _pAvInterface->volumeChanged(value);
}


void
QtControlPanel::checkSliderMoved(int value)
{
    if (_sliderMoved) {
        _sliderMoved = false;
        emit sliderMoved(value);
    }
}


void
QtControlPanel::setSliderMoved(int)
{
    _sliderMoved = true;
}