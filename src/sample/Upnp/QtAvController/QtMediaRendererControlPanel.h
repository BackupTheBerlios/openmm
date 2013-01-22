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

#ifndef QtMediaRendererControlPanel_INCLUDED
#define	QtMediaRendererControlPanel_INCLUDED

#include <QtGui>


class QtNowPlaying : public QLabel
{
public:
    QtNowPlaying(QWidget* pParent = 0);

    void setPlayerName(const std::string& name);
    void setTitleName(const std::string& name);

private:
    void setLabel();

    std::string     _playerName;
    std::string     _titleName;
};


class QtMediaRendererControlPanel : public QToolBar
{
    Q_OBJECT

public:
    QtMediaRendererControlPanel();
    ~QtMediaRendererControlPanel();

    void setPlayerName(const std::string& name);

signals:
    // buttons
    void playButtonPressed();
    void stopButtonPressed();
    // volume slider
    void volumeSliderMoved(int value);
    // position slider
    void positionSliderMoved(int value);
//    void setPosSlider(int duration, int pos);

public slots:
    void setSeekSlider(int max, int val);
    void setVolumeSlider(int val);
    void setTrackInfo(const QString& title, const QString& artist, const QString& album);

private slots:
//        QAbstractSlider emits signal valueChanged() when the slider was
//        once moved and some time later (a new track is loaded), the range
//        changes. This triggers a Seek in the MediaRenderer to the position
//        of the last Seek (in the previous track). The following two slots
//        make QAbstractSlider only emit valueChanged() when triggered by
//        a user action.
//        This could be considered a bug and not a feature ...
    void checkSliderMoved(int value);
    void setSliderMoved(int value);

private:
    QPushButton*                        _pBackButton;
    QPushButton*                        _pPlayButton;
    QPushButton*                        _pStopButton;
    QPushButton*                        _pForwardButton;

    QSlider*                            _pVolumeSlider;
    QSlider*                            _pSeekSlider;

    QtNowPlaying*                       _pNowPlaying;

    bool                                _sliderMoved;
    bool                                _playToggle;
};


#endif
