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

#ifndef ActivityIndicator_INCLUDED
#define ActivityIndicator_INCLUDED

#include <Poco/Notification.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/XMLConfiguration.h>
#include <Poco/Util/PropertyFileConfiguration.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

#include "../Sys/Signal.h"

#include "../Upnp.h"
#include "../UpnpAvCtlRenderer.h"
#include "../UpnpAvRenderer.h"
#include "../UpnpAvCtlServer.h"
#include "../UpnpAvCtlObject.h"
#include "../UpnpAvRenderer.h"
#include "../UpnpAvServer.h"

#include "../Gui/Application.h"
#include "../Gui/Cluster.h"
#include "../Gui/Navigator.h"
#include "../Gui/ListModel.h"
#include "../Gui/List.h"
#include "../Gui/LazyList.h"
#include "../Gui/ListItem.h"
#include "../Gui/Button.h"
#include "../Gui/Label.h"
#include "../Gui/TextLine.h"
#include "../Gui/Slider.h"
#include "../Gui/HorizontalLayout.h"
#include "../Gui/Image.h"
#include "../Gui/WebBrowser.h"


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


class ActivityIndicator : public Gui::ImageView
{
public:
    ActivityIndicator(Gui::View* pParent = 0);
    ~ActivityIndicator();

public:
    void startActivity();
    void stopActivity();

private:
    void stopIndicator(Poco::Timer& timer);
    void setActivityInProgress(bool set);
    bool activityInProgress();
    void setIndicatorOn(bool set);
    bool indicatorOn();
    bool timerActive();
    void setTimerActive(bool set);

    const int                                   _indicateDuration;
    Gui::ImageModel*                            _pActivityOffModel;
    Gui::ImageModel*                            _pActivityOnModel;
    bool                                        _activityInProgress;
    Poco::FastMutex                             _activityInProgressLock;
    bool                                        _indicatorOn;
    Poco::FastMutex                             _indicatorOnLock;
    Poco::Timer*                                _pOffTimer;
    bool                                        _timerActive;
    Poco::FastMutex                             _timerActiveLock;
    Poco::TimerCallback<ActivityIndicator>      _stopIndicatorCallback;
};


}  // namespace Omm


#endif
