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

#ifndef MediaObject_INCLUDED
#define MediaObject_INCLUDED

#include "../Upnp.h"
#include "../UpnpAvCtlObject.h"

#include "../Gui/Button.h"
#include "../Gui/ListModel.h"
#include "../Gui/List.h"
#include "../Gui/ListItem.h"
#include "../Gui/TextLine.h"


namespace Omm {

class UpnpApplication;
class ControllerWidget;
class MediaObjectModel;
class GuiVisual;
class MediaServerGroupWidget;
class MediaServerDevice;
class MediaRendererGroupWidget;
class MediaRendererView;
class MediaObjectView;
class PlaylistEditor;
class PlaylistEditorObjectView;
class ActivityIndicator;


class MediaContainerWidget : public Gui::ListView, Gui::ListModel, Gui::ListController
{
    friend class MediaServerGroupWidget;
    friend class MediaServerDevice;

public:
    MediaContainerWidget(View* pParent = 0);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
//    virtual void selectedItem(int row);
    virtual void activatedItem(int row);

    void createPlaylist(const std::string& playlistName);

private:
    MediaObjectModel*           _pObjectModel;
    MediaServerGroupWidget*     _pServerGroup;
};


class MediaContainerPlaylistCreator : public Gui::TextLine
{
public:
    MediaContainerPlaylistCreator(MediaContainerWidget* pMediaContainer);

private:
    virtual void editedText(const std::string& text);

    MediaContainerWidget*       _pMediaContainer;
};


class MediaObjectModel : public Av::CtlMediaObject, public Gui::ListItemModel
{
    friend class MediaContainerWidget;

public:
    MediaObjectModel();
    MediaObjectModel(const MediaObjectModel& objectModel);

private:
    class MediaObjectLabelModel : public Gui::LabelModel
    {
    public:
        MediaObjectLabelModel(MediaObjectModel* pSuperModel) : _pSuperModel(pSuperModel) {}

        virtual std::string getLabel();

        MediaObjectModel*   _pSuperModel;
    };

    MediaContainerWidget*     _pContainer;
    static Gui::ImageModel*   _pContainerImageModel;
    static Gui::ImageModel*   _pItemImageModel;
    static Gui::ImageModel*   _pItemAudioItemModel;
    static Gui::ImageModel*   _pItemImageItemModel;
    static Gui::ImageModel*   _pItemVideoItemModel;
    static Gui::ImageModel*   _pItemAudioBroadcastModel;
    static Gui::ImageModel*   _pItemVideoBroadcastModel;
};


class MediaObjectViewPlaylistButtonController : public Gui::ButtonController
{
public:
    MediaObjectViewPlaylistButtonController(MediaObjectView* pMediaObjectView);

private:
    // ButtonController interface
    virtual void pushed();

    MediaObjectView*    _pMediaObjectView;
};


//class MediaObjectViewDestroyButtonController : public Gui::ButtonController
//{
//public:
//    MediaObjectViewDestroyButtonController(MediaObjectView* pMediaObjectView);
//
//private:
//    // ButtonController interface
//    virtual void pushed();
//
//    MediaObjectView*    _pMediaObjectView;
//};


class MediaObjectView : public Gui::ListItemView
{
public:
    MediaObjectView(View* pParent = 0, bool playlistButton = true);

private:
    Gui::Button*         _pPlaylistButton;
//    Gui::Button*         _pDestroyButton;
};


}  // namespace Omm


#endif
