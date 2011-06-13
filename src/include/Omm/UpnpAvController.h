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
#include "UpnpAvObject.h"


namespace Omm {
namespace Av {

class ServerController;
class AvController;
class CtlMediaServer;
class CtlMediaRenderer;
class Engine;
class AvServer;
class AvRenderer;

class RendererView
{
    friend class AvController;
    friend class AvUserInterface;
    
public:
    const std::string getName();
    const std::string getUuid();
    
private:
    RendererView(CtlMediaRenderer* rendererController);
    
    CtlMediaRenderer*    _pRendererController;
};


class ControllerObject : public MediaObject
{
    friend class ServerController;
    friend class AvUserInterface;
    
public:
    ControllerObject();
    
    void setServerController(CtlMediaServer* _pServer);
    
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
    
    unsigned int                     _childCount;
    bool                             _fetchedAllChildren;
    CtlMediaServer*                  _server;
};


class AvUserInterface : public UserInterface
{
    friend class AvController;
    
public:
    AvUserInterface();
    ~AvUserInterface();
    
    /// all device add/remove callbacks must run/queue a method in the main
    /// thread where the event loop of the GUI toolkit lives

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
    virtual void showMenu(bool show) {}
    
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

    std::string selectedRendererUuid();

    void setLocalEngine(Engine* pEngine);
    void addLocalServer(AvServer* pServer);
    void startLocalServers();
    void stopLocalServers();
    void startLocalRenderer();
    void stopLocalRenderer();

private:
    void pollPositionInfo(Poco::Timer& timer);
//    void startFileServers();
//    void startWebradio();
    
    AvController*                         _pAvController;
    CtlMediaRenderer*                     _pSelectedRenderer;
    ControllerObject*                     _pSelectedObject;
    Poco::Timer                           _positionInfoTimer;

    Engine*                               _pEngine;
    AvRenderer*                           _pRenderer;
    std::vector<AvServer*>                _localServers;
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
    virtual void addDeviceContainer(DeviceContainer* pDeviceContainer);
    /// get root device of pDeviceContainer
    /// create ControllerImplAdapter with ServiceControllers depending on type of root device
    /// add it to list of renderers if type is AvRenderer
    /// add it to list of servers if type is AvServer
    // TODO: handle subdevices.
    virtual void removeDeviceContainer(DeviceContainer* pDeviceContainer);
    
    Container<RendererView>               _renderers;
    Container<ServerController>           _servers;
};


} // namespace Av
} // namespace Omm


#endif
