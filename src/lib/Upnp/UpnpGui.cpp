/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/StreamCopier.h>
#include <Poco/NamedMutex.h>
#include <Poco/Util/Application.h>

#include "UpnpGui.h"
#include "Gui/GuiLogger.h"
#include "UpnpAv.h"
#include "UpnpAvCtlServer.h"
#include "MediaImages.h"

// FIXME: make engines also loadable, so that ommcontroller executable
//        doesn't need to link against an engine.
#ifdef __IPHONE__
#include <Omm/X/EngineMPMoviePlayer.h>
#else
#include <Omm/X/EngineVlc.h>
#include <Omm/X/EnginePhonon.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Net/NameValueCollection.h>
#endif


namespace Omm {


void
ConfigRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    Log::instance()->upnp().debug("omm application http request \"" + request.getURI() + "\" from " + request.getHost());

    Poco::URI requestUri(request.getURI());

    if (requestUri.getPath() == UpnpApplication::PLAYLIST_URI) {
        std::stringstream* pPlaylistResource = _pApp->_pControllerWidget->getPlaylistResource();
        if (pPlaylistResource) {
            std::ostream& outStream = response.send();
            Poco::StreamCopier::copyStream(*pPlaylistResource, outStream);
            delete pPlaylistResource;
        }
    }
    else if (requestUri.getPath() == UpnpApplication::CONFIG_URI) {
//        response.setChunkedTransferEncoding(true);
        response.setContentType("text/html");

        Poco::Net::HTMLForm form(request, request.stream());
        if (requestUri.getQuery() == UpnpApplication::CONFIG_APP_QUERY) {
            _pApp->handleAppConfigRequest(form);
        }
        else if (requestUri.getQuery() == UpnpApplication::CONFIG_DEV_QUERY) {
            _pApp->handleDevConfigRequest(form);
        }

        std::ostream& outStream = response.send();
        std::istream* pConfigPage = _pApp->generateConfigPage();
        Poco::StreamCopier::copyStream(*pConfigPage, outStream);
        delete pConfigPage;
    }
    else {
        response.send();
    }
}


Poco::Net::HTTPRequestHandler*
ConfigRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    return new ConfigRequestHandler(_pApp);
}


const std::string UpnpApplication::PLAYLIST_URI = "/Playlist";
const std::string UpnpApplication::CONFIG_URI = "/Config";
const std::string UpnpApplication::CONFIG_APP_QUERY = "saveForm=app";
const std::string UpnpApplication::CONFIG_DEV_QUERY = "saveForm=dev";

UpnpApplication::UpnpApplication(int argc, char** argv) :
//Poco::Util::Application(argc, argv),
_argc(argc),
_argv(argv),
_helpRequested(false),
_lockInstance(true),
_ignoreConfig(false),
//#ifdef __IPHONE__
//    _rendererName("iPhone Renderer"),
//#else
//    _rendererName("OMM Renderer"),
//#endif
_enableRenderer(false),
_enableServer(false),
_enableController(true),
_showRendererVisualOnly(false),
_pControllerWidget(0),
_pLocalDeviceServer(new DeviceServer),
_pLocalDeviceContainer(new DeviceContainer),
_pLocalMediaRenderer(0),
_pConf(0),
_instanceMutexName("OmmApplicationMutex"),
_appStandardPort(4009)
{
    setUnixOptions(true);
}


void
UpnpApplication::defineOptions(Poco::Util::OptionSet& options)
{
    Poco::Util::Application::defineOptions(options);

    options.addOption(Poco::Util::Option("help", "", "display help information on command line arguments")
                      .required(false)
                      .repeatable(false));
    options.addOption(Poco::Util::Option("width", "w", "width of application window")
                      .binding("application.width")
                      .required(false)
                      .repeatable(false)
                      .argument("width", true));
    options.addOption(Poco::Util::Option("height", "h", "height of application window")
                      .binding("application.height")
                      .required(false)
                      .repeatable(false)
                      .argument("height", true));
    options.addOption(Poco::Util::Option("fullscreen", "f", "option passed to plugin")
                      .binding("application.fullscreen")
                      .required(false)
                      .repeatable(false));
    options.addOption(Poco::Util::Option("zoom", "z", "application window scale factor")
                      .binding("application.scale")
                      .required(false)
                      .repeatable(false)
                      .argument("scale", true));
    options.addOption(Poco::Util::Option("renderer", "r", "enable renderer  \"name:uuid:engine\"")
                      .required(false)
                      .repeatable(false)
                      .argument("renderername", true));
    options.addOption(Poco::Util::Option("server", "s", "add server \"name:uuid:datamodel:basepath\"")
                      .required(false)
                      .repeatable(true)
                      .argument("device", true));
}


void
UpnpApplication::handleOption(const std::string& name, const std::string& value)
{
    Poco::Util::Application::handleOption(name, value);

    if (name == "help") {
        _helpRequested = true;
    }
    else if (name == "renderer") {
        Poco::StringTokenizer rendererSpec(value, ":");
        if (rendererSpec.count() < 3) {
            Omm::Av::Log::instance()->upnpav().information("renderer spec \"" + value + "\" needs four parameters, \"name:uuid:engine\", ignoring");
        }
        else {
            std::string uuid = rendererSpec[1];
            // uuid may be a valid uuid or empty, when empty assign a random uuid
            if (uuid == "") {
                uuid = Poco::UUIDGenerator().createRandom().toString();
            }
            config().setString("renderer.enable", "true");
            config().setString("renderer.friendlyName", rendererSpec[0]);
            config().setString("renderer.uuid", uuid);
            config().setString("renderer.plugin", "engine-" + rendererSpec[2]);
        }
    }
    else if (name == "server") {
        Poco::StringTokenizer serverSpec(value, ":");
        if (serverSpec.count() < 4) {
            Omm::Av::Log::instance()->upnpav().information("server spec \"" + value + "\" needs four parameters, \"name:uuid:datamodel:basepath\", ignoring");
        }
        else {
            std::string uuid = serverSpec[1];
            // uuid may be a valid uuid or empty, when empty assign a random uuid
            if (uuid == "") {
                uuid = Poco::UUIDGenerator().createRandom().toString();
            }
            config().setString("server." + uuid + ".enable", "true");
            config().setString("server." + uuid + ".friendlyName", serverSpec[0]);
            config().setString("server." + uuid + ".uuid", uuid);
            config().setString("server." + uuid + ".plugin", "model-" + serverSpec[2]);
            config().setString("server." + uuid + ".basePath", serverSpec[3]);
        }
    }
    else if (name == "fullscreen") {
        config().setString("application.fullscreen", "true");
    }
}


int
UpnpApplication::main(const std::vector<std::string>& args)
{
    if (_helpRequested)
    {
        displayHelp();
    }
    else
    {
        Poco::Util::Application::init(_argc, _argv);

        if (instanceAlreadyRunning()) {
            Log::instance()->upnp().information("omm application instance running, starting in controller mode");
            setLockInstance(false);
            setIgnoreConfig(true);
        }

        loadConfig();
        initConfig();
        startAppHttpServer();

        Gui::Application::runEventLoop(_argc, _argv);

        stopAppHttpServer();
        saveConfig();

        uninitialize();
    }
    return Poco::Util::Application::EXIT_OK;
}


Omm::Gui::View*
UpnpApplication::createMainView()
{
    _pControllerWidget = new Omm::ControllerWidget(this);
    if (!_showRendererVisualOnly) {
        setToolBar(_pControllerWidget->getControlPanel());
        setStatusBar(_pControllerWidget->getStatusBar());
    }
    return _pControllerWidget;
}


void
UpnpApplication::presentedMainView()
{
    if (config().getBool("application.fullscreen", false)) {
        _pControllerWidget->setTabBarHidden(true);
        _pControllerWidget->showOnlyBasicDeviceGroups(true);
    }
    else if (_showRendererVisualOnly) {
        _pControllerWidget->showOnlyRendererVisual(true);
        _pControllerWidget->setTabBarHidden(true);
    }
    _pControllerWidget->init();
}


void
UpnpApplication::start()
{
    initLocalDevices();

    if (_enableController) {
        _pControllerWidget->setState(config().getString("application.state", DeviceManager::Started));
//        _pControllerWidget->setState(DeviceManager::Started);
    }
    _pLocalDeviceServer->setState(config().getString("application.state", DeviceManager::Started));
//    _pLocalDeviceServer->setState(DeviceManager::Started);
}


void
UpnpApplication::stop()
{
    Omm::Av::Log::instance()->upnpav().debug("omm application stopping ...");
    _pLocalDeviceServer->setState(DeviceManager::Stopped);
    if (_enableController) {
        _pControllerWidget->setState(DeviceManager::Stopped);
    }
    Omm::Av::Log::instance()->upnpav().debug("omm application stopped.");
}


void
UpnpApplication::displayHelp()
{
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("OMM application running a UPnP controller, renderer and servers.");
    helpFormatter.format(std::cout);
}


void
UpnpApplication::setLockInstance(bool lock)
{
    _lockInstance = lock;
}


void
UpnpApplication::setIgnoreConfig(bool ignore)
{
    _ignoreConfig = ignore;
}


void
UpnpApplication::printConfig()
{
//    std::vector<std::string> rootKeys;
//    config().keys(rootKeys);
//    for (std::vector<std::string>::iterator it = rootKeys.begin(); it != rootKeys.end(); ++it) {
//        Omm::Av::Log::instance()->upnpav().debug("omm config, root keys: " + *it);
//    }

    std::vector<std::string> rendererKeys;
    config().keys("renderer", rendererKeys);
    for (std::vector<std::string>::iterator it = rendererKeys.begin(); it != rendererKeys.end(); ++it) {
        Omm::Av::Log::instance()->upnpav().debug("omm config renderer." + *it + ": " + config().getString("renderer." + *it, ""));
    }

    std::vector<std::string> serverKeys;
    config().keys("server", serverKeys);
    for (std::vector<std::string>::iterator it = serverKeys.begin(); it != serverKeys.end(); ++it) {
        std::vector<std::string> serverConfigKeys;
        config().keys("server." + *it, serverConfigKeys);
        for (std::vector<std::string>::iterator cit = serverConfigKeys.begin(); cit != serverConfigKeys.end(); ++cit) {
            Omm::Av::Log::instance()->upnpav().debug("omm config server." + *it + "." + *cit + ": " + config().getString("server." + *it + "." + *cit, ""));
        }
    }

//    if (_pConf) {
//        std::vector<std::string> confKeys;
//        _pConf->keys(confKeys);
//        for (std::vector<std::string>::iterator it = confKeys.begin(); it != confKeys.end(); ++it) {
//            Omm::Av::Log::instance()->upnpav().debug("omm config, config file keys: " + *it + ", value: " + _pConf->getString(*it, ""));
//        }
//    }

//    std::vector<std::string> appKeys;
//    config().keys("application", appKeys);
//    for (std::vector<std::string>::iterator it = appKeys.begin(); it != appKeys.end(); ++it) {
//        Omm::Av::Log::instance()->upnpav().debug("omm config, application keys: " + *it + ", value: " + config().getString("application." + *it, ""));
//    }
//
//    std::vector<std::string> sysKeys;
//    config().keys("system", sysKeys);
//    for (std::vector<std::string>::iterator it = sysKeys.begin(); it != sysKeys.end(); ++it) {
//        Omm::Av::Log::instance()->upnpav().debug("omm config, system keys: " + *it + ", value: " + config().getString("system." + *it, ""));
//    }
}


void
UpnpApplication::printForm(const Poco::Net::HTMLForm& form)
{
    for (Poco::Net::NameValueCollection::ConstIterator it = form.begin(); it != form.end(); ++it)
    {
        Av::Log::instance()->upnpav().debug("form " + it->first + ": " + it->second);
    }
}


void
UpnpApplication::loadConfig()
{
    if (!_ignoreConfig) {
        Omm::Av::Log::instance()->upnpav().information("reading config file ...");

        _confFilePath = Omm::Util::Home::instance()->getConfigDirPath("/") + "omm.properties";
        _pConf = new Poco::Util::PropertyFileConfiguration;
        try {
            _pConf->load(_confFilePath);
        }
        catch (Poco::Exception& e) {
            Omm::Av::Log::instance()->upnpav().debug("no config file present");
        }
    //        config().addWriteable(_pConf, -200);
        config().addWriteable(_pConf, 0);
    }
    printConfig();
}


void
UpnpApplication::initConfig()
{
    setFullscreen(config().getBool("application.fullscreen", false));
    resizeMainView(config().getInt("application.width", 800), config().getInt("application.height", 480));
    scaleMainView(config().getDouble("application.scale", 1.0));
}


void
UpnpApplication::saveConfig()
{
    if (!_ignoreConfig) {
//            _pConf->setInt("width", app.width());
//            _pConf->setInt("height", app.height());
    // FIXME: main view is smaller than app window
        _pConf->setInt("application.width", getMainView()->width());
        _pConf->setInt("application.height", getMainView()->height());
//            config().setInt("width", app.getMainView()->width());
//            config().setInt("height", app.getMainView()->height());
        _pConf->save(_confFilePath);
    }
}



std::stringstream*
UpnpApplication::generateConfigPage()
{
    std::stringstream* pOutStream = new std::stringstream;
    *pOutStream << "<html>\n"
                    "<head>\n"
                    "<title>OMM Configuration</title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<h1>OMM Configuration</h1>\n";

    *pOutStream << "Address of this page is " << getAppHttpUri() << CONFIG_URI << "<br>\n";

    if (!_pConf) {
        *pOutStream << "configuration file not found<br>\n";
        return pOutStream;
    }

    *pOutStream << "<form method=\"POST\" action=\"" + CONFIG_URI + "?" + CONFIG_APP_QUERY + "\"><br>\n";

    std::string appState = _pConf->getString("application.state", "Started");
    *pOutStream << std::string("state<select name=\"application.state\" size=\"1\">") +
                "<option " + (appState == DeviceManager::Started ? "selected" : "") + ">" + DeviceManager::Started + "</option>" +
                "<option " + (appState == DeviceManager::Local ? "selected" : "") + ">" + DeviceManager::Local + "</option>" +
                "<option " + (appState == DeviceManager::Stopped ? "selected" : "") + ">" + DeviceManager::Stopped + "</option>" +
                "</select><br>\n";

    *pOutStream << "<input type=\"submit\" value=\"Save\">\n";
    *pOutStream << "</form>\n";

    *pOutStream << "<form method=\"POST\" action=\"" + CONFIG_URI + "?" + CONFIG_DEV_QUERY + "\">\n";

    *pOutStream << "<h2>Media Renderer</h2>\n";
    bool rendererEnable = _pConf->getBool("renderer.enable", false);
    std::string rendererName = _pConf->getString("renderer.friendlyName", "");
    std::string rendererUuid = _pConf->getString("renderer.uuid", "");
    std::string rendererPlugin = _pConf->getString("renderer.plugin", "");
    *pOutStream << "<fieldset><legend>" + rendererName + "</legend>"
        "<table>"
        "<tr><td>friendly name</td><td><input type=\"text\" name=\"renderer.friendlyName\" size=\"32\" value=\"" + rendererName +  "\"></td></tr>\n"
        "<tr><td>uuid</td><td><input type=\"text\" name=\"renderer.uuid\" size=\"32\" value=\"" + rendererUuid +  "\"></td></tr>\n"
        "<tr><td>plugin</td><td><input type=\"text\" name=\"renderer.plugin\" size=\"32\" value=\"" + rendererPlugin +  "\"></td></tr>\n"
        "<tr><td>enable</td><td><input type=\"checkbox\" name=\"renderer.enable\" value=\"true\"" +  (rendererEnable ? "checked" : "") + " ></td></tr>\n"
        "</table>"
        "</fieldset>";

    *pOutStream << "<h2>Media Servers</h2>\n";
    std::string serversString = _pConf->getString("servers", "");
    Poco::StringTokenizer servers(serversString, ",");
    for (Poco::StringTokenizer::Iterator it = servers.begin(); it != servers.end(); ++it) {
        std::string serverKey = "server." + *it;
        bool serverEnable = _pConf->getBool(serverKey + ".enable", false);
        std::string serverName = _pConf->getString(serverKey + ".friendlyName", "");
        std::string serverUuid = _pConf->getString(serverKey + ".uuid", "");
        std::string serverPlugin = _pConf->getString(serverKey + ".plugin", "");
        std::string basePath = _pConf->getString(serverKey + ".basePath", "");
        std::string pollUpdateId = _pConf->getString(serverKey + ".pollUpdateId", "0");
        bool checkMod = _pConf->getBool(serverKey + ".checkMod", false);
        std::string layout = _pConf->getString(serverKey + ".layout", "");

        *pOutStream << "<fieldset><legend>" + serverName + "</legend>"
            "<table>"
            "<tr><td>enable</td><td><input type=\"hidden\" name=\"" + serverKey + ".enable\" value=\"false\"/><input type=\"checkbox\" name=\"" + serverKey + ".enable\" value=\"true\" " +  (serverEnable ? "checked" : "") + "></td></tr>\n"
            "<tr><td>friendly name</td><td><input type=\"text\" name=\"" + serverKey + ".friendlyName\" size=\"32\" value=\"" + serverName +  "\"></td></tr>\n"
            "<tr><td>uuid</td><td><input type=\"text\" name=\"" + serverKey + ".uuid\" size=\"32\" value=\"" + serverUuid +  "\"></td></tr>\n"
            "<tr><td>plugin</td><td><input type=\"text\" name=\"" + serverKey + ".plugin\" size=\"32\" value=\"" + serverPlugin +  "\"></td></tr>\n"
            "<tr><td>base path</td><td><input type=\"text\" name=\"" + serverKey + ".basePath\" size=\"32\" value=\"" + basePath +  "\"></td></tr>\n"
            "<tr><td>poll</td><td><input type=\"text\" name=\"" + serverKey + ".pollUpdateId\" size=\"10\" value=\"" + pollUpdateId +  "\"></td></tr>\n"
            "<tr><td>check mod</td><td><input type=\"hidden\" name=\"" + serverKey + ".checkMod\" value=\"false\"/><input type=\"checkbox\" name=\"" + serverKey + ".checkMod\" value=\"true\" " +  (checkMod ? "checked" : "") + "></td></tr>\n"
            "<tr><td>layout</td><td><select name=\"" + serverKey + ".layout\" size=\"1\">"
                "<option " + (layout == Av::ServerContainer::LAYOUT_FLAT ? "selected" : "") + ">" + Av::ServerContainer::LAYOUT_FLAT + "</option>"
                "<option " + (layout == Av::ServerContainer::LAYOUT_DIR_STRUCT ? "selected" : "") + ">" + Av::ServerContainer::LAYOUT_DIR_STRUCT + "</option>"
                "<option " + (layout == Av::ServerContainer::LAYOUT_PROPERTY_GROUPS ? "selected" : "") + ">" + Av::ServerContainer::LAYOUT_PROPERTY_GROUPS + "</option>"
                "</select></td></tr>\n"
            "<tr><td><input type=\"submit\" name=\"delete." + *it + "\" value=\"Delete\"></td></tr>\n"
            "</table>"
            "</fieldset><br>";
    }

    std::string newServerKey = "server.new";
    *pOutStream << "<fieldset><legend>New Server</legend>"
            "<table>"
            "<tr><td>enable</td><td><input type=\"hidden\" name=\"" + newServerKey + ".enable\" value=\"false\"/><input type=\"checkbox\" name=\"" + newServerKey + ".enable\" value=\"true\"></td></tr>\n"
            "<tr><td>friendly name</td><td><input type=\"text\" name=\"" + newServerKey + ".friendlyName\" size=\"32\" value=\"\"></td></tr>\n"
            "<tr><td>uuid</td><td><input type=\"text\" name=\"" + newServerKey + ".uuid\" size=\"32\" value=\"" + Poco::UUIDGenerator().createRandom().toString() +  "\"></td></tr>\n"
            "<tr><td>plugin</td><td><input type=\"text\" name=\"" + newServerKey + ".plugin\" size=\"32\" value=\"\"></td></tr>\n"
            "<tr><td>base path</td><td><input type=\"text\" name=\"" + newServerKey + ".basePath\" size=\"32\" value=\"\"></td></tr>\n"
            "<tr><td>poll</td><td><input type=\"text\" name=\"" + newServerKey + ".pollUpdateId\" size=\"10\" value=\"0\"></td></tr>\n"
            "<tr><td>check mod</td><td><input type=\"hidden\" name=\"" + newServerKey + ".checkMod\" value=\"false\"/><input type=\"checkbox\" name=\"" + newServerKey + ".checkMod\" value=\"true\"></td></tr>\n"
            "<tr><td>layout</td><td><select name=\"" + newServerKey + ".layout\" size=\"1\"> <option>" + Av::ServerContainer::LAYOUT_FLAT + "</option>"
                "<option>" + Av::ServerContainer::LAYOUT_DIR_STRUCT + "</option>"
                "<option>" + Av::ServerContainer::LAYOUT_PROPERTY_GROUPS + "</option></select></td></tr>\n"
            "<tr><td><input type=\"submit\" name=\"create\" value=\"New\"></td></tr>\n"
            "</table>"
            "</fieldset><br>";

    *pOutStream << "<input type=\"submit\" value=\"Save\">\n";
    *pOutStream << "</form>\n";
    *pOutStream << "</body></html>\n";

    return pOutStream;
}


void
UpnpApplication::handleAppConfigRequest(const Poco::Net::HTMLForm& form)
{
    if (!form.empty()) {
        printForm(form);
        for (Poco::Net::NameValueCollection::ConstIterator it = form.begin(); it != form.end(); ++it) {
            _pConf->setString(it->first, it->second);
            if (it->first == "application.state") {
                if (_enableController) {
                    _pControllerWidget->setState(it->second);
                }
                _pLocalDeviceServer->setState(it->second);
            }
        }
    }
}


void
UpnpApplication::handleDevConfigRequest(const Poco::Net::HTMLForm& form)
{
    if (!form.empty()) {
        Av::Log::instance()->upnpav().debug("omm local devices config update ...");
        printConfig();
        printForm(form);

        // read form data and write all entries to file configuration
        Av::Log::instance()->upnpav().debug("omm config read in devices form config ...");
        std::set<std::string> serverIds;
        std::string deleteUuid;
        bool createServer = false;
        for (Poco::Net::NameValueCollection::ConstIterator it = form.begin(); it != form.end(); ++it) {
            // NOTE: When delete is pressed, an additional form entry is generated: delete.uuid (same for create w/o uuid)
            Poco::StringTokenizer key(it->first, ".");
            if (key[0] == "delete") {
                deleteUuid = key[1];
            }
            else if (it->first == "create") {
                createServer = true;
            }
            else {
                _pConf->setString(it->first, it->second);
            }
            if (key[0] == "server") {
                serverIds.insert(key[1]);
            }
        }
        serverIds.erase(deleteUuid);
        serverIds.erase("new");

        if (createServer) {
            // find new or deleted config file id for local server.
            int id = 0;
            std::string newId = "0";
            while (serverIds.find(newId) != serverIds.end()) {
                id++;
                newId = Poco::NumberFormatter::format(id);
            }
            // copy data of new server into new server entry.
            Av::Log::instance()->upnpav().debug("omm config create new local device with id: " + newId);
            std::vector<std::string> newServerKeys;
            _pConf->keys("server.new", newServerKeys);
            for (std::vector<std::string>::iterator it = newServerKeys.begin(); it != newServerKeys.end(); ++it) {
                Av::Log::instance()->upnpav().debug(std::string("add config entry ") + "server." + newId + "." + *it + ": " + _pConf->getString(*it, ""));
                _pConf->setString("server." + newId + "." + *it, _pConf->getString("server.new." + *it, ""));
            }
            serverIds.insert(newId);
        }

        std::string servers;
        for (std::set<std::string>::const_iterator it = serverIds.begin(); it != serverIds.end(); ++it) {
            servers.append(*it);
            if (it != --serverIds.end()) {
                servers.append(",");
            }
        }
        _pConf->setString("servers", servers);

        // restart local device container and all devices contained
        _pLocalDeviceServer->setState(DeviceManager::Stopped);
        _pLocalDeviceServer = new DeviceServer;
//        _pLocalDeviceServer->removeDeviceContainer(_pLocalDeviceContainer);
        // delete local devices
//        delete _pLocalDeviceContainer;
        _pLocalDeviceContainer = new DeviceContainer;
        initConfig();
        initLocalDevices();
        _pLocalDeviceServer->setState(DeviceManager::Started);
        Av::Log::instance()->upnpav().debug("omm local devices config update done.");
    }
}


void
UpnpApplication::enableController(bool enable)
{
//    if (_pControllerWidget) {
//        enable ? _pControllerWidget->start() : _pControllerWidget->stop();
//    }
    _enableController = enable;
}


void
UpnpApplication::showRendererVisualOnly(bool show)
{
//    _pControllerWidget->setTabBarHidden(!show);
//    _pControllerWidget->showOnlyRendererVisual(show);
    _showRendererVisualOnly = show;
    showToolBar(false);
    showStatusBar(false);
}


void
UpnpApplication::initLocalDevices()
{
    // add local renderer
    Omm::Av::Log::instance()->upnpav().debug("omm application init local devices ...");
    if (config().getBool("renderer.enable", false)) {
        setLocalRenderer(config().getString("renderer.friendlyName", "OMM Renderer"),
                config().getString("renderer.uuid", ""),
                config().getString("renderer.plugin", ""));
    }

    // add local servers from config file
    std::string serversString;
    if (_pConf) {
        serversString = _pConf->getString("servers", "");
    }
    Poco::StringTokenizer servers(serversString, ",");
    for (Poco::StringTokenizer::Iterator it = servers.begin(); it != servers.end(); ++it) {
        Omm::Av::Log::instance()->upnpav().debug("omm config, server: " + *it);
        if (_pConf->getBool("server." + *it + ".enable", false)) {
            addLocalServer(*it);
        }
    }
    // add local servers from command line
    std::vector<std::string> serverKeys;
    config().keys("server", serverKeys);
    for (std::vector<std::string>::iterator it = serverKeys.begin(); it != serverKeys.end(); ++it) {
        if (!(_pConf && _pConf->hasProperty("server." + *it + ".enable"))) {
            Omm::Av::Log::instance()->upnpav().debug("omm config, server: " + *it);
            if (config().getBool("server." + *it + ".enable", false)) {
                addLocalServer(*it);
            }
        }
    }

//#ifndef __IPHONE__
    if (_enableRenderer) {
        setLocalRenderer();
    }
    if (_enableRenderer || _enableServer) {
        _pLocalDeviceServer->addDeviceContainer(_pLocalDeviceContainer);
    }
//#endif
    _pLocalDeviceServer->init();
//#ifndef __IPHONE__
    if (_enableRenderer && _pLocalMediaRenderer) {
        // TODO: get default renderer from config file via uuid
        _pControllerWidget->setDefaultRenderer(_pLocalMediaRenderer);
    }
//#endif
   Omm::Av::Log::instance()->upnpav().debug("omm application init local devices done.");
}


void
UpnpApplication::setLocalRenderer(const std::string& name, const std::string& uuid, const std::string& pluginName)
{
    _enableRenderer = true;
    _rendererName = name;
    _rendererUuid = uuid;
    _rendererPlugin = pluginName;
}


void
UpnpApplication::setLocalRenderer()
{
    Omm::Av::Log::instance()->upnpav().debug("omm application set local renderer ...");

    Omm::Av::Engine* pEngine;
#ifdef __IPHONE__
//        pEngine = new MPMoviePlayerEngine;
#else
    Omm::Util::PluginLoader<Omm::Av::Engine> pluginLoader;
    try {
        pEngine = pluginLoader.load(_rendererPlugin);
    }
    catch(Poco::NotFoundException) {
        Omm::Av::Log::instance()->upnpav().error("could not find engine plugin: " + _rendererPlugin);
        pEngine = new VlcEngine;
    }
    Omm::Av::Log::instance()->upnpav().information("engine plugin: " + _rendererPlugin + " loaded successfully");
#endif

    pEngine->setVisual(_pControllerWidget->getLocalRendererVisual());
    pEngine->createPlayer();

    Av::MediaRenderer* pMediaRenderer = new Av::MediaRenderer;
    _pLocalMediaRenderer = pMediaRenderer;
    pMediaRenderer->addEngine(pEngine);
    Omm::Icon* pRendererIcon = new Omm::Icon(22, 22, 8, "image/png", "renderer.png");
    pMediaRenderer->addIcon(pRendererIcon);
    pMediaRenderer->setFriendlyName(_rendererName);
    pMediaRenderer->setUuid(_rendererUuid);
    if (_enableRenderer) {
        _pLocalDeviceContainer->addDevice(pMediaRenderer);
        _pLocalDeviceContainer->setRootDevice(pMediaRenderer);
    }
    else {
        if (_pLocalDeviceContainer->getDeviceCount()) {
            _pLocalDeviceContainer->setRootDevice((*_pLocalDeviceContainer->beginDevice()));
        }
    }
    Omm::Av::Log::instance()->upnpav().debug("omm application set local renderer finished.");
}


void
UpnpApplication::addLocalServer(const std::string& id)
{
    Omm::Av::Log::instance()->upnpav().debug("omm application add local server with id: " + id + " ...");

    std::string pluginName = config().getString("server." + id + ".plugin", "model-webradio");
    Omm::Av::AbstractDataModel* pDataModel;
    Omm::Util::PluginLoader<Omm::Av::AbstractDataModel> pluginLoader;
    try {
        pDataModel = pluginLoader.load(pluginName);
    }
    catch(Poco::NotFoundException) {
        Omm::Av::Log::instance()->upnpav().error("could not find server plugin: " + pluginName);
        return;
    }
    Omm::Av::Log::instance()->upnpav().information("container plugin: " + pluginName + " loaded successfully");

    _enableServer = true;
    Omm::Av::MediaServer* pMediaServer = new Av::MediaServer;

    std::string name = config().getString("server." + id + ".friendlyName", "OMM Server");
    Omm::Av::ServerContainer* pContainer = new Av::ServerContainer(pMediaServer);
    pContainer->setTitle(name);
    pContainer->setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER));
    pContainer->setDataModel(pDataModel);
    std::string basePath = config().getString("server." + id + ".basePath", "webradio.conf");
    std::string path = basePath;
    // if relative path then search file in config directory
    if (basePath[0] != '/') {
        path = Omm::Util::Home::instance()->getConfigDirPath("/") + basePath;
    }
    pContainer->setBasePath(path);
    pContainer->setLayout(config().getString("server." + id + ".layout", "Flat"));
    pContainer->getDataModel()->setTextEncoding(config().getString("server." + id + ".textEncoding", "UTF8"));
    pContainer->getDataModel()->setCheckObjectModifications(config().getBool("server." + id + ".checkMod", false));

    pMediaServer->setRoot(pContainer);
    pMediaServer->setFriendlyName(name);
    pMediaServer->setUuid(config().getString("server." + id + ".uuid", ""));
    Omm::Icon* pIcon = new Omm::Icon(32, 32, 8, "image/png", "server.png");
    pMediaServer->addIcon(pIcon);
    pMediaServer->setPollSystemUpdateIdTimer(config().getInt("server." + id + ".pollUpdateId", 0));

    _pLocalDeviceContainer->addDevice(pMediaServer);
    if (!_enableRenderer) {
        _pLocalDeviceContainer->setRootDevice(pMediaServer);
    }

    Omm::Av::Log::instance()->upnpav().debug("omm application add local server finished.");
}


void
UpnpApplication::startAppHttpServer()
{
    int port = config().getInt("application.configPort", _appStandardPort);
    bool useRandomPort = true;
    if (!instanceAlreadyRunning()) {
        try {
            _socket = Poco::Net::ServerSocket(port);
            useRandomPort = false;
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnp().error("failed to start application http server on port " + Poco::NumberFormatter::format(port) + "(" + e.displayText() + ") , using random port.");
        }
    }
    if (useRandomPort) {
        try {
            _socket = Poco::Net::ServerSocket(0);
        }
        catch (Poco::Exception& e) {
            Log::instance()->upnp().error("failed to start application http server: " + e.displayText());
        }
    }
    Poco::Net::HTTPServerParams* pParams = new Poco::Net::HTTPServerParams;
    _pHttpServer = new Poco::Net::HTTPServer(new ConfigRequestHandlerFactory(this), _socket, pParams);
    _pHttpServer->start();
    Log::instance()->upnp().information("omm application http server listening on: " + _socket.address().toString());
}


void
UpnpApplication::stopAppHttpServer()
{
    _pHttpServer->stop();
    Log::instance()->upnp().information("omm application http server stopped on port: " + Poco::NumberFormatter::format(_socket.address().port()));
}


bool
UpnpApplication::instanceAlreadyRunning()
{
#ifdef __IPHONE__
    return false;
#endif

    if (_lockInstance) {
        _lockInstance = false;
        Poco::NamedMutex mutex(_instanceMutexName);
        return !mutex.tryLock();
    }
    else {
        return false;
    }
}


std::string
UpnpApplication::getAppHttpUri()
{
    std::string address = Net::NetworkInterfaceManager::instance()->getValidIpAddress().toString();
    return "http://" + address + ":" + Poco::NumberFormatter::format(_socket.address().port());
}


std::string
UpnpApplication::getConfigHttpUri()
{
    // FIXME: on second process instance, configuration is ignored, but config page of first process instance
    // should be displayed. If standard port is used, this works, but not if another port is configured.
    // However, getting the configured port doesn't solve the problem, as it may be in use when first instance was started,
    // and so first instance chose a random port.
    return "http://localhost:" + config().getString("application.configPort", Poco::NumberFormatter::format(_appStandardPort)) + "/Config";
//    return "http://localhost:" + config().getString("application.configPort", Poco::NumberFormatter::format(_socket.address().port())) + "/Config";
//    return "http://localhost:" + Poco::NumberFormatter::format(_socket.address().port()) + "/Config";
}


ControllerWidget::ControllerWidget(UpnpApplication* pApplication) :
_pApplication(pApplication)
{
    Gui::Log::instance()->gui().debug("controller widget register device groups ...");
    _pMediaServerGroupWidget = new MediaServerGroupWidget;
    registerDeviceGroup(_pMediaServerGroupWidget);
    _pMediaRendererGroupWidget = new MediaRendererGroupWidget(this);
    registerDeviceGroup(_pMediaRendererGroupWidget);
    _pPlaylistEditor = new PlaylistEditor(this);
    addView(_pPlaylistEditor, "Playlist Editor");
    _pVisual = new GuiVisual;
    addView(_pVisual, "Video");
    _pConfigBrowser = new Gui::WebBrowser;
    addView(_pConfigBrowser, "Setup");
    _pConfigBrowser->setUri(_pApplication->getConfigHttpUri());
    _pControlPanel = new MediaRendererView;
    _pActivityIndicator = new ActivityIndicator;
//    _pStatusBar->resize(20, 20);

    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, TransportStateNotification>(*this, &ControllerWidget::newTransportState));
    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<ControllerWidget, PlaylistNotification>(*this, &ControllerWidget::newPlaylist));
    attachController(new KeyController(this));
}


void
ControllerWidget::setState(State newState)
{
    Log::instance()->upnp().debug("controller widget state change: " + _state + " -> " + newState);
    if (_state == newState) {
        Log::instance()->upnp().debug("new state equal to old state, ignoring");
        return;
    }
    if (newState == Started) {
        Controller::setState(Started);
    }
    else if (newState == Stopped) {
        Controller::setState(Stopped);
        // don't stop selected renderer, it should continue playing though the controller is stopped
//        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pMediaRendererGroupWidget->getSelectedDevice());
//        if (pRenderer) {
//            pRenderer->stopPressed();
//        }
        // stop position info timer for all renderer devices that controller knows of (renderers continue to play)
        for (int r = 0; r < _pMediaRendererGroupWidget->getDeviceCount(); r++) {
            MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pMediaRendererGroupWidget->getDevice(r));
            pRenderer->startPositionTimer(false);
        }
        // clear device views and remove controller's device containers
        _pMediaServerGroupWidget->popToRoot();
        _pMediaServerGroupWidget->clearDevices();
        _pMediaServerGroupWidget->syncView();
        _pMediaRendererGroupWidget->clearDevices();
        _pMediaRendererGroupWidget->syncView();
    }
    Log::instance()->upnp().debug("controller widget state change finished");
}


GuiVisual*
ControllerWidget::getLocalRendererVisual()
{
    return _pVisual;
}


MediaRendererView*
ControllerWidget::getControlPanel()
{
    return _pControlPanel;
}


Gui::View*
ControllerWidget::getStatusBar()
{
    return _pActivityIndicator;
}


void
ControllerWidget::setDefaultRenderer(Omm::Av::MediaRenderer* pRenderer)
{
    _localRendererUuid = pRenderer->getUuid();
    _pMediaRendererGroupWidget->setDefaultDevice(pRenderer);
}


void
ControllerWidget::newTransportState(TransportStateNotification* pNotification)
{
    Gui::Log::instance()->gui().debug("controller widget device: " + pNotification->_uuid + " got new transport state: " + pNotification->_transportState);
    Gui::Log::instance()->gui().debug("local renderer uuid: " + _localRendererUuid);
    if (pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING && pNotification->_uuid == _localRendererUuid) {
        Av::CtlMediaRenderer* pRenderer = static_cast<Av::CtlMediaRenderer*>(_pMediaRendererGroupWidget->getDevice(pNotification->_uuid));
        if (pRenderer) {
            Av::CtlMediaObject2* pObject = pRenderer->getObject();
            if (pObject) {
                Gui::Log::instance()->gui().debug("local renderer plays object: " + pObject->getTitle() + ", class: " + pObject->getClass());
                if (Av::AvClass::matchClass(pObject->getClass(), Av::AvClass::ITEM, Av::AvClass::VIDEO_ITEM)
                    || Av::AvClass::matchClass(pObject->getClass(), Av::AvClass::ITEM, Av::AvClass::VIDEO_BROADCAST)) {
                    setCurrentView(_pVisual);
                }
            }
        }
    }
    else if (pNotification->_transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED) {
        showMainMenu();
    }
}


void
ControllerWidget::newPlaylist(PlaylistNotification* pNotification)
{
    // write resource of playlist container
    // TODO: move this to playlist editor, assign a playlist container to playlist editor.
//    pModel->writeResource(getControllerHttpUri());
}


void
ControllerWidget::showMainMenu()
{
    setCurrentView(_pMediaServerGroupWidget);
}


void
ControllerWidget::showOnlyBasicDeviceGroups(bool show)
{
    addView(_pMediaRendererGroupWidget, "Player", !show);
    addView(_pPlaylistEditor, "Playlist Editor", !show);
}


void
ControllerWidget::showOnlyRendererVisual(bool show)
{
    addView(_pMediaServerGroupWidget, "Media", !show);
    addView(_pMediaRendererGroupWidget, "Player", !show);
    addView(_pPlaylistEditor, "Playlist Editor", !show);
}


void
ControllerWidget::navigateListWithKey(Gui::Controller::KeyCode key)
{
    if (_pMediaServerGroupWidget->getVisibleView()) {
        _pMediaServerGroupWidget->getVisibleView()->triggerKeyEvent(key);
    }
}


void
ControllerWidget::back()
{
    _pMediaServerGroupWidget->pop();
}


void
ControllerWidget::signalNetworkActivity(bool on)
{
    on ? _pActivityIndicator->startActivity() : _pActivityIndicator->stopActivity();
}


std::stringstream*
ControllerWidget::getPlaylistResource()
{
    return _pPlaylistEditor->getPlaylistResource();
}


MediaServerDevice*
ControllerWidget::getServer(const std::string& uuid)
{
    return static_cast<MediaServerDevice*>(_pMediaServerGroupWidget->getDevice(uuid));
}


void
KeyController::keyPressed(KeyCode key)
{
    Gui::Log::instance()->gui().debug("key controller, key pressed: " + Poco::NumberFormatter::format(key));
    switch (key) {
        case Gui::Controller::KeyMenu:
            _pControllerWidget->showMainMenu();
            break;
        case Gui::Controller::KeyLeft:
            if (_pControllerWidget->getCurrentTab()) {
                _pControllerWidget->setCurrentTab(_pControllerWidget->getCurrentTab() - 1);
            }
            break;
        case Gui::Controller::KeyRight:
            if (_pControllerWidget->getCurrentTab() < _pControllerWidget->getTabCount() - 1) {
                _pControllerWidget->setCurrentTab(_pControllerWidget->getCurrentTab() + 1);
            }
            break;
        case Gui::Controller::KeyUp:
        case Gui::Controller::KeyDown:
        case Gui::Controller::KeyReturn:
            _pControllerWidget->navigateListWithKey(key);
            break;
        case Gui::Controller::KeyBack:
            _pControllerWidget->back();
            break;
    }
}


DeviceGroupWidget::DeviceGroupWidget(DeviceGroupDelegate* pDeviceGroupDelegate) :
DeviceGroup(pDeviceGroupDelegate)
{
    Gui::Log::instance()->gui().debug("device group widget ctor (delegate)");
    attachController(this);
}


void
DeviceGroupWidget::addDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        syncViews();
        if (pDevice->getUuid() == _defaultDeviceUuid) {
            _deviceGroupListView.selectRow(index);
        }
    }
}


void
DeviceGroupWidget::removeDevice(Device* pDevice, int index, bool begin)
{
    if (!begin) {
        if (getSelectedDevice() && pDevice->getUuid() == getSelectedDevice()->getUuid()) {
            popToRoot();
        }
        syncViews();
    }
}


void
DeviceGroupWidget::showDeviceGroup()
{
    Gui::Log::instance()->gui().debug("device group widget show device group");
    ControllerWidget* pController = static_cast<ControllerWidget*>(getController());
    pController->addView(this, shortName());
}


int
DeviceGroupWidget::totalItemCount()
{
    Gui::Log::instance()->gui().debug("device group widget total item count returns: " + Poco::NumberFormatter::format(getDeviceCount()));
    return getDeviceCount();
}


void
DeviceGroupWidget::selectedItem(int row)
{
    Gui::Log::instance()->gui().debug("device group widget selected device");
    Device* pDevice = static_cast<Device*>(getDevice(row));
    DeviceGroup::selectDevice(pDevice);
}


void
DeviceGroupWidget::setDefaultDevice(Device* pDevice)
{
    _defaultDeviceUuid = pDevice->getUuid();
}


DeviceGroupNavigatorController::DeviceGroupNavigatorController(DeviceGroupWidget* pDeviceGroupWidget) :
_pDeviceGroupWidget(pDeviceGroupWidget)
{
}


void
DeviceGroupNavigatorController::changedSearchText(const std::string& searchText)
{
    _pDeviceGroupWidget->changedSearchText(searchText);
}


MediaRendererGroupWidget::MediaRendererGroupWidget(ControllerWidget* pControllerWidget) :
DeviceGroupWidget(new Av::MediaRendererGroupDelegate),
_pControllerWidget(pControllerWidget)
{
//    Gui::Log::instance()->gui().debug("media renderer group widget ctor");
    View::setName("media renderer group view");
    _deviceGroupListView.setName("media renderer group view");
    push(&_deviceGroupListView, "Player");

    _deviceGroupListView.setItemViewHeight(80);
    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
}


Device*
MediaRendererGroupWidget::createDevice()
{
//    Gui::Log::instance()->gui().debug("media renderer group widget create renderer device.");
    return new MediaRendererDevice(_pControllerWidget);
}


Gui::View*
MediaRendererGroupWidget::createItemView()
{
//    Gui::Log::instance()->gui().debug("media renderer group widget create renderer view.");
    return new MediaRendererView;
}


Gui::Model*
MediaRendererGroupWidget::getItemModel(int row)
{
    return static_cast<MediaRendererDevice*>(getDevice(row));
}


void
MediaRendererGroupWidget::selectedItem(int row)
{
    DeviceGroupWidget::selectedItem(row);
    _pControllerWidget->getControlPanel()->setModel(static_cast<MediaRendererDevice*>(getDevice(row)));
}


void
MediaRendererDevice::initController()
{
    _rendererName.setLabel(getFriendlyName());
    _volume.setValue(getVolume());
    _position.setValue(0);
}


void
MediaRendererDevice::newUri(const std::string& uri)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new uri: " + uri);
//    _trackName.setLabel(uri);
    Av::Connection* pConnection = getConnectionManager()->getConnection(0);
    if (pConnection) {
        std::string serverUuid = pConnection->getServer().getConnectionManagerId().getUuid();
        MediaServerDevice* pServer = _pControllerWidget->getServer(serverUuid);
        if (pServer) {
            Gui::Log::instance()->gui().debug("media renderer device connected to server: \"" + pServer->getFriendlyName() + "\"");
            Av::CtlMediaObject2* pObject = pServer->getMediaObjectFromResource(uri);
            if (pObject) {
                Gui::Log::instance()->gui().debug("media renderer device playing object with title: \"" + pObject->getTitle() + "\"");
                _trackName.setLabel(pObject->getTitle());
            }
        }
    }
    syncViews();
}


void
MediaRendererDevice::newTrack(const std::string& title, const std::string& artist, const std::string& album)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new track: " + title + ", " + artist + ", " + album);
    _trackName.setLabel(artist == "" ? title : artist + " - " + title);
    syncViews();
}


void
MediaRendererDevice::newPosition(int duration, int position)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new position: " + Poco::NumberFormatter::format(position) + ", duration: " + Poco::NumberFormatter::format(duration));
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" position slider: " + Poco::NumberFormatter::format(((float)position / duration) * 100.0));
    _duration = duration;
    if (duration == 0) {
        _position.setValue(0);
    }
    else {
        _position.setValue(((float)position / duration) * 100);
    }
    syncViews();
}


void
MediaRendererDevice::newVolume(const int volume)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new volume: " + Poco::NumberFormatter::format(volume));
//    _volume = volume;
    _volume.setValue(volume);
    syncViews();
}


void
MediaRendererDevice::newTransportState(const std::string& transportState)
{
    Gui::Log::instance()->gui().debug("media renderer device \"" + getFriendlyName() + "\" new transport state: " + transportState);
    _transportState = transportState;
    syncViews();
    startPositionTimer(transportState == Av::AvTransportArgument::TRANSPORT_STATE_PLAYING);
    Poco::NotificationCenter::defaultCenter().postNotification(new TransportStateNotification(getUuid(), transportState));
}


std::string
MediaRendererDevice::getTransportState()
{
    // FIXME: track name should be retrieved from CtlService::TransportState of the corresponding AVTransport instance.
    //        Don't store it in a separate class member.
    return _transportState;
}


class BackButton : public Gui::Button
{
public:
    BackButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-backward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->backPressed();
        pRenderer->stopPressed();
        pRenderer->playPressed();
    }

    Gui::Image _image;
};


class PlayButton : public Gui::Button
{
public:
    PlayButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-start.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->playPressed();
    }

    virtual bool getEnabled()
    {
        return true;

        Gui::Log::instance()->gui().debug("media renderer play button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            Gui::Log::instance()->gui().debug("media renderer play button get enabled, transport state: " + transportState);
            return (transportState == Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
    }

    Gui::Image _image;
};


class StopButton : public Gui::Button
{
public:
    StopButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-stop.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->stopPressed();
    }

    virtual bool getEnabled()
    {
        return true;

        Gui::Log::instance()->gui().debug("media renderer stop button get enabled");
        if (_pParent && _pParent->getModel()) {
            std::string transportState = static_cast<MediaRendererDevice*>(_pParent->getModel())->getTransportState();
            Gui::Log::instance()->gui().debug("media renderer stop button get enabled, transport state: " + transportState);
            return (transportState != Av::AvTransportArgument::TRANSPORT_STATE_STOPPED);
        }
        else {
            return Button::getEnabled();
        }
    }

    Gui::Image _image;
};


class ForwButton : public Gui::Button
{
public:
    ForwButton(Gui::View* pParent = 0) : Gui::Button(pParent)
    {
        _image.setData(MediaImages::instance()->getResource("media-skip-forward.png"));
        setImage(&_image);
        setSizeConstraint(45, height(Gui::View::Pref), Gui::View::Pref);
//        setEnabled(false);
    }

    virtual void pushed()
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->forwardPressed();
        pRenderer->stopPressed();
        pRenderer->playPressed();
    }

    Gui::Image _image;
};


class RendererButton : public Gui::Button
{
public:
    RendererButton(Gui::View* pParent = 0) : Gui::Button(pParent) {}

    virtual void pushed()
    {
        MediaRendererView* pRenderer = static_cast<MediaRendererView*>(_pParent);
        pRenderer->selectedRenderer();
    }
};


class VolSlider : public Gui::Slider
{
public:
    VolSlider(Gui::View* pParent = 0) : Gui::Slider(pParent)
    {
//        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->volumeChanged(value);
    }

    virtual bool getEnabled()
    {
        if (_pModel) {
            return true;
        }
        else {
            return false;
        }
    }
};


class SeekSlider : public Gui::Slider
{
public:
    SeekSlider(Gui::View* pParent = 0) : Gui::Slider(pParent)
    {
//        setEnabled(false);
    }

    virtual void valueChanged(int value)
    {
        MediaRendererDevice* pRenderer = static_cast<MediaRendererDevice*>(_pParent->getModel());
        pRenderer->positionMoved(value / 100.0 * pRenderer->_duration);
        // TODO: seek position value should be multiplied with lenght of track
    }
};


MediaRendererView::MediaRendererView()
{
    setName("media renderer view");

    _pBackButton = new BackButton(this);
    _pPlayButton = new PlayButton(this);
    _pStopButton = new StopButton(this);
    _pForwButton = new ForwButton(this);

    _pVolSlider = new VolSlider(this);
    _pSeekSlider = new SeekSlider(this);

//    _pRendererName = new RendererName(this);
    _pRendererName = new Gui::LabelView(this);
    _pRendererName->setAlignment(Gui::View::AlignCenter);

    _pTrackName = new Gui::LabelView(this);
    _pTrackName->setAlignment(Gui::View::AlignCenter);

    setSizeConstraint(800, 40, Gui::View::Pref);
    resize(800, 40);
    setLayout(&_layout);
}


void
MediaRendererView::setModel(Gui::Model* pModel)
{
    _pRendererName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_rendererName);
    _pTrackName->setModel(&static_cast<MediaRendererDevice*>(pModel)->_trackName);
    _pSeekSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_position);
    _pVolSlider->setModel(&static_cast<MediaRendererDevice*>(pModel)->_volume);
    Gui::View::setModel(pModel);
}


void
MediaRendererView::selectedRenderer()
{
    NOTIFY_CONTROLLER(Gui::Controller, selected);
}


void
MediaRendererView::syncViewImpl()
{
    Gui::Log::instance()->gui().debug("media renderer view sync view impl");
    // FIXME: submodels should by synced implicitely
    _pRendererName->syncViewImpl();
    _pPlayButton->syncViewImpl();
    _pStopButton->syncViewImpl();
    _pSeekSlider->syncViewImpl();
    _pVolSlider->syncViewImpl();
    _pTrackName->syncViewImpl();
}


MediaServerGroupWidget::MediaServerGroupWidget() :
DeviceGroupWidget(new Av::MediaServerGroupDelegate)
{
//    Gui::Log::instance()->gui().debug("media server group widget ctor");
    View::setName("media server group view");
    _deviceGroupListView.setName("media server group view");
    push(&_deviceGroupListView, "Media");

    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
    attachController(new DeviceGroupNavigatorController(this));

    // TODO: get search capabilities
    _searchString = Poco::Util::Application::instance().config().getString("controller.searchString", "dc:title contains %s");
}


Device*
MediaServerGroupWidget::createDevice()
{
    Gui::Log::instance()->gui().debug("media server group widget create server device.");
    return new MediaServerDevice(this);
}


Gui::View*
MediaServerGroupWidget::createItemView()
{
    Gui::Log::instance()->gui().debug("media server group widget create server view.");
    return new MediaServerView;
}


Gui::Model*
MediaServerGroupWidget::getItemModel(int row)
{
    return static_cast<MediaServerDevice*>(getDevice(row));
}


void
MediaServerGroupWidget::selectedItem(int row)
{
    Gui::Log::instance()->gui().debug("media server group widget selected device");
    MediaServerDevice* pServer = static_cast<MediaServerDevice*>(getDevice(row));
    DeviceGroup::selectDevice(pServer);
    pServer->browseRootObject();
    MediaObjectModel* pRootObject = static_cast<MediaObjectModel*>(pServer->getRootObject());
    if (pRootObject->isContainer()) {
        Gui::Log::instance()->gui().debug("media server group widget selected device has container as root object");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        pContainer->_pObjectModel = pRootObject;
        pContainer->_pServerGroup = this;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        // TODO: hide search box when view is popped
        push(pContainer, pServer->getFriendlyName());
        if (pRootObject->isSearchable()) {
            showSearchBox();
        }
    }
}


void
MediaServerGroupWidget::changedSearchText(const std::string& searchText)
{
    Gui::Log::instance()->gui().debug("media server group widget changed search text: " + searchText);

    if (searchText == "") {
        return;
    }

    MediaServerDevice* pServer = static_cast<MediaServerDevice*>(getSelectedDevice());
    if (pServer) {
        // get (object id of) container, that is on top of navigator
        MediaContainerWidget* pContainer = static_cast<MediaContainerWidget*>(getVisibleView());
        Av::CtlMediaObject2* pObject = pContainer->_pObjectModel;

        std::string searchExp = Poco::replace(_searchString, std::string("%s"), "\"" + searchText + "\"");
        Gui::Log::instance()->gui().debug("search expression: " + searchExp);
        pObject->setSearch(searchExp);

        // clear cache (reset data model)
        pObject->clear();
        // if total item count of list model is 0, no items are fetched and thus total item count is not updated
        // so we need to fetch first child in the current search context
        pObject->getChildForRow(0);
//        pContainer->scrollToRowOffset(0);
        pContainer->resetListView();
        // sync view
        pContainer->syncViewImpl();
    }
}


void
MediaServerDevice::initController()
{
    Gui::LabelModel* pLabelModel = new Gui::LabelModel;
    pLabelModel->setLabel(getFriendlyName());
    setLabelModel(pLabelModel);

    Icon* pIcon = 0;
    for (IconIterator it = beginIcon(); it != endIcon(); ++it) {
        pIcon = *it;
    }
    Gui::ImageModel* pImageModel = new Gui::ImageModel;
    if (pIcon) {
        pImageModel->setData(pIcon->getBuffer());
    }
    else {
        pImageModel->setData(MediaImages::instance()->getResource("media-server.png"));
    }
    setImageModel(pImageModel);
}


Av::CtlMediaObject2*
MediaServerDevice::createMediaObject()
{
    return new MediaObjectModel;
}


void
MediaServerDevice::newSystemUpdateId(ui4 id)
{
    Gui::Log::instance()->gui().debug("media server device \"" + getFriendlyName() + "\" new system update id: " + Poco::NumberFormatter::format(id));

    // get (object id of) container, that is on top of navigator
    // can also be a MediaServerGroupWidget, thus the dynamic_cast
    MediaContainerWidget* pContainer = dynamic_cast<MediaContainerWidget*>(_pServerGroupWidget->getVisibleView());
    if (pContainer && _pServerGroupWidget->getSelectedDevice() && _pServerGroupWidget->getSelectedDevice()->getUuid() == getUuid()) {
        Av::CtlMediaObject2* pObject = pContainer->_pObjectModel;
        if (pObject) {
            // clear cache (reset data model)
            pObject->clear();
            // if total item count of list model is 0, no items are fetched and thus total item count is not updated
            // so we need to fetch first child in the current search context
            pObject->getChildForRow(0);
            // sync view
            pContainer->syncView();
        }
    }
}


MediaContainerWidget::MediaContainerWidget(View* pParent) :
ListView(pParent)
{
//    setItemViewHeight(30);
}


int
MediaContainerWidget::totalItemCount()
{
    Gui::Log::instance()->gui().debug("media container widget total item count: " + Poco::NumberFormatter::format(_pObjectModel->getChildCount()));
    return _pObjectModel->getChildCount();
}


Gui::View*
MediaContainerWidget::createItemView()
{
    return new MediaObjectView;
}


Gui::Model*
MediaContainerWidget::getItemModel(int row)
{
    Gui::Log::instance()->gui().debug("media container widget get item model in row: " + Poco::NumberFormatter::format(row));
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pObjectModel->getChildForRow(row));
    if (!pModel) {
        Gui::Log::instance()->gui().error("media container widget failed to get item model in row: " + Poco::NumberFormatter::format(row));
        return 0;
    }
    // FIXME: this can be done once, when the model is created.
    Av::AbstractProperty* pClass = pModel->getProperty(Av::AvProperty::CLASS);
    if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::CONTAINER)) {
        pModel->setImageModel(MediaObjectModel::_pContainerImageModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::AUDIO_ITEM)) {
        pModel->setImageModel(MediaObjectModel::_pItemAudioItemModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::IMAGE_ITEM)) {
        Omm::Icon* pIcon = pModel->getIcon();
//        Omm::Icon* pIcon = pModel->getImageRepresentation();
        if (pIcon) {
            Omm::Gui::ImageModel* pImage = new Omm::Gui::ImageModel;
            pImage->setData(pIcon->getBuffer());
            pModel->setImageModel(pImage);
        }
        else {
            pModel->setImageModel(MediaObjectModel::_pItemImageItemModel);
        }
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::VIDEO_ITEM)) {
        pModel->setImageModel(MediaObjectModel::_pItemVideoItemModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::AUDIO_BROADCAST)) {
        pModel->setImageModel(MediaObjectModel::_pItemAudioBroadcastModel);
    }
    else if (Av::AvClass::matchClass(pClass->getValue(), Av::AvClass::ITEM, Av::AvClass::VIDEO_BROADCAST)) {
        pModel->setImageModel(MediaObjectModel::_pItemVideoBroadcastModel);
    }
    else {
        pModel->setImageModel(MediaObjectModel::_pItemImageModel);
    }
    return pModel;
}


void
MediaContainerWidget::selectedItem(int row)
{
    Gui::Log::instance()->gui().debug("media container widget selected media object");
    MediaObjectModel* pChildObject = static_cast<MediaObjectModel*>(getItemModel(row));
    if (pChildObject->isContainer()) {
        Gui::Log::instance()->gui().debug("media container widget selected media container");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        pContainer->_pObjectModel = pChildObject;
        pContainer->_pServerGroup = _pServerGroup;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        // don't rely on childCount attribute being present and fetch first children to get total child count
        pChildObject->getChildForRow(0);
        _pServerGroup->push(pContainer, pChildObject->getTitle());
    }
    else {
        Gui::Log::instance()->gui().debug("media container widget selected media item");
        _pObjectModel->getServer()->selectMediaObject(pChildObject, _pObjectModel, row);
    }
}


Gui::ImageModel*   MediaObjectModel::_pContainerImageModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemImageModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemAudioItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemImageItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemVideoItemModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemAudioBroadcastModel = 0;
Gui::ImageModel*   MediaObjectModel::_pItemVideoBroadcastModel = 0;

MediaObjectModel::MediaObjectModel()
{
    setLabelModel(new MediaObjectLabelModel(this));

    if (!_pContainerImageModel) {
        _pContainerImageModel = new Gui::Image;
        _pContainerImageModel->setData(MediaImages::instance()->getResource("media-container.png"));
    }
    if (!_pItemImageModel) {
        _pItemImageModel = new Gui::Image;
        _pItemImageModel->setData(MediaImages::instance()->getResource("media-item.png"));
    }
    if (!_pItemAudioItemModel) {
        _pItemAudioItemModel = new Gui::Image;
        _pItemAudioItemModel->setData(MediaImages::instance()->getResource("media-audio-item.png"));
    }
    if (!_pItemImageItemModel) {
        _pItemImageItemModel = new Gui::Image;
        _pItemImageItemModel->setData(MediaImages::instance()->getResource("media-image-item.png"));
    }
    if (!_pItemVideoItemModel) {
        _pItemVideoItemModel = new Gui::Image;
        _pItemVideoItemModel->setData(MediaImages::instance()->getResource("media-video-item.png"));
    }
    if (!_pItemAudioBroadcastModel) {
        _pItemAudioBroadcastModel = new Gui::Image;
        _pItemAudioBroadcastModel->setData(MediaImages::instance()->getResource("media-audio-broadcast.png"));
    }
    if (!_pItemVideoBroadcastModel) {
        _pItemVideoBroadcastModel = new Gui::Image;
        _pItemVideoBroadcastModel->setData(MediaImages::instance()->getResource("media-video-broadcast.png"));
    }
}


MediaObjectModel::MediaObjectModel(const MediaObjectModel& objectModel) :
Av::CtlMediaObject2(objectModel),
Gui::ListItemModel(objectModel),
_pContainer(objectModel._pContainer)
{
    setLabelModel(new MediaObjectLabelModel(this));
}


std::string
MediaObjectModel::MediaObjectLabelModel::getLabel()
{
    Av::AbstractProperty* pArtist = _pSuperModel->getProperty(Av::AvProperty::ARTIST);
    if (pArtist) {
        return  pArtist->getValue() + " - " + _pSuperModel->getTitle();
    }
    else {
        return _pSuperModel->getTitle();
    }
}


MediaObjectViewPlaylistButtonController::MediaObjectViewPlaylistButtonController(MediaObjectView* pMediaObjectView) :
_pMediaObjectView(pMediaObjectView)
{
}


void
MediaObjectViewPlaylistButtonController::pushed()
{
    Gui::Log::instance()->gui().debug("media object playlist button pushed.");
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pMediaObjectView->getModel());
    std::string objectClass = pModel->getClass();
    Gui::Log::instance()->gui().debug("media object has class: " + objectClass);

//    Gui::Image* pImage = new Gui::Image;
//    pImage->setData(MediaImages::instance()->getResource("media-container.png"));
//    pModel->setImageModel(pImage);
    Poco::NotificationCenter::defaultCenter().postNotification(new PlaylistNotification(pModel));
}


MediaObjectView::MediaObjectView(View* pParent)
{
    _pPlaylistButton = new Gui::Button(this);
    _pPlaylistButton->setLabel("P");
    _pPlaylistButton->setBackgroundColor(Gui::Color("blue"));
    _pPlaylistButton->setStretchFactor(-1.0);
    _pPlaylistButton->resize(20, 15);
    _pPlaylistButton->attachController(new MediaObjectViewPlaylistButtonController(this));
}


PlaylistNotification::PlaylistNotification(MediaObjectModel* pMediaObject) :
_pMediaObject(pMediaObject)
{
}


PlaylistEditor::PlaylistEditor(ControllerWidget* pControllerWidget) :
_pControllerWidget(pControllerWidget),
_pPlaylistContainer(0)
{
    Poco::NotificationCenter::defaultCenter().addObserver(Poco::Observer<PlaylistEditor,
            PlaylistNotification>(*this, &PlaylistEditor::playlistNotification));
    setModel(this);
    attachController(this);
}


int
PlaylistEditor::totalItemCount()
{
    Gui::Log::instance()->gui().debug("playlist editor total item count: " + Poco::NumberFormatter::format(_playlistItems.size()));
    return _playlistItems.size();
}


Gui::View*
PlaylistEditor::createItemView()
{
//    return new Gui::ListItemView;
    return new PlaylistEditorObjectView(this);
}


Gui::Model*
PlaylistEditor::getItemModel(int row)
{
    Gui::Log::instance()->gui().debug("playlist editor get item model in row: " + Poco::NumberFormatter::format(row));
    Gui::Log::instance()->gui().debug("playlist editor get item with title: " + _playlistItems[row]->getTitle());
    return _playlistItems[row];
}


void
PlaylistEditor::playlistNotification(PlaylistNotification* pNotification)
{
    MediaObjectModel* pModel = pNotification->_pMediaObject;
    if (pModel->isContainer()) {
        if (Av::AvClass::matchClass(pModel->getClass(), Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)) {
            if (pModel->getResource() && pModel->getResource()->getAttributeValue(Av::AvProperty::IMPORT_URI) != "") {
                Gui::Log::instance()->gui().debug("playlist editor load playlist: " + pModel->getTitle());
                setPlaylistContainer(pModel);
                Av::AbstractMediaObject* pObject = pModel->getChildForRow(0);
                Gui::Log::instance()->gui().debug("media object playlist button pushed, container with count children: " + Poco::NumberFormatter::format(pModel->getChildCount()));
                for (int r = 0; r < pModel->getChildCount(); r++) {
                    Gui::Log::instance()->gui().debug("title: " + pModel->getChildForRow(r)->getTitle());
                    _playlistItems.push_back(new MediaObjectModel(*static_cast<MediaObjectModel*>(pModel->getChildForRow(r))));
                }
            }
        }
    }
    else if (_pPlaylistContainer) {
        Gui::Log::instance()->gui().debug("media object playlist add item with title: " + pModel->getTitle());
        _playlistItems.push_back(new MediaObjectModel(*pModel));
        _pPlaylistContainer->writeResource(_pControllerWidget->_pApplication->getAppHttpUri() + UpnpApplication::PLAYLIST_URI);

        // FIXME: why does this crash?
//        _pPlaylistContainer->writeResource(getPlaylistResourceUri());
    }
    syncViewImpl();
}


void
PlaylistEditor::setPlaylistContainer(MediaObjectModel* pPlaylistContainer)
{
    Gui::Log::instance()->gui().debug("set playlist container with title: " + pPlaylistContainer->getTitle());
    _pPlaylistContainer = new MediaObjectModel(*pPlaylistContainer);
}


std::stringstream*
PlaylistEditor::getPlaylistResource()
{
    std::stringstream* pPlaylistResource = new std::stringstream;
    for (std::vector<MediaObjectModel*>::iterator it = _playlistItems.begin(); it != _playlistItems.end(); ++it) {
        *pPlaylistResource << (*it)->getResource()->getUri() << std::endl;
    }
    return pPlaylistResource;
}


std::string
PlaylistEditor::getPlaylistResourceUri()
{
    _pControllerWidget->_pApplication->getAppHttpUri() + UpnpApplication::PLAYLIST_URI;
}


void
PlaylistEditor::deleteItem(MediaObjectModel* pModel)
{
    Gui::Log::instance()->gui().debug("delete media object from playlist with title: " + pModel->getTitle());
    std::vector<MediaObjectModel*>::iterator pos = std::find(_playlistItems.begin(), _playlistItems.end(), pModel);
    if (pos != _playlistItems.end()) {
        _playlistItems.erase(pos);
        _pPlaylistContainer->writeResource(_pControllerWidget->_pApplication->getAppHttpUri() + UpnpApplication::PLAYLIST_URI);
        delete pModel;
        syncViewImpl();
    }
}


PlaylistEditorDeleteObjectController::PlaylistEditorDeleteObjectController(PlaylistEditorObjectView* pPlaylistEditorObjectView) :
_pPlaylistEditorObjectView(pPlaylistEditorObjectView)
{
}


void
PlaylistEditorDeleteObjectController::pushed()
{
    Gui::Log::instance()->gui().debug("playlist editor delete button pushed.");
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pPlaylistEditorObjectView->getModel());
    _pPlaylistEditorObjectView->_pPlaylistEditor->deleteItem(pModel);
}


PlaylistEditorObjectView::PlaylistEditorObjectView(PlaylistEditor* pPlaylistEditor, View* pParent) :
_pPlaylistEditor(pPlaylistEditor)
{
    _pDeleteButton = new Gui::Button(this);
    _pDeleteButton->setLabel("D");
    _pDeleteButton->setBackgroundColor(Gui::Color("blue"));
    _pDeleteButton->setStretchFactor(-1.0);
    _pDeleteButton->resize(20, 15);
    _pDeleteButton->attachController(new PlaylistEditorDeleteObjectController(this));
}


GuiVisual::GuiVisual(Gui::View* pParent)
{
    setBackgroundColor(Gui::Color("black"));
}


GuiVisual::~GuiVisual()
{
}


void
GuiVisual::show()
{
    Gui::View::show();
}


void
GuiVisual::hide()
{
    Gui::View::hide();
}


void*
GuiVisual::getWindow()
{
    return Gui::View::getNativeView();
}


GuiVisual::WindowHandle
GuiVisual::getWindowId()
{
    return (GuiVisual::WindowHandle)Gui::View::getNativeWindowId();
//    return winId();
}


Sys::Visual::VisualType
GuiVisual::getType()
{
    // GuiVisual is multi-platform, and type of visual is platform dependent.
#ifdef __LINUX__
    return Omm::Sys::Visual::VTX11;
#elif __DARWIN__
    return Omm::Sys::Visual::VTOSX;
#elif __WINDOWS__
    return Omm::Sys::Visual::VTWin;
#else
    return Omm::Sys::Visual::VTNone;
#endif
}


void
GuiVisual::renderImage(const std::string& imageData)
{
}


void
GuiVisual::blank()
{
}


ActivityIndicator::ActivityIndicator(Gui::View* pParent) :
Gui::ImageView(pParent),
_indicateDuration(250),
_activityInProgress(false),
_indicatorOn(false),
_timerActive(false),
_pOffTimer(0),
_stopIndicatorCallback(*this, &ActivityIndicator::stopIndicator)
{
    _pActivityOffModel = new Gui::ImageModel;
    _pActivityOffModel->setData(MediaImages::instance()->getResource("activity-off.png"));
    _pActivityOnModel = new Gui::ImageModel;
    _pActivityOnModel->setData(MediaImages::instance()->getResource("activity-on.png"));
//    resize(15, 15);
    setModel(_pActivityOffModel);
}


ActivityIndicator::~ActivityIndicator()
{
}


void
ActivityIndicator::startActivity()
{
//    Gui::Log::instance()->gui().debug("activity indicator start activity");

    setActivityInProgress(true);
    if (!indicatorOn()) {
        setIndicatorOn(true);
        setModel(_pActivityOnModel);
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator already on, do nothing");
    }
}


void
ActivityIndicator::stopActivity()
{
//    Gui::Log::instance()->gui().debug("activity indicator stop activity");

    setActivityInProgress(false);
    if (indicatorOn() && !timerActive()) {
//        Gui::Log::instance()->gui().debug("turn off indicator after short delay ...");
        setTimerActive(true);
        if (_pOffTimer) {
            delete _pOffTimer;
        }
        _pOffTimer = new Poco::Timer;
        _pOffTimer->setStartInterval(_indicateDuration);
        _pOffTimer->start(_stopIndicatorCallback);
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator already off or timer running, do nothing");
    }
}


void
ActivityIndicator::stopIndicator(Poco::Timer& timer)
{
//    Gui::Log::instance()->gui().debug("activity indicator stop timer callback ...");

    if (!activityInProgress() && indicatorOn()) {
        setModel(_pActivityOffModel);
        setIndicatorOn(false);
//        Gui::Log::instance()->gui().debug("indicator turned off, no activity in progress anymore");
    }
    else {
//        Gui::Log::instance()->gui().debug("turn off indicator ignored, activity still in progress or indicator already off");
    }
    setTimerActive(false);
//    Gui::Log::instance()->gui().debug("activity indicator stop timer callback finished.");
}


void
ActivityIndicator::setActivityInProgress(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    if (set) {
//        Gui::Log::instance()->gui().debug("indicator flag \"activity in progress\" set to true");
    }
    else {
//        Gui::Log::instance()->gui().debug("indicator flag \"activity in progress\" set to false");
    }
    _activityInProgress = set;
}


bool
ActivityIndicator::activityInProgress()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    return _activityInProgress;
}


void
ActivityIndicator::setIndicatorOn(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    if (set) {
//        Gui::Log::instance()->gui().debug("flag \"indicator on\" set to true");
    }
    else {
//        Gui::Log::instance()->gui().debug("flag \"indicator on\" set to false");
    }
    _indicatorOn = set;
}


bool
ActivityIndicator::indicatorOn()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    return _indicatorOn;
}


bool
ActivityIndicator::timerActive()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    return _timerActive;
}


void
ActivityIndicator::setTimerActive(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    _timerActive = set;
}



} // namespace Omm
