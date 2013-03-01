/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#ifndef MediaRenderer_INCLUDED
#define MediaRenderer_INCLUDED

#include "../Upnp.h"
#include "../UpnpAvCtlRenderer.h"

#include "../Gui/ListModel.h"
#include "../Gui/List.h"
#include "../Gui/ListItem.h"
#include "../Gui/Button.h"
#include "../Gui/Label.h"
#include "../Gui/Slider.h"
#include "../Gui/HorizontalLayout.h"
#include "../Gui/VerticalLayout.h"

#include "DeviceGroup.h"

namespace Omm {

class UpnpApplication;
class ControllerWidget;
class MediaObjectModel;
class GuiVisual;
class MediaServerGroupWidget;
class MediaServerDevice;
class MediaRendererGroupWidget;
class MediaRendererView;
class MediaObjectView;
class PlaylistEditor;
class PlaylistEditorObjectView;
class ActivityIndicator;
class VolLabel;
class SeekLabel;


class TransportStateNotification : public Poco::Notification
{
public:
    TransportStateNotification(const std::string& uuid, const std::string& transportState) : _uuid(uuid), _transportState(transportState) {}

    std::string         _uuid;
    std::string         _transportState;
};


class TrackNotification : public Poco::Notification
{
public:
    TrackNotification(const std::string& uuid, const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass) : _uuid(uuid), _title(title), _artist(artist), _album(album), _objectClass(objectClass) {}

    std::string         _uuid;
    std::string         _title;
    std::string         _artist;
    std::string         _album;
    std::string         _objectClass;
};


class MediaRendererGroupWidget : public DeviceGroupWidget
{
public:
    MediaRendererGroupWidget(ControllerWidget* pControllerWidget);

    // Omm::DeviceGroup interface
    virtual Device* createDevice();

    // ListModel interface
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);
    virtual void selectedItem(int row);

private:
    ControllerWidget*   _pControllerWidget;
};


class MediaRendererDevice : public Av::CtlMediaRenderer, public Gui::Model
{
    friend class MediaRendererView;
    friend class VolSeekSlider;

public:
    MediaRendererDevice(ControllerWidget* pControllerWidget) : _transportState(""), _duration(0), _pControllerWidget(pControllerWidget) {}

    std::string getTransportState();

private:
    virtual void initController();

    virtual void newUri(const std::string& uri);
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass);
    virtual void newPosition(int duration, int position);
    virtual void newVolume(const int volume);
    virtual void newTransportState(const std::string& transportState);

    std::string         _transportState;
    int                 _duration;
    Gui::LabelModel     _rendererName;
    Gui::LabelModel     _trackName;
    Gui::LabelModel     _positionLabel;
    Gui::SliderModel    _position;
    Gui::LabelModel     _volumeLabel;
    Gui::SliderModel    _volume;
    ControllerWidget*   _pControllerWidget;
};


class MediaRendererView : public Gui::View
{
    friend class MediaRendererLayout;
    friend class VolSeekView;
    friend class VolLabel;
    friend class SeekLabel;
    friend class VolSeekSlider;

public:
    MediaRendererView(bool lineBreak = true);

    // FIXME: add submodels in setModel, no needed if submodels are supported
    virtual void setModel(Gui::Model* pModel);

    void selectedRenderer();

private:
    virtual void syncViewImpl();
    void switchVolSeekSlider();

    bool                    _lineBreak;
    Gui::View*              _pButtonPanel;
    Gui::View*              _pSliderPanel;
    Gui::View*              _pLabelPanel;

    Gui::Button*            _pBackButton;
    Gui::Button*            _pPlayButton;
    Gui::Button*            _pStopButton;
    Gui::Button*            _pForwButton;
    VolLabel*               _pVolLabel;
    SeekLabel*              _pSeekLabel;
    Gui::Slider*            _pVolSeekSlider;
    Gui::LabelView*         _pRendererName;
    Gui::LabelView*         _pTrackName;

    bool                    _modeVolume;
};

}  // namespace Omm


#endif
