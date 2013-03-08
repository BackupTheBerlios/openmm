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

#include <Poco/Util/Application.h>

#include "Gui/GuiLogger.h"

#include "UpnpGui/MediaServer.h"
#include "UpnpGui/Playlist.h"
#include "UpnpGui/MediaObject.h"

#include "MediaImages.h"


namespace Omm {

MediaContainerWidget::MediaContainerWidget(View* pParent) :
ListView(pParent),
_pObjectModel(0),
_pServerGroup(0)
{
    if (Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
        setItemViewHeight(80);
        showScrollBars(false);
    }
    else {
#ifdef __IPHONE__
        setItemViewHeight(50);
#else
        setItemViewHeight(30);
#endif
        setDragMode(Omm::Gui::ListView::DragSource);
    }
}


int
MediaContainerWidget::totalItemCount()
{
    LOGNS(Gui, gui, debug, "media container widget total item count: " + Poco::NumberFormatter::format(_pObjectModel->getChildCount()));
    return _pObjectModel->getChildCount();
}


Gui::View*
MediaContainerWidget::createItemView()
{
//    return new MediaObjectView;
    return new MediaObjectView(0, !Av::AvClass::matchClass(_pObjectModel->getClass(), Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER));
}


Gui::Model*
MediaContainerWidget::getItemModel(int row)
{
    LOGNS(Gui, gui, debug, "media container widget get item model in row: " + Poco::NumberFormatter::format(row));
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pObjectModel->getChildForRow(row));
    if (!pModel) {
        LOGNS(Gui, gui, error, "media container widget failed to get item model in row: " + Poco::NumberFormatter::format(row));
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
#ifdef __IPHONE__
MediaContainerWidget::selectedItem(int row)
#else
MediaContainerWidget::activatedItem(int row)
#endif
{
    LOGNS(Gui, gui, debug, "media container widget selected media object");
    MediaObjectModel* pChildObject = static_cast<MediaObjectModel*>(getItemModel(row));
    if (pChildObject && pChildObject->isContainer()) {
        LOGNS(Gui, gui, debug, "media container widget selected media container");
        MediaContainerWidget* pContainer = new MediaContainerWidget;
        pContainer->_pObjectModel = pChildObject;
        pContainer->_pServerGroup = _pServerGroup;
        pContainer->attachController(pContainer);
        pContainer->setModel(pContainer);
        // don't rely on childCount attribute being present and fetch first children to get total child count
        pChildObject->getChildForRow(0);
        if (!pChildObject->isRestricted()
                && !Av::AvClass::matchClass(pChildObject->getClass(), Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)
                && !Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {
            MediaContainerPlaylistCreator* pTopView = new MediaContainerPlaylistCreator(pContainer);
            pTopView->setTextLine("new playlist");
            pTopView->setName("top view");
            pTopView->setAlignment(Gui::View::AlignCenter);
            pContainer->addTopView(pTopView);
        }
        if (Av::AvClass::matchClass(pChildObject->getClass(), Av::AvClass::CONTAINER, Av::AvClass::PLAYLIST_CONTAINER)) {
            Poco::NotificationCenter::defaultCenter().postNotification(new PlaylistNotification(pChildObject));
        }
        _pServerGroup->push(pContainer, pChildObject->getTitle());
    }
    else {
        LOGNS(Gui, gui, debug, "media container widget selected media item");
        _pObjectModel->getServer()->selectMediaObject(pChildObject, _pObjectModel, row);
    }
}


void
MediaContainerWidget::createPlaylist(const std::string& playlistName)
{
    if (!_pObjectModel) {
        return;
    }
    if (!_pObjectModel->isRestricted()) {
        LOGNS(Gui, gui, debug, "media server group widget create playlist");
        Av::CtlMediaObject* pObject = new Av::CtlMediaObject;
        pObject->setTitle(playlistName);
        pObject->setIsContainer(false);
        pObject->setClass(Av::AvClass::className(Av::AvClass::ITEM, Av::AvClass::PLAYLIST_ITEM));
        _pObjectModel->createChildObject(pObject);
    }
}


MediaContainerPlaylistCreator::MediaContainerPlaylistCreator(MediaContainerWidget* pMediaContainer) :
_pMediaContainer(pMediaContainer)
{
}


void
MediaContainerPlaylistCreator::editedText(const std::string& text)
{
    _pMediaContainer->createPlaylist(text);
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
Av::CtlMediaObject(objectModel),
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
    LOGNS(Gui, gui, debug, "media object playlist button pushed.");
    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pMediaObjectView->getModel());
    std::string objectClass = pModel->getClass();
    LOGNS(Gui, gui, debug, "media object has class: " + objectClass);

//    Gui::Image* pImage = new Gui::Image;
//    pImage->setData(MediaImages::instance()->getResource("media-container.png"));
//    pModel->setImageModel(pImage);
    Poco::NotificationCenter::defaultCenter().postNotification(new PlaylistNotification(pModel));
}


//MediaObjectViewDestroyButtonController::MediaObjectViewDestroyButtonController(MediaObjectView* pMediaObjectView) :
//_pMediaObjectView(pMediaObjectView)
//{
//}
//
//
//void
//MediaObjectViewDestroyButtonController::pushed()
//{
//    LOGNS(Gui, gui, debug, "media object destroy button pushed.");
//    MediaObjectModel* pModel = static_cast<MediaObjectModel*>(_pMediaObjectView->getModel());
//    pModel->destroyObject(pModel->getId());
//}


MediaObjectView::MediaObjectView(View* pParent, bool playlistButton) :
ListItemView(pParent)
{
    setName("media object view");
    if (playlistButton && !Poco::Util::Application::instance().config().getBool("application.fullscreen", false)) {

        _pPlaylistButton = new Gui::Button(this);
        _pPlaylistButton->setLabel("P");
    //    _pPlaylistButton->setBackgroundColor(Gui::Color("blue"));
        _pPlaylistButton->setStretchFactor(-1.0);
    //    _pPlaylistButton->resize(20, 15);
        _pPlaylistButton->setSizeConstraint(20, 15, View::Pref);
        _pPlaylistButton->attachController(new MediaObjectViewPlaylistButtonController(this));

//        _pDestroyButton = new Gui::Button(this);
//        _pDestroyButton->setLabel("D");
//    //    _pDestroyButton->setBackgroundColor(Gui::Color("red"));
//        _pDestroyButton->setStretchFactor(-1.0);
//    //    _pDestroyButton->resize(20, 15);
//        _pDestroyButton->setSizeConstraint(20, 15, View::Pref);
//        _pDestroyButton->attachController(new MediaObjectViewDestroyButtonController(this));
    }
    else {
        setBackgroundColor(Gui::Color("black"));
    }
}


} // namespace Omm
