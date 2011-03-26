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
#ifndef UpnpAvController_INCLUDED
#define UpnpAvController_INCLUDED

#include <Poco/Timer.h>
#include <Poco/DOM/Node.h>

#include "Upnp.h"
#include "UpnpAvTypes.h"
#include "UpnpAvControllers.h"

namespace Omm {
namespace Av {

class ServerController;
class AvController;

class RendererView
{
    friend class AvController;
    friend class AvUserInterface;
    
public:
    const std::string getName();
    const std::string getUuid();
    
private:
    RendererView(MediaRendererController* rendererController);
    
    MediaRendererController*    _pRendererController;
};


class ControllerObject : public MediaObject
{
    friend class ServerController;
    friend class AvUserInterface;
    
public:
    ControllerObject();
    
    void setServerController(MediaServerController* _pServer);
    
    int fetchChildren();
    bool fetchedAllChildren();
    ui4 childCount();
    ControllerObject* parent();
    
    std::string getProperty(const std::string& name);

    Icon* getIcon();
    Icon* getImageRepresentation();
    
private:
    Resource* getResource(int num = 0);
    virtual void addResource(Resource* pResource);
    void setFetchedAllChildren(bool fetchedAllChildren);
    
    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);
    
    unsigned int                            _childCount;
    bool                                    _fetchedAllChildren;
    MediaServerController*                  _server;
};


class AvUserInterface : public UserInterface
{
    friend class AvController;
    
public:
    AvUserInterface();
    ~AvUserInterface();
    
    // TODO: alle device add/remove callbacks should be run in the main thread where the event loop lives
    // this avoids complications when implementing thread safety in the target GUI toolkit
    // Poco::NotificationCenter doesn't help here, cause notifications are delivered in the thread in which
    // they are posted ... after thinking a bit about it, it seems to be impossible, as the method call has
    // to be queued in the event loop, and this is responsibility of the GUI main thread's event loop.
    virtual void beginAddRenderer(int position) {}
    virtual void beginAddServer(int position) {}
    virtual void beginRemoveRenderer(int position) {}
    virtual void beginRemoveServer(int position) {}
    virtual void endAddRenderer(int position) {}
    virtual void endAddServer(int position) {}
    virtual void endRemoveRenderer(int position) {}
    virtual void endRemoveServer(int position) {}
    
    virtual void newPosition(int duration, int position) {}
    virtual void newTrack(const std::string& title, const std::string& artist, const std::string& album) {}
    virtual void newVolume(const int volume) {}
    
    int rendererCount();
    RendererView* rendererView(int numRenderer);
    bool isPlaying(RendererView* pRenderer);
    int serverCount();
    ControllerObject* serverRootObject(int numServer);
    const std::string serverUuid(int numServer);

    // TODO: getIcon() ... of server or renderer device ...
    // retrieves the icon that is closest to width, heigth, depth.
    // if no parameters are given, the first icon is retrieved.
    // if no icon is retrieved, 0 is returned.
//    Icon* getIcon(int width = 0, int height = 0, int depth = 0);
    
    void playPressed();
    void stopPressed();
    void pausePressed();
    void positionMoved(int position);
    void volumeChanged(int value);
    
    void rendererSelected(RendererView* pRenderer);
    void mediaObjectSelected(ControllerObject* pObject);
    
private:
    void pollPositionInfo(Poco::Timer& timer);
    
    AvController*                         _pAvController;
    MediaRendererController*              _pSelectedRenderer;
    ControllerObject*                     _pSelectedObject;
    Poco::Timer                           _positionInfoTimer;
};


class AvController : public Controller
{
public:
    void setUserInterface(AvUserInterface* pUserInterface);

    int rendererCount();
    int serverCount();
    RendererView* rendererView(int numRenderer);
    ControllerObject* serverRootObject(int numServer);
    Device* serverDevice(int numServer);
    
private:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
    Container<RendererView>               _renderers;
    Container<ServerController>           _servers;
};


} // namespace Av
} // namespace Omm


#endif
