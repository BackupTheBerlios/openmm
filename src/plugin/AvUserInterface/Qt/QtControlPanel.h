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

#ifndef QtControlPanel_INCLUDED
#define	QtControlPanel_INCLUDED

#include <QToolBar>
#include <QPushButton>
#include <QSlider>


class QtAvInterface;
class QtApplication;
class QtActivityIndicator;


class QtPlayerRackButton : public QPushButton
{
public:
    QtPlayerRackButton(QWidget* pParent = 0);

    void setPlayerName(const std::string& name);
    void setTitleName(const std::string& name);

private:
    void setLabel();

    std::string     _playerName;
    std::string     _titleName;
};


class QtControlPanel : public QToolBar
{
    Q_OBJECT

public:
    QtControlPanel(QtAvInterface* pAvInterface, QWidget* pParent = 0);
    QtControlPanel(QtApplication* pApplication, QWidget* pParent = 0);
    ~QtControlPanel();

    void setPlayerName(const std::string& name);

signals:
    // position slider
    void sliderMoved(int value);
    void setSlider(int duration, int pos);
    // volume slider
    void volSliderMoved(int value);

private slots:
    void playButtonPressed();
    void stopButtonPressed();

    void setSeekSlider(int max, int val);
    void setVolumeSlider(int val);
    void setTrackInfo(const QString& title, const QString& artist, const QString& album);

    void positionSliderMoved(int position);
    void volumeSliderMoved(int value);
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
    QtAvInterface*                      _pAvInterface;
    QtApplication*                      _pApplication;
    QPushButton*                        _pBackButton;
    QPushButton*                        _pPlayButton;
    QPushButton*                        _pStopButton;
    QPushButton*                        _pForwardButton;

    QtActivityIndicator*                _pActivityIndicator;

    QSlider*                            _pVolumeSlider;
    QSlider*                            _pSeekSlider;

    QtPlayerRackButton*                 _pPlayerRackButton;

    bool                                _sliderMoved;
    bool                                _playToggle;
};


#endif
