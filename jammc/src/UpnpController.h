/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef UPNPCONTROLLER_H
#define UPNPCONTROLLER_H

#include <Jamm/UpnpAvTypes.h>

#include "UpnpAvCtrlImpl.h"

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
    
    Jamm::Container<Jamm::Device>*    m_pDevices;
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


class UpnpAvUserInterface : UpnpUserInterface
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
    void mediaObjectSelected(Jamm::Av::MediaObject* pObject);
    
private:
    Jamm::Container<RendererView>*              m_pRenderers;
    Jamm::Container<ServerController>*          m_pServers;
    MediaRendererController*                    m_pSelectedRenderer;
    Jamm::Av::MediaObject*                      m_pSelectedObject;
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

class UpnpController : public Jamm::Controller
{
public:
    ~UpnpController();
    
    void setUserInterface(UpnpUserInterface* pUserInterface) { m_pUserInterface = pUserInterface; }
    
    void start();
    
protected:
    virtual void deviceAdded(Jamm::DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(Jamm::DeviceRoot* pDeviceRoot);
    
private:
    Jamm::Container<Jamm::Device>               m_devices;
    UpnpUserInterface*                          m_pUserInterface;
};


class UpnpAvController : public UpnpController
{
public:
    void setUserInterface(UpnpAvUserInterface* pUserInterface);
    
private:
    virtual void deviceAdded(Jamm::DeviceRoot* pDeviceRoot);
    virtual void deviceRemoved(Jamm::DeviceRoot* pDeviceRoot);
    
    Jamm::Container<RendererView>               m_renderers;
    Jamm::Container<ServerController>           m_servers;
    UpnpAvUserInterface*                        m_pAvUserInterface;
};

// end: exposed interface to controller application

#endif /* UPNPCONTROLLER_H */

