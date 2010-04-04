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
    virtual void beginAddDevice(int position) = 0;
    virtual void beginRemoveDevice(int position) = 0;
    virtual void endAddDevice() = 0;
    virtual void endRemoveDevice() = 0;
    
//     virtual void deviceAdded(Device* pDevice);
//     virtual void deviceRemoved(Device* pDevice);
    
    Container<Device>*    m_pDevices;
};


class RendererView
{
    friend class UpnpAvController;
    friend class UpnpAvUserInterface;
    
public:
    const std::string& getName();
    
private:
    RendererView(MediaRendererController* rendererController);
    
    MediaRendererController*    m_pRendererController;
};


class MediaObjectView
{
};


class UpnpAvUserInterface : public UpnpUserInterface
{
    friend class UpnpAvController;
    
public:
    UpnpAvUserInterface();
    
    virtual void beginAddRenderer(int position) = 0;
    virtual void beginAddServer(int position) = 0;
    virtual void beginRemoveRenderer(int position) = 0;
    virtual void beginRemoveServer(int position) = 0;
    virtual void endAddRenderer() = 0;
    virtual void endAddServer() = 0;
    virtual void endRemoveRenderer() = 0;
    virtual void endRemoveServer() = 0;
    
    int rendererCount();
    RendererView* rendererView(int numRenderer);
    int serverCount();
    MediaObject* serverRootObject(int numServer);
    
    void playPressed();
    void stopPressed();
    void pausePressed();
    void positionMoved(int position);
    void volumeChanged(int value);
    
    void rendererSelected(RendererView* pRenderer);
    void mediaObjectSelected(MediaObject* pObject);
    
private:
    Container<RendererView>*              m_pRenderers;
    Container<ServerController>*          m_pServers;
    MediaRendererController*              m_pSelectedRenderer;
    MediaObject*                          m_pSelectedObject;
};

// end: exposed interface to user interface

class ServerController
{
public:
    ServerController(MediaServerController* pServerController);
    
    MediaServerController* controller() { return m_pServerController; }
    MediaObject* root() { return m_pRoot; }
    
private:
    MediaServerController*    m_pServerController;
    MediaObject*              m_pRoot;
};

// begin: exposed interface to controller application

class UpnpController : public Controller
{
public:
    ~UpnpController();
    
    void setUserInterface(UpnpUserInterface* pUserInterface) { m_pUserInterface = pUserInterface; }
    
//     void start();
    
protected:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
private:
    Container<Device>               m_devices;
    UpnpUserInterface*              m_pUserInterface;
};


class UpnpAvController : public UpnpController
{
public:
    void setUserInterface(UpnpAvUserInterface* pUserInterface);
    
private:
    virtual void deviceAdded(DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(DeviceRoot* pDeviceRoot);
    
    Container<RendererView>               m_renderers;
    Container<ServerController>           m_servers;
    UpnpAvUserInterface*                  m_pAvUserInterface;
};

// end: exposed interface to controller application

} // namespace Av
} // namespace Omm

#endif /* UPNPCONTROLLER_H */

