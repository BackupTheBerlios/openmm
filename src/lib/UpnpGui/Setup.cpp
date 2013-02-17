/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2013                                                       |
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

#include <vector>
#include <Poco/StringTokenizer.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/PropertyFileConfiguration.h>

#include "Gui/ListItem.h"
#include "Gui/VerticalLayout.h"

#include "UpnpGui/UpnpApplication.h"
#include "UpnpGui/ControllerWidget.h"
#include "UpnpGui/Setup.h"


namespace Omm {

class ServerListModel : public Gui::ListModel
{
    friend class GuiSetup;

    ServerListModel();

    void clearConfigItems();
    void readConfig();
    void appendConfigItem(const std::string& label);

    virtual int totalItemCount()
    {
        return _configServerItems.size();
    }


    virtual Gui::Model* getItemModel(int row)
    {
        return _configServerItems[row];
    }


    virtual Gui::View* createItemView()
    {
        return new Gui::ListItemView;
    }

    std::vector<Gui::ListItemModel*>    _configServerItems;
    Poco::Util::LayeredConfiguration&   _config;
};


ServerListModel::ServerListModel() :
_config(Poco::Util::Application::instance().config())
{
}


void
ServerListModel::clearConfigItems()
{
    for (std::vector<Gui::ListItemModel*>::iterator it = _configServerItems.begin(); it != _configServerItems.end(); ++it) {
        delete *it;
    }
    _configServerItems.clear();
}


void
ServerListModel::readConfig()
{
    clearConfigItems();

    std::vector<std::string> serverKeys;
    _config.keys("server", serverKeys);
    for (std::vector<std::string>::iterator it = serverKeys.begin(); it != serverKeys.end(); ++it) {
        std::vector<std::string> serverConfigKeys;
        _config.keys("server." + *it, serverConfigKeys);
        if (*it != "new") {
            appendConfigItem(_config.getString("server." + *it + ".friendlyName", ""));
        }

//        for (std::vector<std::string>::iterator cit = serverConfigKeys.begin(); cit != serverConfigKeys.end(); ++cit) {
//        }
    }
}


void
ServerListModel::appendConfigItem(const std::string& label)
{
    Gui::ListItemModel* pConfigItemModel = new Gui::ListItemModel;
    pConfigItemModel->setLabelModel(new Gui::LabelModel(label));
    _configServerItems.push_back(pConfigItemModel);
}


GuiSetup::GuiSetup(Gui::View* pParent) :
NavigatorView(pParent)
{
    _pSetupView = new View;
    _pSetupView->setName("Setup");
    _pSetupView->setLayout(new Gui::VerticalLayout);

    _pServerListModel = new ServerListModel;
    _pServerListModel->readConfig();
    _pServerList = new Gui::ListView(_pSetupView);
    _pServerList->setModel(_pServerListModel);

    showNavigatorBar(false);
    push(_pSetupView, "Setup");
}


GuiSetup::~GuiSetup()
{
}


WebSetup::WebSetup(UpnpApplication* pApp, ControllerWidget* pControllerWidget) :
_pApp(pApp),
_pControllerWidget(pControllerWidget)
{
}


std::stringstream*
WebSetup::generateConfigPage()
{
    Poco::Util::PropertyFileConfiguration* _pConf = _pApp->getFileConfiguration();

    std::stringstream* pOutStream = new std::stringstream;
    *pOutStream << "<html>\n"
                    "<head>\n"
                    "<title>OMM Configuration</title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<h1>OMM Configuration</h1>\n";

    *pOutStream << "Address of this page is " << _pApp->getAppHttpUri() << UpnpApplication::CONFIG_URI << "<br>\n";

    if (!_pConf) {
        *pOutStream << "configuration file not found<br>\n";
        return pOutStream;
    }

    *pOutStream << "<form method=\"POST\" action=\"" + UpnpApplication::CONFIG_URI + "?" + UpnpApplication::CONFIG_APP_QUERY + "\"><br>\n";

    std::string appState = _pConf->getString("application.state", "Started");
    *pOutStream << std::string("state<select name=\"application.state\" size=\"1\">") +
                "<option " + (appState == DeviceManager::Started ? "selected" : "") + ">" + DeviceManager::Started + "</option>" +
                "<option " + (appState == DeviceManager::Local ? "selected" : "") + ">" + DeviceManager::Local + "</option>" +
                "<option " + (appState == DeviceManager::Stopped ? "selected" : "") + ">" + DeviceManager::Stopped + "</option>" +
                "</select><br>\n";

    *pOutStream << "<input type=\"submit\" value=\"Save\">\n";
    *pOutStream << "</form>\n";

    *pOutStream << "<form method=\"POST\" action=\"" + UpnpApplication::CONFIG_URI + "?" + UpnpApplication::CONFIG_DEV_QUERY + "\">\n";

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
WebSetup::handleAppConfigRequest(const Poco::Net::HTMLForm& form)
{
    Poco::Util::PropertyFileConfiguration* _pConf = _pApp->getFileConfiguration();

    if (!form.empty()) {
//        printForm(form);
        for (Poco::Net::NameValueCollection::ConstIterator it = form.begin(); it != form.end(); ++it) {
            _pConf->setString(it->first, it->second);
            if (it->first == "application.state") {
                _pApp->setState(it->second);
            }
        }
    }
}


void
WebSetup::handleDevConfigRequest(const Poco::Net::HTMLForm& form)
{
    Poco::Util::PropertyFileConfiguration* _pConf = _pApp->getFileConfiguration();

    if (!form.empty()) {
        LOGNS(Av, upnpav, debug, "omm local devices config update ...");
//        printConfig();
//        printForm(form);

        // read form data and write all entries to file configuration
        LOGNS(Av, upnpav, debug, "omm config read in devices form config ...");
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
            LOGNS(Av, upnpav, debug, "omm config create new local device with id: " + newId);
            std::vector<std::string> newServerKeys;
            _pConf->keys("server.new", newServerKeys);
            for (std::vector<std::string>::iterator it = newServerKeys.begin(); it != newServerKeys.end(); ++it) {
                LOGNS(Av, upnpav, debug, std::string("add config entry ") + "server." + newId + "." + *it + ": " + _pConf->getString(*it, ""));
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
        _pApp->restartLocalServers();

        LOGNS(Av, upnpav, debug, "omm local devices config update done.");
    }
}

} // namespace Omm
