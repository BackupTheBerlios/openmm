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

#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

#include "UpnpAvTypes.h"
#include "UpnpAvControllers.h"

namespace Omm {
namespace Av {

class ServerController;
class UpnpAvController;

// begin: exposed interface to user interface

class UpnpUserInterface
{
    friend class UpnpController;
    
public:
    // TODO: pass command line arguments to user interface gui-toolkit
    virtual int eventLoop() = 0;
    
    virtual void initGui() = 0;
    virtual void showMainWindow() = 0;
    
protected:
    virtual void beginAddDevice(int position) {}
    virtual void beginRemoveDevice(int position) {}
    virtual void endAddDevice(int position) {}
    virtual void endRemoveDevice(int position) {}
    
//     virtual void deviceAdded(Device* pDevice);
//     virtual void deviceRemoved(Device* pDevice);
    
    Container<Device>*    _pDevices;
};


class RendererView
{
    friend class UpnpAvController;
    friend class UpnpAvUserInterface;
    
public:
    const std::string& getName();
    
private:
    RendererView(MediaRendererController* rendererController);
    
    MediaRendererController*    _pRendererController;
};


class ControllerObject : public MediaObject
{
public:
    // NOTE: instead of this ctor make a MediaObjectWriter
    // MediaObject* createObject(const std::string&)
    // MediaObject* createObject(DatabasePointer)
    // and a MediaObjectReader ...
    
    // NOTE: should be split into a Controller and Device part?
//     MediaObject(const std::string& metaData);
//     MediaObject(Poco::XML::Node* pNode);
    
    ControllerObject();
    
    // controller interface
    virtual void addResource(Resource* pResource);
    
    void readChildren(const std::string& metaData);
    void readMetaData(const std::string& metaData);
    void readNode(Poco::XML::Node* pNode);
    
    int fetchChildren();
    bool fetchedAllChildren();
    ui4 childCount();
    ControllerObject* parent();
    
    std::string getProperty(const std::string& name);
    Resource* getResource(int num = 0);
    

    void setServerController(MediaServerController* _pServer);
    void setFetchedAllChildren(bool fetchedAllChildren);
    
private:
    unsigned int                            _childCount;
    bool                                    _fetchedAllChildren;
    MediaServerController*                  _server;
};


class UpnpAvUserInterface : public UpnpUserInterface
{
    friend class UpnpAvController;
    
public:
    UpnpAvUserInterface();
    
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

// end: exposed interface to user interface

class ServerController
{
public:
    ServerController(MediaServerController* pServerController);
    
    MediaServerController* controller() { return _pServerController; }
    ControllerObject* root() { return _pRoot; }
    
private:
    MediaServerController*    _pServerController;
    ControllerObject*         _pRoot;
};

// begin: exposed interface to controller application

class UpnpController : public Controller
{
public:
    ~UpnpController();
    
    void setUserInterface(UpnpUserInterface* pUserInterface) { _pUserInterface = pUserInterface; }
    
//     void start();
    
protected:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
private:
    Container<Device>               _devices;
    UpnpUserInterface*              _pUserInterface;
};


class UpnpAvController : public UpnpController
{
public:
    void setUserInterface(UpnpAvUserInterface* pUserInterface);
    
private:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
    Container<RendererView>               _renderers;
    Container<ServerController>           _servers;
    UpnpAvUserInterface*                  _pAvUserInterface;
};

// end: exposed interface to controller application

} // namespace Av
} // namespace Omm

#endif /* UPNPCONTROLLER_H */

