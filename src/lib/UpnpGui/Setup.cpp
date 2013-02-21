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

#include <string>
#include <vector>

#include <Poco/StringTokenizer.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/PropertyFileConfiguration.h>

#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/ListItem.h"
#include "Gui/Switch.h"
#include "Gui/Button.h"
#include "Gui/TextLine.h"

#include "UpnpGui/UpnpApplication.h"
#include "UpnpGui/ControllerWidget.h"
#include "UpnpGui/Setup.h"


namespace Omm {

class AppStateSelector : public Gui::Selector
{
    friend class GuiSetup;

    AppStateSelector(UpnpApplication* pApp, View* pParent) :
    Selector(pParent),
    _pApp(pApp)
    {
        syncView();
        std::string state = _pApp->getFileConfiguration()->getString("application.state", "Started");
        if (state == DeviceManager::Started) {
            setCurrentIndex(0);
        }
        else if (state == DeviceManager::Local) {
            setCurrentIndex(1);
        }
        else if (state == DeviceManager::Stopped) {
            setCurrentIndex(2);
        }
    }


    virtual int totalItemCount()
    {
        return 3;
    }


    virtual std::string getItemLabel(int index)
    {
        switch(index) {
            case 0:
                return DeviceManager::Started;
                break;
            case 1:
                return DeviceManager::Local;
                break;
            case 2:
                return DeviceManager::Stopped;
                break;
        }
    }


    virtual void selected(int index)
    {
        switch(index) {
            case 0:
                _pApp->setState(DeviceManager::Started);
                _pApp->getFileConfiguration()->setString("application.state", DeviceManager::Started);
                break;
            case 1:
                _pApp->setState(DeviceManager::Local);
                _pApp->getFileConfiguration()->setString("application.state", DeviceManager::Local);
                break;
            case 2:
                _pApp->setState(DeviceManager::Stopped);
                _pApp->getFileConfiguration()->setString("application.state", DeviceManager::Stopped);
                break;
        }
    }

    UpnpApplication*    _pApp;
};


class RendererConfView : public Gui::View
{
    friend class GuiSetup;
    friend class RendererDoneButton;

    RendererConfView(GuiSetup* pGuiSetup, View* pParent = 0);
    void writeConf();

    GuiSetup*       _pGuiSetup;
    Gui::Switch*    _pRendererEnableSwitch;
    Gui::TextLine*  _pRendererFriendlyNameText;
};


class RendererItemController : public Gui::Controller
{
    friend class GuiSetup;

    RendererItemController(GuiSetup* pGuiSetup) : _pGuiSetup(pGuiSetup) {}

    virtual void selected()
    {
        _pGuiSetup->push(_pGuiSetup->_pRendererConfig);
    }

    GuiSetup*   _pGuiSetup;
};


class RendererDoneButton : Gui::Button
{
    friend class RendererConfView;

    RendererDoneButton(GuiSetup* pGuiSetup, RendererConfView* pRendererConfView, View* pParent = 0) :
    Button(pParent),
    _pGuiSetup(pGuiSetup),
    _pRendererConfView(pRendererConfView)
    {
        setLabel("Done");
    }

    virtual void pushed()
    {
        _pRendererConfView->writeConf();
        _pGuiSetup->_pRendererItemModel->getLabelModel()->setLabel(_pGuiSetup->_pApp->getFileConfiguration()->getString("renderer.friendlyName", ""));
        _pGuiSetup->_pRendererItem->syncView();
        _pGuiSetup->pop();
        _pGuiSetup->_pApp->restartLocalDeviceContainer();
    }

    GuiSetup*           _pGuiSetup;
    RendererConfView*   _pRendererConfView;
};


class RendererCancelButton : Gui::Button
{
    friend class RendererConfView;

    RendererCancelButton(GuiSetup* pGuiSetup, View* pParent = 0) :
    Button(pParent),
    _pGuiSetup(pGuiSetup)
    {
        setLabel("Cancel");
    }

    virtual void pushed()
    {
        _pGuiSetup->pop();
    }

    GuiSetup*   _pGuiSetup;
};


RendererConfView::RendererConfView(GuiSetup* pGuiSetup, View* pParent) :
View(pParent),
_pGuiSetup(pGuiSetup)
{
    setLayout(new Gui::VerticalLayout);

    Gui::View* pRendererEnableView = new Gui::View(this);
    pRendererEnableView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pRendererEnableLabel = new Gui::Label(pRendererEnableView);
    pRendererEnableLabel->setLabel("enable");
    _pRendererEnableSwitch = new Gui::Switch(pRendererEnableView);
    _pRendererEnableSwitch->setState(_pGuiSetup->_pApp->getFileConfiguration()->getBool("renderer.enable", false));

    Gui::View* pRendererFriendlyNameView = new Gui::View(this);
    pRendererFriendlyNameView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pRendererFriendlyNameLabel = new Gui::Label(pRendererFriendlyNameView);
    pRendererFriendlyNameLabel->setLabel("friendly name");
    _pRendererFriendlyNameText = new Gui::TextLine(pRendererFriendlyNameView);
    _pRendererFriendlyNameText->setTextLine(_pGuiSetup->_pApp->getFileConfiguration()->getString("renderer.friendlyName", ""));

//        Gui::View* pRendererUuidView = new Gui::View(this);
//        pRendererUuidView->setLayout(new Gui::HorizontalLayout);
//        Gui::Label* pRendererUuidLabel = new Gui::Label(pRendererUuidView);
//        pRendererUuidLabel->setLabel("uuid");
//        Gui::TextLine* pRendererUuidText = new Gui::TextLine(pRendererUuidView);
//        pRendererUuidText->setTextLine(_pGuiSetup->_pApp->getFileConfiguration()->getString("renderer.uuid", ""));

    View* pDoneCancelView = new View(this);
    pDoneCancelView->setLayout(new Gui::HorizontalLayout);
    new RendererDoneButton(pGuiSetup, this, pDoneCancelView);
    new RendererCancelButton(pGuiSetup, pDoneCancelView);
}


void
RendererConfView::writeConf()
{
    _pGuiSetup->_pApp->getFileConfiguration()->setBool("renderer.enable", _pRendererEnableSwitch->getStateOn());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("renderer.friendlyName", _pRendererFriendlyNameText->getTextLine());
}


class ServerListModel : public Gui::ListModel
{
    friend class GuiSetup;
    friend class ServerDoneButton;

    ServerListModel(GuiSetup* pGuiSetup) :
    _pGuiSetup(pGuiSetup),
    _config(Poco::Util::Application::instance().config())
    {
    }


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
    GuiSetup*                           _pGuiSetup;
};


class ServerEnableSwitch : Gui::Switch
{
    friend class ServerConfView;

    ServerEnableSwitch(UpnpApplication* pApp, View* pParent = 0) :
    Switch(pParent),
    _pApp(pApp)
    {
    }

    virtual void switched(bool on)
    {
    }

    UpnpApplication*    _pApp;
};


class ServerLayoutSelector : Gui::Selector
{
    friend class ServerConfView;

    ServerLayoutSelector(View* pParent = 0) : Selector(pParent)
    {
        syncView();
    }

    virtual int totalItemCount()
    {
        return 3;
    }


    virtual std::string getItemLabel(int index)
    {
        switch(index) {
            case 0:
                return Av::ServerContainer::LAYOUT_FLAT;
            case 1:
                return Av::ServerContainer::LAYOUT_DIR_STRUCT;
            case 2:
                return Av::ServerContainer::LAYOUT_PROPERTY_GROUPS;
        }
    }
};

class ServerConfView : public Gui::View
{
    friend class GuiSetup;
    friend class ServerItemController;
    friend class ServerListController;
    friend class ServerDoneButton;

    ServerConfView(GuiSetup* pGuiSetup, int index, View* pParent = 0);
    virtual void syncViewImpl();
    void writeConf();

    GuiSetup*               _pGuiSetup;
    int                     _index;

    ServerEnableSwitch*     _pServerEnableSwitch;
    Gui::TextLine*          _pServerFriendlyNameText;
    Gui::TextLine*          _pServerPluginText;
    Gui::TextLine*          _pServerBasePathText;
    Gui::TextLine*          _pServerPollText;
    ServerLayoutSelector*   _pServerLayoutSelector;
};


class ServerDoneButton : Gui::Button
{
    friend class ServerConfView;

    ServerDoneButton(GuiSetup* pGuiSetup, ServerConfView* pServerConfView, View* pParent = 0) :
    Button(pParent),
    _pGuiSetup(pGuiSetup),
    _pServerConfView(pServerConfView)
    {
        setLabel("Done");
    }

    virtual void pushed()
    {
        _pServerConfView->writeConf();
        _pGuiSetup->_pServerListModel->readConfig();
        _pGuiSetup->_pServerList->syncView();
        _pGuiSetup->pop();
        _pGuiSetup->_pApp->restartLocalDeviceContainer();
    }

    GuiSetup*       _pGuiSetup;
    ServerConfView* _pServerConfView;
};


class ServerCancelButton : Gui::Button
{
    friend class ServerConfView;

    ServerCancelButton(GuiSetup* pGuiSetup, View* pParent = 0) :
    Button(pParent),
    _pGuiSetup(pGuiSetup)
    {
        setLabel("Cancel");
    }

    virtual void pushed()
    {
        _pGuiSetup->pop();
    }

    GuiSetup*   _pGuiSetup;
};


class ServerConfModel : public Gui::Model
{
    friend class ServerListController;
    friend class ServerConfView;

    ServerConfModel(GuiSetup* pGuiSetup, int index) : _pGuiSetup(pGuiSetup), _index(index) {}

    bool getEnabled()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getBool("server." + Poco::NumberFormatter::format(_index) + ".enable", false);
    }

    std::string getFriendlyName()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + Poco::NumberFormatter::format(_index) + ".friendlyName", "");
    }

    std::string getPlugin()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + Poco::NumberFormatter::format(_index) + ".plugin", "");
    }

    std::string getBasePath()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + Poco::NumberFormatter::format(_index) + ".basePath", "");
    }

    std::string getPollTime()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + Poco::NumberFormatter::format(_index) + ".pollUpdateId", "");
    }

    std::string getLayout()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + Poco::NumberFormatter::format(_index) + ".layout", "");
    }

    int             _index;
    GuiSetup*       _pGuiSetup;
};


ServerConfView::ServerConfView(GuiSetup* pGuiSetup, int index, View* pParent) :
View(pParent),
_pGuiSetup(pGuiSetup),
_index(index)
{
    setLayout(new Gui::VerticalLayout);

    Gui::View* pServerEnableView = new Gui::View(this);
    pServerEnableView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerEnableLabel = new Gui::Label(pServerEnableView);
    pServerEnableLabel->setLabel("enable");
    _pServerEnableSwitch = new ServerEnableSwitch(_pGuiSetup->_pApp, pServerEnableView);

    Gui::View* pServerFriendlyNameView = new Gui::View(this);
    pServerFriendlyNameView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerFriendlyNameLabel = new Gui::Label(pServerFriendlyNameView);
    pServerFriendlyNameLabel->setLabel("friendly name");
    _pServerFriendlyNameText = new Gui::TextLine(pServerFriendlyNameView);

//        Gui::View* pServerUuidView = new Gui::View(this);
//        pServerUuidView->setLayout(new Gui::HorizontalLayout);
//        Gui::Label* pServerUuidLabel = new Gui::Label(pServerUuidView);
//        pServerUuidLabel->setLabel("uuid");
//        Gui::TextLine* pServerUuidText = new Gui::TextLine(pServerUuidView);
//        pServerUuidText->setTextLine(_pGuiSetup->_pApp->getFileConfiguration()->getString("renderer.uuid", ""));

    Gui::View* pServerPluginView = new Gui::View(this);
    pServerPluginView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerPluginLabel = new Gui::Label(pServerPluginView);
    pServerPluginLabel->setLabel("plugin");
    _pServerPluginText = new Gui::TextLine(pServerPluginView);

    Gui::View* pServerBasePathView = new Gui::View(this);
    pServerBasePathView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerBasePathLabel = new Gui::Label(pServerBasePathView);
    pServerBasePathLabel->setLabel("base path");
    _pServerBasePathText = new Gui::TextLine(pServerBasePathView);

    Gui::View* pServerPollView = new Gui::View(this);
    pServerPollView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerPollLabel = new Gui::Label(pServerPollView);
    pServerPollLabel->setLabel("poll time");
    _pServerPollText = new Gui::TextLine(pServerPollView);

    Gui::View* pServerLayoutView = new Gui::View(this);
    pServerLayoutView->setLayout(new Gui::HorizontalLayout);
    Gui::Label* pServerLayoutLabel = new Gui::Label(pServerLayoutView);
    pServerLayoutLabel->setLabel("layout");
    _pServerLayoutSelector = new ServerLayoutSelector(pServerLayoutView);

    View* pDoneCancelView = new View(this);
    pDoneCancelView->setLayout(new Gui::HorizontalLayout);
    new ServerDoneButton(_pGuiSetup, this, pDoneCancelView);
    new ServerCancelButton(_pGuiSetup, pDoneCancelView);
}


void ServerConfView::syncViewImpl()
{
    _pServerEnableSwitch->setState(static_cast<ServerConfModel*>(_pModel)->getEnabled());
    _pServerFriendlyNameText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getFriendlyName());
    _pServerPluginText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getPlugin());
    _pServerBasePathText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getBasePath());
    _pServerPollText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getPollTime());
    std::string layout = static_cast<ServerConfModel*>(_pModel)->getLayout();
    if (layout == Av::ServerContainer::LAYOUT_FLAT) {
        _pServerLayoutSelector->setCurrentIndex(0);
    }
    else if (layout == Av::ServerContainer::LAYOUT_DIR_STRUCT) {
        _pServerLayoutSelector->setCurrentIndex(1);
    }
    else if (layout == Av::ServerContainer::LAYOUT_PROPERTY_GROUPS) {
        _pServerLayoutSelector->setCurrentIndex(2);
    }
}


void ServerConfView::writeConf()
{
    _pGuiSetup->_pApp->getFileConfiguration()->setBool("server." + Poco::NumberFormatter::format(_index) + ".enable", _pServerEnableSwitch->getStateOn());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".friendlyName", _pServerFriendlyNameText->getTextLine());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".plugin", _pServerPluginText->getTextLine());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".basePath", _pServerBasePathText->getTextLine());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".pollUpdateId", _pServerPollText->getTextLine());
    switch(_pServerLayoutSelector->getCurrentIndex()) {
        case 0:
            _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".layout", Av::ServerContainer::LAYOUT_FLAT);
            break;
        case 1:
            _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".layout", Av::ServerContainer::LAYOUT_DIR_STRUCT);
            break;
        case 2:
            _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + Poco::NumberFormatter::format(_index) + ".layout", Av::ServerContainer::LAYOUT_PROPERTY_GROUPS);
            break;
    }
}


//class ServerItemController : public Gui::Controller
//{
//    friend class GuiSetup;
//    friend class ServerListModel;
//
//    ServerItemController(GuiSetup* pGuiSetup) : _pGuiSetup(pGuiSetup) {}
//
//    virtual void selected()
//    {
//        ServerConfView* pServerConfView = new ServerConfView(_pGuiSetup);
//        _pGuiSetup->push(pServerConfView);
//    }
//
//    GuiSetup*   _pGuiSetup;
//};


class ServerListController : public Gui::ListController
{
    friend class GuiSetup;
    friend class ServerListModel;

    ServerListController(GuiSetup* pGuiSetup) : _pGuiSetup(pGuiSetup) {}

    virtual void selectedItem(int row)
    {
        ServerConfView* pServerConfView = new ServerConfView(_pGuiSetup, row);
        pServerConfView->setModel(new ServerConfModel(_pGuiSetup, row));
        _pGuiSetup->push(pServerConfView);
    }

    GuiSetup*   _pGuiSetup;
};


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


GuiSetup::GuiSetup(UpnpApplication* pApp, Gui::View* pParent) :
NavigatorView(pParent),
_pApp(pApp)
{
    _pSetupView = new View;
    _pSetupView->setName("Setup");
    _pSetupView->setLayout(new Gui::VerticalLayout);

    Gui::Label* pControllerLabel = new Gui::Label(_pSetupView);
    pControllerLabel->setLabel("Controller");
    pControllerLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pControllerLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pAppStateSelector = new AppStateSelector(pApp, _pSetupView);
    _pAppStateSelector->setSizeConstraint(10, 20, Gui::View::Pref);

    Gui::Label* pRendererLabel = new Gui::Label(_pSetupView);
    pRendererLabel->setLabel("Local Renderer");
    pRendererLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pRendererLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pRendererItem = new Gui::ListItemView(_pSetupView);
    _pRendererItem->setSizeConstraint(10, 20, Gui::View::Pref);
    _pRendererItemModel = new Gui::ListItemModel;
    Gui::LabelModel* pRendererItemLabel = new Gui::LabelModel;
    pRendererItemLabel->setLabel(_pApp->getFileConfiguration()->getString("renderer.friendlyName", "Local Renderer"));
    _pRendererItemModel->setLabelModel(pRendererItemLabel);
    _pRendererItem->setModel(_pRendererItemModel);
    _pRendererItem->attachController(new RendererItemController(this));

    _pRendererConfig = new RendererConfView(this);

    Gui::Label* pServerLabel = new Gui::Label(_pSetupView);
    pServerLabel->setLabel("Server");
    pServerLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pServerLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pServerListModel = new ServerListModel(this);
    _pServerListModel->readConfig();
    _pServerList = new Gui::ListView(_pSetupView);
    _pServerList->setModel(_pServerListModel);
    _pServerList->attachController(new ServerListController(this));

    showNavigatorBar(false);
    push(_pSetupView, "Setup");
}


GuiSetup::~GuiSetup()
{
}


//void
//GuiSetup::writeConf()
//{
//
//}


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
        LOGNS(Av, upnpav, debug, "omm config read in devices from config ...");
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
        _pApp->restartLocalDeviceContainer();

        LOGNS(Av, upnpav, debug, "omm local devices config update done.");
    }
}

} // namespace Omm
