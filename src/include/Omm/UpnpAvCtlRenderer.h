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
#ifndef UpnpAvCtlRenderer_INCLUDED
#define UpnpAvCtlRenderer_INCLUDED

#include <Poco/Timer.h>
#include <Poco/DOM/Node.h>
#include <Poco/Notification.h>

#include "Upnp.h"
#include "UpnpInternal.h"
#include "UpnpAvObject.h"


namespace Omm {
namespace Av {

class AvServerView;
class AvController;
class CtlMediaServerCode;
class CtlMediaRendererCode;
class CtlMediaObject;
class CtlMediaObject;
class MediaItemNotification;
class MediaObjectSelectedNotification;
class ConnectionManager;


class CtlMediaRenderer : public Device
{
public:
    CtlMediaRenderer();

    virtual void addCtlDeviceCode();
    virtual void initController();

    void setObject(CtlMediaObject* pObject, CtlMediaObject* pParentObject, ui4 row);
    CtlMediaObject* getObject();
    void playPressed();
    void stopPressed();
    void pausePressed();
    void forwardPressed();
    void backPressed();
    void positionMoved(r8 position);
    /// positionMoved() seeks to position in secs
    void volumeChanged(int value);
    ui2 getVolume();
    void setMute(bool mute);
    bool getMute();

    void startPositionTimer(bool start = true);
    ConnectionManager* getConnectionManager();

    virtual void newPosition(r8 duration, r8 position) {}
    virtual void newUri(const std::string& uri) {}
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album, const std::string& objectClass, const std::string& server, const std::string& uri) {}
    virtual void newVolume(const int volume) {}
    virtual void newTransportState(const std::string& transportState) {}


private:
    void pollPositionInfo(Poco::Timer& timer);

    // for convenience only, to avoid multiple pointer cast from CtlDeviceCode* to CtlMediaRendererCode*;
    CtlMediaRendererCode*   _pCtlMediaRendererCode;
    CtlMediaObject*         _pCurrentMediaObject;
    bool                    _usePlaylistResource;
    Poco::Timer*            _pPositionTimer;
    long                    _positionTimerInterval;
};


class MediaRendererGroupDelegate : public DeviceGroupDelegate
{
public:
    virtual std::string getDeviceType();
    virtual std::string shortName();

private:
    virtual void init();

    void mediaItemSelectedHandler(MediaObjectSelectedNotification* pMediaItemNotification);
};


} // namespace Av
} // namespace Omm


#endif
