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

#include <iostream>

#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>

#include <Omm/Gui/Application.h>
#include <Omm/UpnpGui.h>
#include <Omm/UpnpAvRenderer.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/Util.h>
#ifdef __IPHONE__
#include <Omm/X/EngineMPMoviePlayer.h>
#else
#include <Omm/X/EngineVlc.h>
#endif


class OmmApplication : public Omm::Gui::Application
{
public:

    OmmApplication() :
    _fullscreen(false)
    {
    }

    virtual Omm::Gui::View* createMainView()
    {
        _pController = new Omm::ControllerWidget;
        return _pController;
    }

    virtual void presentedMainView()
    {
        _pController->setTabBarHidden(_fullscreen);
        _pController->init();
        _pController->start();
        addLocalRenderer();
        _localDeviceServer.addDeviceContainer(&_localDeviceContainer);
        _localDeviceServer.init();
        _localDeviceServer.start();
        _pController->setDefaultRenderer(&_mediaRenderer);
    }

    virtual void finishedEventLoop()
    {
        _localDeviceServer.stop();
        _pController->stop();
    }

    void setRendererName(const std::string& rendererName)
    {
        _rendererName = rendererName;
    }

    void setFullscreen(bool fullscreen)
    {
        Omm::Gui::Application::setFullscreen(fullscreen);
        _fullscreen = fullscreen;
    }

    void addLocalRenderer()
    {
        Omm::Av::Log::instance()->upnpav().debug("controller application add local renderer ...");
#ifdef __IPHONE__
        Omm::Av::Engine* pEngine = new MPMoviePlayerEngine;
#else
        Omm::Av::Engine* pEngine = new VlcEngine;
#endif
        pEngine->setVisual(_pController->getLocalRendererVisual());
        pEngine->createPlayer();

        _mediaRenderer.addEngine(pEngine);
        Omm::Icon* pRendererIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
        _mediaRenderer.addIcon(pRendererIcon);
        _mediaRenderer.setFriendlyName(_rendererName);
        _localDeviceContainer.addDevice(&_mediaRenderer);
        _localDeviceContainer.setRootDevice(&_mediaRenderer);
        Omm::Av::Log::instance()->upnpav().debug("controller application add local renderer finished.");
    }

    Omm::ControllerWidget*  _pController;
    Omm::Av::MediaRenderer  _mediaRenderer;
    Omm::DeviceContainer    _localDeviceContainer;
    Omm::DeviceServer       _localDeviceServer;
    bool                    _fullscreen;
    std::string             _rendererName;
};


class Application :  public Poco::Util::Application
{
public:

    Application(int argc, char** argv) :
    _argc(argc),
    _argv(argv),
    _helpRequested(false),
    _rendererName("OMM Renderer"),
    _fullscreen(false),
    _width(800),
    _height(480),
    _scale(1.0)
    {
        setUnixOptions(true);
    }

    void initialize(Application& self)
    {
        loadConfiguration();
        Poco::Util::Application::initialize(self);
    }

    void uninitialize()
    {
        Poco::Util::Application::uninitialize();
    }

    void defineOptions(Poco::Util::OptionSet& options)
    {
        Poco::Util::Application::defineOptions(options);

        options.addOption(
                           Poco::Util::Option("help", "", "display help information on command line arguments")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("renderername", "r", "friendly name of UPnP-AV renderer")
                           .required(false)
                           .repeatable(false)
                           .argument("renderername", true));
        options.addOption(
                           Poco::Util::Option("fullscreen", "f", "option passed to plugin")
                           .required(false)
                           .repeatable(false));
        options.addOption(
                           Poco::Util::Option("width", "w", "width of application window")
                           .required(false)
                           .repeatable(false)
                           .argument("width", true));
        options.addOption(
                           Poco::Util::Option("height", "h", "height of application window")
                           .required(false)
                           .repeatable(false)
                           .argument("height", true));
        options.addOption(
                           Poco::Util::Option("scale", "s", "application window scale factor")
                           .required(false)
                           .repeatable(false)
                           .argument("scale", true));
    }

    void handleOption(const std::string& name, const std::string& value)
    {
        Poco::Util::Application::handleOption(name, value);

        if (name == "help") {
            _helpRequested = true;
        }
        else if (name == "renderername") {
            _rendererName = value;
        }
        else if (name == "fullscreen") {
            _fullscreen = true;
        }
        else if (name == "width") {
            _width = Poco::NumberParser::parse(value);
        }
        else if (name == "height") {
            _height = Poco::NumberParser::parse(value);
        }
        else if (name == "scale") {
            _scale = Poco::NumberParser::parseFloat(value);
        }
    }

    void displayHelp()
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("OMM application running a UPnP controller, renderer and servers.");
        helpFormatter.format(std::cout);
    }

    int main(const std::vector<std::string>& args)
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            OmmApplication app;
            app.setRendererName(_rendererName);
            app.setFullscreen(_fullscreen);
            app.resizeMainView(_width, _height);
            app.scaleMainView(_scale);
            app.run(_argc, _argv);
        }
        return Application::EXIT_OK;
    }

    int                     _argc;
    char**                  _argv;
    bool                    _helpRequested;
    std::string             _rendererName;
    bool                    _fullscreen;
    int                     _width;
    int                     _height;
    float                   _scale;
};


int main(int argc, char** argv)
{
    Application app(argc, argv);
    app.init(argc, argv);
    return app.run();
}

