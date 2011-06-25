/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                           |
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

//#include "UpnpAvServer.h"
//#include "UpnpAvRenderer.h"

#include "UpnpApplication.h"

namespace Omm {
//namespace Av {


UpnpApplication::UpnpApplication() :
_pApplicationUserInterface(0)
//_pEngine(0)
{
    _pController = new Controller;
}


void
UpnpApplication::setUserInterface(ApplicationUserInterface* pAppUserInterface)
{
    _pApplicationUserInterface = pAppUserInterface;
    _pApplicationUserInterface->_pApp = this;
    _pController->setUserInterface(pAppUserInterface);
}


void
UpnpApplication::init()
{
    _pController->init();
    _pApplicationUserInterface->initGui();
    _pApplicationUserInterface->showMainWindow();
}


void
UpnpApplication::start()
{
    _pController->start();
}


void
UpnpApplication::stop()
{
    _pController->stop();
}


//void
//UpnpApplication::startLocalServers()
//{
//    for (std::vector<AvServer*>::iterator it = _localServers.begin(); it != _localServers.end(); ++it) {
//        // TODO: start DeviceContainer instead of single devices
////        (*it)->startThreaded();
//    }
//}
//
//
//void
//UpnpApplication::stopLocalServers()
//{
//    for (std::vector<AvServer*>::iterator it = _localServers.begin(); it != _localServers.end(); ++it) {
//        // TODO: start DeviceContainer instead of single devices
////        (*it)->stopThreaded();
//    }
//}
//
//
//void
//UpnpApplication::startLocalRenderer()
//{
//    if (_pEngine) {
//        _pEngine->createPlayer();
//        _pRenderer = new Omm::Av::AvRenderer(_pEngine);
////        _pRenderer->setFriendlyName("Local Player");
//        //_pEngine->setParentView(parentView);
//        // TODO: start device container of renderer
////        _pRenderer->start();
//    }
//    else {
//        Log::instance()->upnpav().warning("no engine set in user interface, local renderer not started.");
//    }
//}
//
//
//void
//UpnpApplication::stopLocalRenderer()
//{
//    // TODO: stop device container of renderer
////    _pRenderer->stop();
//}
//
//
//void
//UpnpApplication::setLocalEngine(Engine* pEngine)
//{
//    _pEngine = pEngine;
//}
//
//
//void
//UpnpApplication::addLocalServer(AvServer* pServer)
//{
//    _localServers.push_back(pServer);
//}


//void
//AvUserInterface::startFileServers()
//{
//    ThreadedServer* pMusic = new ThreadedServer("mp3", "Music (on iPhone)");
//    _FileServers.push_back(pMusic);
//    ThreadedServer* pNeuseeland = new ThreadedServer("Neuseeland", "Neuseeland (on iPhone)");
//    _FileServers.push_back(pNeuseeland);
//    pMusic->start();
//    pNeuseeland->start();
//}


//void
//AvUserInterface::startWebradio()
//{
//    std::string home = Poco::Environment::get("HOME");
//
//    // TODO: load webradio server plugin?
//    //_pWebradioContainer = new WebradioServer;
//    std::string webradioName = "Web Radio";
//    std::string webradioConf = home + "/.omm/webradio.conf";
////    std::string webradioConf = "/var/root/.omm/webradio.conf";
////    std::string webradioConf = "/Users/jb/.omm/webradio.conf";
//    _pWebradioContainer->setOption("basePath", webradioConf);
//    _pWebradioContainer->setTitle(webradioName);
//    _pWebradioServer = new Omm::Av::AvServer;
//    _pWebradioServer->setRoot(_pWebradioContainer);
//    _pWebradioServer->setFriendlyName(webradioName);
//    _pWebradioIcon = new Omm::Icon(22, 22, 8, "image/png", "device.png");
//    _pWebradioServer->addIcon(_pWebradioIcon);
//    _pWebradioServer->start();
//}



//} // namespace Av
} // namespace Omm
