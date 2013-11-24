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

#include "Gui/GuiLogger.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/ListItem.h"
#include "Gui/Switch.h"
#include "Gui/Button.h"
#include "Gui/TextLine.h"

#include "UpnpGui/UpnpApplication.h"
#include "UpnpGui/ControllerWidget.h"
#include "UpnpGui/Setup.h"

// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
#include "Omm/Dvb/Device.h"
#include "Omm/Dvb/Frontend.h"
#endif


namespace Omm {

class ServerPluginSelector;


class AppStateSelector : public Gui::Selector
{
    friend class GuiSetup;

    AppStateSelector(UpnpApplication* pApp, View* pParent) :
    Selector(pParent),
    _pApp(pApp)
    {
        syncView();
        std::string state = _pApp->getFileConfiguration()->getString("application.devices", "Public");
        if (state == DeviceManager::Public) {
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
                return DeviceManager::Public;
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
                _pApp->setLocalDeviceContainerState(DeviceManager::Public);
                _pApp->getFileConfiguration()->setString("application.devices", DeviceManager::Public);
                break;
            case 1:
                _pApp->setLocalDeviceContainerState(DeviceManager::Local);
                _pApp->getFileConfiguration()->setString("application.devices", DeviceManager::Local);
                break;
            case 2:
                _pApp->setLocalDeviceContainerState(DeviceManager::Stopped);
                _pApp->getFileConfiguration()->setString("application.devices", DeviceManager::Stopped);
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
    pRendererEnableView->setStretchFactor(-1.0);
    pRendererEnableView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pRendererEnableLabel = new Gui::Label(pRendererEnableView);
    pRendererEnableLabel->setLabel("Enable");
//    pRendererEnableLabel->setStretchFactor(-1.0);
    pRendererEnableLabel->setSizeConstraint(20, 20, Gui::View::Pref);
    _pRendererEnableSwitch = new Gui::Switch(pRendererEnableView);
//    _pRendererEnableSwitch->setStretchFactor(-1.0);
    _pRendererEnableSwitch->setSizeConstraint(20, 20, Gui::View::Pref);
    _pRendererEnableSwitch->setState(_pGuiSetup->_pApp->getFileConfiguration()->getBool("renderer.enable", false));

    Gui::View* pRendererFriendlyNameView = new Gui::View(this);
    pRendererFriendlyNameView->setLayout(new Gui::HorizontalLayout);
    pRendererFriendlyNameView->setStretchFactor(-1.0);
    pRendererFriendlyNameView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pRendererFriendlyNameLabel = new Gui::Label(pRendererFriendlyNameView);
    pRendererFriendlyNameLabel->setLabel("Name");
//    pRendererFriendlyNameLabel->setStretchFactor(-1.0);
    pRendererFriendlyNameLabel->setSizeConstraint(20, 20, Gui::View::Pref);
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
    pDoneCancelView->setStretchFactor(-1.0);
    pDoneCancelView->setSizeConstraint(10, 20, Gui::View::Pref);
    new RendererDoneButton(pGuiSetup, this, pDoneCancelView);
    new RendererCancelButton(pGuiSetup, pDoneCancelView);
}


void
RendererConfView::writeConf()
{
    _pGuiSetup->_pApp->getFileConfiguration()->setBool("renderer.enable", _pRendererEnableSwitch->getStateOn());
    _pGuiSetup->_pApp->getFileConfiguration()->setString("renderer.friendlyName", _pRendererFriendlyNameText->getTextLine());
}


class ServerListItemModel : public Gui::ListItemModel
{
    friend class ServerListModel;
    friend class ServerDeleteButton;

    ServerListItemModel(const std::string& id) : _id(id) {}

    std::string _id;
};


class ServerListModel : public Gui::ListModel
{
    friend class GuiSetup;
    friend class ServerListController;
    friend class ServerDoneButton;
    friend class ServerNewButton;
    friend class ServerDeleteButton;

    ServerListModel(GuiSetup* pGuiSetup) :
    _pGuiSetup(pGuiSetup)
    {
    }


    void clearConfigItems();
    void readConfig();
    void writeConfig();
    void appendConfigItem(const std::string& id, const std::string& label);
    void removeConfigItem(const std::string& id);
    std::string getId(int row);

    virtual int totalItemCount();
    virtual Gui::Model* getItemModel(int row);
    virtual Gui::View* createItemView();

    std::map<std::string, ServerListItemModel*> _configServerItems;
    std::vector<std::string>                    _configServerIds;
    GuiSetup*                                   _pGuiSetup;
};


class ServerDeleteButton : public Gui::Button
{
    friend class ServerListModel;

    ServerDeleteButton(Gui::ListView* pServerListView, View* pParent = 0) : Button(pParent), _pServerListView(pServerListView)
    {
        setLabel("X");
    }

    virtual void pushed()
    {
        ServerListItemModel* pItemModel = dynamic_cast<ServerListItemModel*>(getParent()->getModel());
        ServerListModel* pServerListModel = dynamic_cast<ServerListModel*>(_pServerListView->getModel());
        if (pItemModel && pServerListModel) {
            std::string serverId = pItemModel->_id;
            LOGNS(Gui, gui, debug, "server config delete id: " + serverId);
            pServerListModel->removeConfigItem(serverId);
            pServerListModel->writeConfig();
            _pServerListView->syncView();
        }
    }

    Gui::ListView* _pServerListView;
};


class ServerEnableSwitch : Gui::Switch
{
    friend class ServerConfView;
    friend class ServerConfModel;

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
    friend class ServerConfModel;

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
    friend class ServerConfModel;
    friend class ServerItemController;
    friend class ServerListController;
    friend class ServerDoneButton;
    friend class ServerNewButton;
    friend class ServerPluginSelector;

    ServerConfView(GuiSetup* pGuiSetup, bool newServer, View* pParent = 0);
    virtual void syncViewImpl();
//    void writeConf();

    GuiSetup*               _pGuiSetup;

    Gui::Label*             _pServerPluginText;
    ServerPluginSelector*   _pServerPluginSelector;
    bool                    _newServer;
    ServerEnableSwitch*     _pServerEnableSwitch;
    Gui::TextLine*          _pServerFriendlyNameText;
    Gui::TextLine*          _pServerBasePathText;
    Gui::Label*             _pServerBasePathLabel;
//    Gui::TextLine*          _pServerPollText;
    ServerLayoutSelector*   _pServerLayoutSelector;
    Gui::Button*            _pServerScanButton;
    Gui::Label*             _pServerScanProgressLabel;
    Gui::View*              _pServerBasePathView;
};


class ServerPluginSelector : Gui::Selector
{
    friend class ServerConfView;
    friend class ServerConfModel;

    ServerPluginSelector(ServerConfView* pConfView, View* pParent = 0) : Selector(pParent), _pConfView(pConfView)
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
                return "Fileserver";
            case 1:
                return "Webradio";
            case 2:
                return "Digital TV";
        }
    }


    std::string getTextLine()
    {
        switch(getCurrentIndex()) {
            case 0:
                return "model-file";
            case 1:
                return "model-webradio";
            case 2:
                return "model-dvb";
        }
    }


    void setTextLine(const std::string& model)
    {
        if (model == "model-file") {
            setCurrentIndex(0);
        }
        else if (model == "model-webradio") {
            setCurrentIndex(1);
        }
        else if (model == "model-dvb") {
            setCurrentIndex(2);
        }
    }


    virtual void selected(int index)
    {
        _pConfView->syncViewImpl();
    }

    ServerConfView* _pConfView;
};


class ServerConfModel;

class ServerScanNotification : public Av::DataModelScanNotification
{
    friend class ServerConfModel;

    ServerScanNotification(ServerConfModel* pServerConfModel) : _pServerConfModel(pServerConfModel) {}

    virtual void itemScanned(const std::string& path);

    ServerConfModel*    _pServerConfModel;
};


// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
class DvbFrontendKeySelectorModel : public Gui::SelectorModel
{
//    friend class ServerConfModel;
//    friend class ServerConfView;
public:
    DvbFrontendKeySelectorModel() {}

    DvbFrontendKeySelectorModel(Omm::Dvb::Frontend* pFrontend)
    {
        pFrontend->getInitialTransponderKeys(_frontendKeys);
    }

    virtual int totalItemCount()
    {
        return _frontendKeys.size();
    }

    virtual std::string getItemLabel(int index)
    {
        return _frontendKeys[index];
    }

    std::vector<std::string>    _frontendKeys;
};
#endif


class ServerConfModel : public Gui::Model
{
    friend class ServerListController;
    friend class ServerDoneButton;
    friend class ServerNewButton;
    friend class ServerScanButton;
    friend class ServerConfView;
    friend class ServerScanNotification;
    friend class DvbFrontendKeySelectorController;

    ServerConfModel(GuiSetup* pGuiSetup, ServerConfView* pConfView, const std::string& id) : _pGuiSetup(pGuiSetup), _pConfView(pConfView), _id(id), _serverScanitemCount(0)
    // TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
    , _pDvbDevice(0)
#endif
    {
        _uuid = _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".uuid", Poco::UUIDGenerator().createRandom().toString());
        _pServerScanNotification = new ServerScanNotification(this);
// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
        if (getPlugin() == "model-dvb") {
            _pDvbDevice = Omm::Dvb::Device::instance();
            for (Omm::Dvb::Device::AdapterIterator it = _pDvbDevice->adapterBegin(); it != _pDvbDevice->adapterEnd(); ++it) {
                for (Omm::Dvb::Adapter::FrontendIterator fit = it->second->frontendBegin(); fit != it->second->frontendEnd(); ++fit) {
                    LOGNS(Gui, gui, debug, "server conf model-dvb, frontend type: " + (*fit)->getType());
//                    (*fit)->getInitialTransponderKeys(_dvbFrontendKeys[(*fit)->getType()]);
//                    _dvbFrontendKeys[(*fit)->getType()] = DvbFrontendKeySelectorModel(*fit);
                    _dvbFrontendKeys.insert(std::make_pair((*fit)->getType(), DvbFrontendKeySelectorModel(*fit)));
                }
            }
        }
#endif
    }

    bool getEnabled()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getBool("server." + _id + ".enable", false);
    }

    std::string getFriendlyName()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".friendlyName", "");
    }

    std::string getPlugin()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".plugin", "");
    }

    std::string getPluginText()
    {
        std::string plugin = getPlugin();
        if(plugin == "model-file") {
            return "Fileserver";
        }
        else if (plugin == "model-webradio") {
            return "Webradio";
        }
        else if (plugin == "model-dvb") {
            return "Digital TV";
        }
    }

    std::string getBasePath()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".basePath", "");
    }

    std::string getPollTime()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".pollUpdateId", "");
    }

    std::string getLayout()
    {
        return _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + _id + ".layout", "");
    }

// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
    void setDvbFrontendKey(const std::string& frontendType, int index)
    {
        std::string key = _dvbFrontendKeys[frontendType].getItemLabel(index);
        _dvbSelectedKeys[frontendType] = key;
        Omm::Dvb::Device::instance()->addInitialTransponders(frontendType, key);
    }
#endif

    void writeConf()
    {
        _pGuiSetup->_pApp->getFileConfiguration()->setBool("server." + _id + ".enable", _pConfView->_pServerEnableSwitch->getStateOn());
        _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".friendlyName", _pConfView->_pServerFriendlyNameText->getTextLine());
        _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".uuid", _uuid);
        _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".plugin", _pConfView->_pServerPluginSelector->getTextLine());
        _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".basePath", _pConfView->_pServerBasePathText->getTextLine());
//        _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".pollUpdateId", _pConfView->_pServerPollText->getTextLine());
        switch(_pConfView->_pServerLayoutSelector->getCurrentIndex()) {
            case 0:
                _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".layout", Av::ServerContainer::LAYOUT_FLAT);
                break;
            case 1:
                _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".layout", Av::ServerContainer::LAYOUT_DIR_STRUCT);
                break;
            case 2:
                _pGuiSetup->_pApp->getFileConfiguration()->setString("server." + _id + ".layout", Av::ServerContainer::LAYOUT_PROPERTY_GROUPS);
                break;
        }
    }

    std::string                                         _id;
    std::string                                         _uuid;
    GuiSetup*                                           _pGuiSetup;
    ServerConfView*                                     _pConfView;
    ServerScanNotification*                             _pServerScanNotification;
    ui4                                                 _serverScanitemCount;
// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
    Omm::Dvb::Device*                                   _pDvbDevice;
    std::map<std::string, DvbFrontendKeySelectorModel>  _dvbFrontendKeys;
    std::map<std::string, std::string>                  _dvbSelectedKeys;
#endif
};


// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
class DvbFrontendKeySelectorController : public Gui::SelectorController
{
    friend class ServerConfView;

    DvbFrontendKeySelectorController(const std::string& frontendType, ServerConfModel* pConfModel) : _frontendType(frontendType), _pConfModel(pConfModel) {}

    virtual void selected(int index)
    {
        _pConfModel->setDvbFrontendKey(_frontendType, index);
    }

    std::string         _frontendType;
    ServerConfModel*    _pConfModel;
};
#endif

void
ServerScanNotification::itemScanned(const std::string& path)
{
    _pServerConfModel->_serverScanitemCount++;
    _pServerConfModel->syncViews();
}


class ServerScanButton : Gui::Button
{
    friend class ServerConfView;

    ServerScanButton(UpnpApplication* pApp, ServerConfView* pServerConfView, View* pParent = 0) :
    Button(pParent),
    _pApp(pApp),
    _pServerConfView(pServerConfView)
    {
        setLabel("Start");
    }

    virtual void pushed()
    {
        ServerConfModel* pServerConfModel = static_cast<ServerConfModel*>(_pServerConfView->getModel());
        Av::MediaServer* pServer = _pApp->getLocalMediaServer(pServerConfModel->_uuid);
        if (pServer) {
            Av::AbstractDataModel* pDataModel = pServer->getRoot()->getDataModel();
            if (pDataModel) {
                pDataModel->checkSystemUpdateId(true);
            }
        }
    }

    UpnpApplication* _pApp;
    ServerConfView*  _pServerConfView;
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
        ServerConfModel* pServerConfModel = static_cast<ServerConfModel*>(_pServerConfView->getModel());
        pServerConfModel->writeConf();

        ServerListModel* pServerListModel = _pGuiSetup->_pServerListModel;
        pServerListModel->readConfig();
        // add id of server, if this server is newly created
        std::vector<std::string>& serverIds = pServerListModel->_configServerIds;
        std::string thisServerId = pServerConfModel->_id;
        LOGNS(Gui, gui, debug, "server config done, id: " + thisServerId);
        if (std::find(serverIds.begin(), serverIds.end(), thisServerId) == serverIds.end()) {
            LOGNS(Gui, gui, debug, "server config new id: " + thisServerId);
            pServerListModel->appendConfigItem(thisServerId, pServerConfModel->getFriendlyName());
        }
        pServerListModel->writeConfig();
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


class ServerNewButton : public Gui::Button
{
    friend class GuiSetup;

    ServerNewButton(GuiSetup* pGuiSetup) : _pGuiSetup(pGuiSetup)
    {
        setLabel("New Media");
    }

    virtual void pushed()
    {
        ServerConfView* pServerConfView = new ServerConfView(_pGuiSetup, true);
        // find new or deleted config file id for local server.
        int id = 0;
        std::string newId = "0";
        std::vector<std::string>& serverIds = _pGuiSetup->_pServerListModel->_configServerIds;
        while (std::find(serverIds.begin(), serverIds.end(), newId) != serverIds.end()) {
            id++;
            newId = Poco::NumberFormatter::format(id);
        }
        ServerConfModel* pServerConfModel = new ServerConfModel(_pGuiSetup, pServerConfView, newId);
        pServerConfView->setModel(pServerConfModel);

        _pGuiSetup->push(pServerConfView);
    }

    GuiSetup*   _pGuiSetup;
};


ServerConfView::ServerConfView(GuiSetup* pGuiSetup, bool newServer, View* pParent) :
View(pParent),
_pGuiSetup(pGuiSetup),
_pServerPluginSelector(0),
_pServerPluginText(0),
_newServer(newServer)
{
    setLayout(new Gui::VerticalLayout);

    Gui::View* pServerPluginView = new Gui::View(this);
    pServerPluginView->setLayout(new Gui::HorizontalLayout);
    pServerPluginView->setStretchFactor(-1.0);
    pServerPluginView->setSizeConstraint(10, 20, Gui::View::Pref);
    if (newServer) {
        Gui::Label* pServerPluginLabel = new Gui::Label(pServerPluginView);
        pServerPluginLabel->setLabel("Type");
        pServerPluginLabel->setStretchFactor(-1.0);
        _pServerPluginSelector = new ServerPluginSelector(this, pServerPluginView);
    }
    else {
        _pServerPluginText = new Gui::Label(pServerPluginView);
    }

    Gui::View* pServerFriendlyNameView = new Gui::View(this);
    pServerFriendlyNameView->setLayout(new Gui::HorizontalLayout);
    pServerFriendlyNameView->setStretchFactor(-1.0);
    pServerFriendlyNameView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pServerFriendlyNameLabel = new Gui::Label(pServerFriendlyNameView);
    pServerFriendlyNameLabel->setLabel("Name");
    pServerFriendlyNameLabel->setStretchFactor(-1.0);
    _pServerFriendlyNameText = new Gui::TextLine(pServerFriendlyNameView);

//        Gui::View* pServerUuidView = new Gui::View(this);
//        pServerUuidView->setLayout(new Gui::HorizontalLayout);
//        Gui::Label* pServerUuidLabel = new Gui::Label(pServerUuidView);
//        pServerUuidLabel->setLabel("uuid");
//        Gui::TextLine* pServerUuidText = new Gui::TextLine(pServerUuidView);
//        pServerUuidText->setTextLine(_pGuiSetup->_pApp->getFileConfiguration()->getString("renderer.uuid", ""));

    Gui::View* pServerLayoutView = new Gui::View(this);
    pServerLayoutView->setLayout(new Gui::HorizontalLayout);
    pServerLayoutView->setStretchFactor(-1.0);
    pServerLayoutView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pServerLayoutLabel = new Gui::Label(pServerLayoutView);
    pServerLayoutLabel->setLabel("Layout");
    pServerLayoutLabel->setStretchFactor(-1.0);
    _pServerLayoutSelector = new ServerLayoutSelector(pServerLayoutView);

    Gui::View* pServerEnableView = new Gui::View(this);
    pServerEnableView->setLayout(new Gui::HorizontalLayout);
    pServerEnableView->setStretchFactor(-1.0);
    pServerEnableView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pServerEnableLabel = new Gui::Label(pServerEnableView);
    pServerEnableLabel->setLabel("Enable");
    pServerEnableLabel->setStretchFactor(-1.0);
    _pServerEnableSwitch = new ServerEnableSwitch(_pGuiSetup->_pApp, pServerEnableView);

    Gui::View* pServerScanView = new Gui::View(this);
    pServerScanView->setLayout(new Gui::HorizontalLayout);
    pServerScanView->setStretchFactor(-1.0);
    pServerScanView->setSizeConstraint(10, 20, Gui::View::Pref);
    Gui::Label* pServerScanLabel = new Gui::Label(pServerScanView);
    pServerScanLabel->setLabel("Scan");
    pServerScanLabel->setStretchFactor(-1.0);
    _pServerScanButton = new ServerScanButton(_pGuiSetup->_pApp, this, pServerScanView);
    _pServerScanProgressLabel = new Gui::Label(pServerScanView);
    _pServerScanProgressLabel->setLabel("0");
    _pServerScanProgressLabel->setStretchFactor(-1.0);

    _pServerBasePathView = new Gui::View(this);
    _pServerBasePathView->setLayout(new Gui::HorizontalLayout);
    _pServerBasePathView->setStretchFactor(-1.0);
    _pServerBasePathView->setSizeConstraint(10, 20, Gui::View::Pref);

//    _pServerBasePathLabel = new Gui::Label(_pServerBasePathView);
//    _pServerBasePathLabel->setLabel("Path");
//    _pServerBasePathLabel->setStretchFactor(-1.0);
//    _pServerBasePathText = new Gui::TextLine(_pServerBasePathView);

//    Gui::View* pServerPollView = new Gui::View(this);
//    pServerPollView->setLayout(new Gui::HorizontalLayout);
////    pServerPollView->setStretchFactor(-1.0);
//    Gui::Label* pServerPollLabel = new Gui::Label(pServerPollView);
//    pServerPollLabel->setLabel("poll time");
//    pServerPollLabel->setStretchFactor(-1.0);
//    _pServerPollText = new Gui::TextLine(pServerPollView);

    View* pDoneCancelView = new View(this);
    pDoneCancelView->setLayout(new Gui::HorizontalLayout);
    pDoneCancelView->setStretchFactor(-1.0);
    pDoneCancelView->setSizeConstraint(10, 20, Gui::View::Pref);
    new ServerDoneButton(_pGuiSetup, this, pDoneCancelView);
    new ServerCancelButton(_pGuiSetup, pDoneCancelView);
}


void ServerConfView::syncViewImpl()
{
    if (static_cast<ServerConfModel*>(_pModel)->getPlugin() == "model-dvb") {
// TODO: find a better solution to handle dvb includes e.g. on smartphones (where not needed)
#ifndef __IPHONE__
        for (std::map<std::string, DvbFrontendKeySelectorModel>::iterator fit = static_cast<ServerConfModel*>(_pModel)->_dvbFrontendKeys.begin(); fit != static_cast<ServerConfModel*>(_pModel)->_dvbFrontendKeys.end(); ++fit) {
            Gui::Label* pServerDvbFrontendNameLabel = new Gui::Label(_pServerBasePathView);
            pServerDvbFrontendNameLabel->setLabel(fit->first);
            pServerDvbFrontendNameLabel->setStretchFactor(-1.0);
            Gui::SelectorView* pServerDvbFrontendKeySelector = new Gui::Selector(_pServerBasePathView);
            pServerDvbFrontendKeySelector->setModel(&fit->second);
            pServerDvbFrontendKeySelector->attachController(new DvbFrontendKeySelectorController(fit->first, static_cast<ServerConfModel*>(_pModel)));
    //        for (int keyIndex = 0; keyIndex < fit->second.totalItemCount(); keyIndex++) {
    //            LOGNS(Gui, gui, debug, "dvb frontend key: " + fit->first + "/" + fit->second.getItemLabel(keyIndex));
    //        }
        };
#endif
    }
    else {
        _pServerBasePathLabel = new Gui::Label(_pServerBasePathView);
        _pServerBasePathLabel->setLabel("Path");
        _pServerBasePathLabel->setStretchFactor(-1.0);
        _pServerBasePathText = new Gui::TextLine(_pServerBasePathView);
        if (_newServer && _pServerPluginSelector->getCurrentIndex() || !_newServer && static_cast<ServerConfModel*>(_pModel)->getPlugin() != "model-file") {
            _pServerBasePathText->hide();
            _pServerBasePathLabel->hide();
        }
        else {
            _pServerBasePathText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getBasePath());
            _pServerBasePathText->show();
            _pServerBasePathLabel->show();
        }
    }

    if (_newServer) {
        _pServerPluginSelector->setTextLine(static_cast<ServerConfModel*>(_pModel)->getPlugin());
    }
    else {
        _pServerPluginText->setLabel(static_cast<ServerConfModel*>(_pModel)->getPluginText());
    }
    _pServerEnableSwitch->setState(static_cast<ServerConfModel*>(_pModel)->getEnabled());
    _pServerFriendlyNameText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getFriendlyName());

//    _pServerPollText->setTextLine(static_cast<ServerConfModel*>(_pModel)->getPollTime());
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
    LOGNS(Gui, gui, debug, "scan notification server uuid: " + static_cast<ServerConfModel*>(_pModel)->_uuid);
    try {
        Av::MediaServer* pMediaServer = _pGuiSetup->_pApp->getLocalMediaServer(static_cast<ServerConfModel*>(_pModel)->_uuid);
        if (pMediaServer) {
            pMediaServer->getRoot()->getDataModel()->setScanNotification(static_cast<ServerConfModel*>(_pModel)->_pServerScanNotification);
            _pServerScanProgressLabel->setLabel(Poco::NumberFormatter::format(static_cast<ServerConfModel*>(_pModel)->_serverScanitemCount));
        }
    }
    catch (Poco::Exception& e) {
        LOGNS(Gui, gui, debug, "scan notification server not found: " + e.displayText());
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
        ServerConfView* pServerConfView = new ServerConfView(_pGuiSetup, false);
        pServerConfView->setModel(new ServerConfModel(_pGuiSetup, pServerConfView, _pGuiSetup->_pServerListModel->getId(row)));
        _pGuiSetup->push(pServerConfView);
    }

    GuiSetup*   _pGuiSetup;
};


void
ServerListModel::clearConfigItems()
{
    for (std::map<std::string, ServerListItemModel*>::iterator it = _configServerItems.begin(); it != _configServerItems.end(); ++it) {
        delete it->second;
    }
    _configServerItems.clear();
    _configServerIds.clear();
}


void
ServerListModel::readConfig()
{
    clearConfigItems();

    std::string serversString = _pGuiSetup->_pApp->getFileConfiguration()->getString("servers", "");
    Poco::StringTokenizer servers(serversString, ",");
    for (Poco::StringTokenizer::Iterator it = servers.begin(); it != servers.end(); ++it) {
        appendConfigItem(*it, _pGuiSetup->_pApp->getFileConfiguration()->getString("server." + *it + ".friendlyName", ""));
    }
}


void
ServerListModel::writeConfig()
{
    std::string servers = "";
    for (std::vector<std::string>::iterator it = _configServerIds.begin(); it != _configServerIds.end(); ++it) {
        servers += *it;
        if (it != --_configServerIds.end()) {
            servers.append(",");
        }
    }
    LOGNS(Gui, gui, debug, "server config ids: " + servers);
    _pGuiSetup->_pApp->getFileConfiguration()->setString("servers", servers);
}


void
ServerListModel::appendConfigItem(const std::string& id, const std::string& label)
{
    ServerListItemModel* pConfigItemModel = new ServerListItemModel(id);
    pConfigItemModel->setLabelModel(new Gui::LabelModel(label));
    _configServerItems[id] = pConfigItemModel;
    _configServerIds.push_back(id);
}


void
ServerListModel::removeConfigItem(const std::string& id)
{
    std::vector<std::string>::iterator it = std::find(_configServerIds.begin(), _configServerIds.end(), id);
    if (it != _configServerIds.end()) {
        _configServerIds.erase(it);
        delete _configServerItems[id];
        _configServerItems.erase(id);
        LOGNS(Gui, gui, debug, "server config removed id: " + id);
    }
}


std::string
ServerListModel::getId(int row)
{
    return *(_configServerIds.begin() + row);
}


int
ServerListModel::totalItemCount()
{
    return _configServerItems.size();
}


Gui::Model*
ServerListModel::getItemModel(int row)
{
    return _configServerItems[_configServerIds[row]];
}


Gui::View*
ServerListModel::createItemView()
{
    Gui::ListItemView* pListItemView = new Gui::ListItemView;
    ServerDeleteButton* pDeleteButton = new ServerDeleteButton(_pGuiSetup->_pServerList, pListItemView);
    return pListItemView;
}


GuiSetup::GuiSetup(UpnpApplication* pApp, Gui::View* pParent) :
NavigatorView(pParent),
_pApp(pApp)
{
    _pSetupView = new View;
    _pSetupView->setName("Setup");
    _pSetupView->setLayout(new Gui::VerticalLayout);

    Gui::View* pVisibilityView = new Gui::View(_pSetupView);
    pVisibilityView->setLayout(new Gui::HorizontalLayout);
    pVisibilityView->setSizeConstraint(10, 15, Gui::View::Pref);
    Gui::Label* pControllerLabel = new Gui::Label(pVisibilityView);
    pControllerLabel->setLabel("Device Visibility");
    pControllerLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pControllerLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pAppStateSelector = new AppStateSelector(pApp, pVisibilityView);
    _pAppStateSelector->setSizeConstraint(20, 15, Gui::View::Pref);

    Gui::View* pRendererView = new Gui::View(_pSetupView);
    pRendererView->setLayout(new Gui::HorizontalLayout);
    pRendererView->setSizeConstraint(10, 15, Gui::View::Pref);
    Gui::Label* pRendererLabel = new Gui::Label(pRendererView);
    pRendererLabel->setLabel("Media Player");
    pRendererLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pRendererLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pRendererItem = new Gui::ListItemView(pRendererView);
    _pRendererItem->setSizeConstraint(20, 15, Gui::View::Pref);
    _pRendererItemModel = new Gui::ListItemModel;
    Gui::LabelModel* pRendererItemLabel = new Gui::LabelModel;
    pRendererItemLabel->setLabel(_pApp->getFileConfiguration()->getString("renderer.friendlyName", "Local Renderer"));
    _pRendererItemModel->setLabelModel(pRendererItemLabel);
    _pRendererItem->setModel(_pRendererItemModel);
    _pRendererItem->attachController(new RendererItemController(this));

    _pRendererConfig = new RendererConfView(this);

    Gui::Label* pServerLabel = new Gui::Label(_pSetupView);
    pServerLabel->setLabel("Local Media");
    pServerLabel->setBackgroundColor(Gui::Color(220, 220, 220));
    pServerLabel->setSizeConstraint(10, 15, Gui::View::Pref);

    _pServerListModel = new ServerListModel(this);
    _pServerListModel->readConfig();
    _pServerList = new Gui::ListView(_pSetupView);
    _pServerList->addTopView(new ServerNewButton(this));
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

    std::string appState = _pConf->getString("application.devices", "Public");
    *pOutStream << std::string("state<select name=\"application.devices\" size=\"1\">") +
                "<option " + (appState == DeviceManager::Public ? "selected" : "") + ">" + DeviceManager::Public + "</option>" +
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
            if (it->first == "application.devices") {
                _pApp->setLocalDeviceContainerState(it->second);
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
