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

#include "Poco/Util/Application.h"

#include "Util.h"
#include "Gui/GuiLogger.h"
#include "UpnpAv.h"
#include "UpnpAvCtlServer.h"

#include "UpnpGui/DeviceGroup.h"
#include "UpnpGui/MediaServer.h"
#include "UpnpGui/MediaObject.h"

#include "MediaImages.h"


namespace Omm {

class EditPlaylistButton : public Gui::Button
{
    friend class MediaServerGroupWidget;

    EditPlaylistButton(MediaServerGroupWidget* pServerGroup, View* pParent = 0) :
    Gui::Button(pParent),
    _pServerGroup(pServerGroup),
    _editorVisible(false)
    {
        setLabel("edit");
    }

    virtual void pushed()
    {
        if (!_editorVisible) {
            _pServerGroup->_pController->showPlaylistEditor();
            _pServerGroup->pop();
            setLabel("done");
        }
        else {
            _pServerGroup->_pController->showPlaylistEditor(false);
            _pServerGroup->showRightButton(false);
            setLabel("edit");
        }
        _editorVisible = !_editorVisible;
    }

    MediaServerGroupWidget* _pServerGroup;
    bool                    _editorVisible;
};


MediaServerGroupWidget::MediaServerGroupWidget(ControllerWidget* pController) :
DeviceGroupWidget(new Av::MediaServerGroupDelegate),
_pController(pController),
_editorVisible(false)
{
//    LOGNS(Gui, gui, debug, "media server group widget ctor");
    View::setName("Media");
    _deviceGroupListView.setName("Media");
    if (Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        _deviceGroupListView.setItemViewHeight(80);
    }
//    else {
//        _deviceGroupListView.setItemViewHeight(30);
//    }

    push(&_deviceGroupListView, "Media");

    _deviceGroupListView.attachController(this);
    _deviceGroupListView.setModel(this);
    attachController(new DeviceGroupNavigatorController(this));

    // TODO: get search capabilities
    _searchString = Poco::Util::Application::instance().config().getString("controller.searchString", "dc:title contains %s or upnp:artist contains %s");

//    _pEditPlaylistButton = new EditPlaylistButton(this);
//    setStickyView(_pEditPlaylistButton);
//    showRightButton(false);
    setRightButtonLabel("edit");
}


Device*
MediaServerGroupWidget::createDevice()
{
    LOGNS(Gui, gui, debug, "media server group widget create server device.");
    return new MediaServerDevice(this);
}


Gui::View*
MediaServerGroupWidget::createItemView()
{
    LOGNS(Gui, gui, debug, "media server group widget create server view.");
    return new MediaServerView;
}


Gui::Model*
MediaServerGroupWidget::getItemModel(int row)
{
    return static_cast<MediaServerDevice*>(getDevice(row));
}


void
#ifdef __IPHONE__
MediaServerGroupWidget::selectedItem(int row)
#else
MediaServerGroupWidget::activatedItem(int row)
#endif
{
    LOGNS(Gui, gui, debug, "media server group widget selected device");
    MediaServerDevice* pServer = static_cast<MediaServerDevice*>(getDevice(row));
    DeviceGroup::selectDevice(pServer);
    pServer->setSort("+" + Av::AvProperty::ARTIST + ",+" + Av::AvProperty::ALBUM + ",+" + Av::AvProperty::ORIGINAL_TRACK_NUMBER + ",+" + Av::AvProperty::TITLE);
    pServer->browseRootObject();
    MediaObjectModel* pRootObject = static_cast<MediaObjectModel*>(pServer->getRootObject());
    if (pRootObject->isContainer()) {
        LOGNS(Gui, gui, debug, "media server group widget selected device has container as root object");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        pContainer->setName(pServer->getFriendlyName() + " root container");
        if (!pRootObject->isRestricted() && !Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
            MediaContainerPlaylistCreator* pTopView = new MediaContainerPlaylistCreator(pContainer);
            pTopView->setTextLine("new playlist");
            pTopView->setName("top view");
            pTopView->setAlignment(Gui::View::AlignCenter);
            pContainer->addTopView(pTopView);
        }
        pContainer->_pObjectModel = pRootObject;
        pContainer->_pServerGroup = this;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        push(pContainer, pServer->getFriendlyName());
        if (pRootObject->isSearchable()) {
            showSearchBox();
        }
    }
}


void
MediaServerGroupWidget::changedSearchText(const std::string& searchText)
{
    LOGNS(Gui, gui, debug, "media server group widget changed search text: " + searchText);

    if (searchText == "") {
        return;
    }

    MediaServerDevice* pServer = static_cast<MediaServerDevice*>(getSelectedDevice());
    if (pServer) {
        // get container, that is on top of navigator
        MediaContainerWidget* pContainer = dynamic_cast<MediaContainerWidget*>(getVisibleView());
        if (!pContainer) {
            LOGNS(Gui, gui, error, "visible view is no container");
            return;
        }
        Av::CtlMediaObject* pObject = pContainer->_pObjectModel;

        std::string searchExp = Poco::replace(_searchString, std::string("%s"), "\"" + searchText + "\"");
        LOGNS(Gui, gui, debug, "search expression: " + searchExp);
        if (!pObject) {
            LOGNS(Gui, gui, error, "search failed, null object");
            return;
        }
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
MediaServerGroupWidget::rightButtonPushed()
{
    LOGNS(Gui, gui, debug, "media server group widget playlist edit button pushed");

    if (!_editorVisible) {
        _pController->showPlaylistEditor();
        pop();
        setRightButtonLabel("done");
    }
    else {
        _pController->showPlaylistEditor(false);
        showRightButton(false);
        setRightButtonLabel("edit");
    }
    _editorVisible = !_editorVisible;
}


void
MediaServerGroupWidget::finishEditPlaylist()
{
    showRightButton(false);
    setRightButtonLabel("edit");
    _editorVisible = false;

//    _pEditPlaylistButton->hide();
//    _pEditPlaylistButton->setLabel("edit");
//    _pEditPlaylistButton->_editorVisible = false;
}


void
MediaServerDevice::initController()
{
    CtlMediaServer::initController();

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


Av::CtlMediaObject*
MediaServerDevice::createMediaObject()
{
    return new MediaObjectModel;
}


void
MediaServerDevice::newSystemUpdateId(ui4 id)
{
    LOGNS(Gui, gui, debug, "media server device \"" + getFriendlyName() + "\" new system update id: " + Poco::NumberFormatter::format(id));

    if (!_featureHandleUpdateIdChange) {
        return;
    }
    // FIXME: crashes / timeouts when updating system id on device discovery in controller
    // FIXME: avoid handling system update id when device is discovered


//    Av::CtlMediaObject* pObject = getRootObject();
//    if (pObject) {
//        // clear cache (reset data model)
//        pObject->clear();
//        // if total item count of list model is 0, no items are fetched and thus total item count is not updated
//        // so we need to fetch first child in the current search context
//        pObject->getChildForRow(0);
//        // sync view
////        MediaContainerWidget* pContainer = dynamic_cast<MediaContainerWidget*>(_pServerGroupWidget->getVisibleView());
////        pContainer->syncView();
//    }


//    if (!_pServerGroupWidget) {
//        return;
//    }
//
//    // get (object id of) container, that is on top of navigator
//    // can also be a MediaServerGroupWidget, thus the dynamic_cast
//    MediaContainerWidget* pContainer = dynamic_cast<MediaContainerWidget*>(_pServerGroupWidget->getVisibleView());
//    if (pContainer && _pServerGroupWidget->getSelectedDevice() && _pServerGroupWidget->getSelectedDevice()->getUuid() == getUuid()) {
//        Av::CtlMediaObject* pObject = pContainer->_pObjectModel;
//        if (pObject) {
//            // clear cache (reset data model)
//            pObject->clear();
//            // if total item count of list model is 0, no items are fetched and thus total item count is not updated
//            // so we need to fetch first child in the current search context
//            pObject->getChildForRow(0);
//            // sync view
//            pContainer->syncView();
//        }
//    }
}


MediaServerView::MediaServerView()
{
    setName("media server view");
    if (Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        setBackgroundColor(Gui::Color("black"));
    }
//    setSpacing(50);
}

} // namespace Omm
