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

#include <Omm/Gui/Application.h>
#include <Omm/UpnpGui.h>
#include <Omm/UpnpAvRenderer.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/Util.h>

class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
//        setFullscreen(true);
//        resize(800, 480);
//        scaleMainView(2.0);
        _pController = new Omm::ControllerWidget;
        return _pController;
    }

    virtual void presentedMainView()
    {
        _pController->init();
        _pController->start();
        addLocalRenderer();
        _localDeviceServer.addDeviceContainer(&_localDeviceContainer);
        _localDeviceServer.init();
        _localDeviceServer.start();
    }

    virtual void finishedEventLoop()
    {
        _localDeviceServer.stop();
        _pController->stop();
    }

    void addLocalRenderer()
    {
        Omm::Av::Log::instance()->upnpav().debug("controller application add local renderer ...");
        Omm::Av::Engine* pEngine;
        Omm::Util::PluginLoader<Omm::Av::Engine> enginePluginLoader;
        try {
            pEngine = enginePluginLoader.load("engine-vlc");
        }
        catch(Poco::NotFoundException) {
            Omm::Av::Log::instance()->upnpav().error("controller application could not find plugin for engine");
            return;
        }

        pEngine->setVisual(_pController->getLocalRendererVisual());
        pEngine->createPlayer();

        _mediaRenderer.addEngine(pEngine);
        Omm::Icon* pRendererIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
        _mediaRenderer.addIcon(pRendererIcon);
        _mediaRenderer.setFriendlyName("OMM Renderer");
        _localDeviceContainer.addDevice(&_mediaRenderer);
        _localDeviceContainer.setRootDevice(&_mediaRenderer);
        Omm::Av::Log::instance()->upnpav().debug("controller application add local renderer finished.");
    }
    
    Omm::ControllerWidget*  _pController;
    Omm::Av::MediaRenderer  _mediaRenderer;
    Omm::DeviceContainer    _localDeviceContainer;
    Omm::DeviceServer       _localDeviceServer;
};


int main(int argc, char** argv)
{
    Application app;
    return app.run(argc, argv);
}

