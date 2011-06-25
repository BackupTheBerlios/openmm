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

#ifndef UpnpAvApplication_INCLUDED
#define	UpnpAvApplication_INCLUDED

#include "Upnp.h"

namespace Omm {
//namespace Av {

//class Engine;
//class AvServer;
//class AvRenderer;
class UpnpApplication;


class ApplicationUserInterface : public ControllerUserInterface
{
    friend class UpnpApplication;
    
private:
    UpnpApplication*      _pApp;
};


class UpnpApplication
{
public:
    UpnpApplication();

    void setUserInterface(ApplicationUserInterface* pAppUserInterface);
    void init();
    void start();
    void stop();

//    void setLocalEngine(Engine* pEngine);
//    void addLocalServer(AvServer* pServer);
//    void startLocalServers();
//    void stopLocalServers();
//    void startLocalRenderer();
//    void stopLocalRenderer();

//    void startFileServers();
//    void startWebradio();

private:
    ApplicationUserInterface*             _pApplicationUserInterface;
    Controller*                           _pController;
//    Engine*                               _pEngine;
//    AvRenderer*                           _pRenderer;
//    std::vector<AvServer*>                _localServers;

};


//} // namespace Av
} // namespace Omm

#endif

