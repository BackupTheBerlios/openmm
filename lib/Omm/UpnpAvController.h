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
    const std::string& getName();
    
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
    
    int rendererCount();
    RendererView* rendererView(int numRenderer);
    int serverCount();
    ControllerObject* serverRootObject(int numServer);
    
    void playPressed();
    void stopPressed();
    void pausePressed();
    void positionMoved(int position);
    void volumeChanged(int value);
    
    void rendererSelected(RendererView* pRenderer);
    void mediaObjectSelected(ControllerObject* pObject);
    
private:
    Container<RendererView>*              _pRenderers;
    Container<ServerController>*          _pServers;
    MediaRendererController*              _pSelectedRenderer;
    ControllerObject*                     _pSelectedObject;
};


class AvController : public Controller
{
public:
    void setUserInterface(AvUserInterface* pUserInterface);
    
private:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
    Container<RendererView>               _renderers;
    Container<ServerController>           _servers;
};


} // namespace Av
} // namespace Omm


#endif
