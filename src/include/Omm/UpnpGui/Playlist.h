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

#ifndef Playlist_INCLUDED
#define Playlist_INCLUDED

#include "../UpnpAvCtlObject.h"

#include "../Gui/ListModel.h"
#include "../Gui/List.h"
#include "../Gui/ListItem.h"
#include "../Gui/Button.h"
#include "../Gui/Label.h"
#include "../Gui/HorizontalLayout.h"


namespace Omm {

class ControllerWidget;
class MediaObjectModel;

class PlaylistNotification : public Poco::Notification
{
public:
    PlaylistNotification(MediaObjectModel* pMediaObject) : _pMediaObject(pMediaObject) {}

    MediaObjectModel*   _pMediaObject;
};


class PlaylistEditor : public Gui::ListView, Gui::ListModel, Gui::ListController
{
    friend class PlaylistEditorDeleteObjectController;

public:
    PlaylistEditor(ControllerWidget* pControllerWidget);

    // ListModel interface
    virtual int totalItemCount();
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
    virtual void draggedItem(int row);
    virtual void droppedItem(Gui::Model* pModel, int row);

    void setPlaylistContainer(MediaObjectModel* pPlaylistContainer);
    std::stringstream* getPlaylistResource();
    std::string getPlaylistResourceUri();
    void writePlaylistResource();

    void deleteItem(MediaObjectModel* pModel);

private:
    void playlistNotification(PlaylistNotification* pNotification);

    ControllerWidget*                   _pControllerWidget;
    MediaObjectModel*                   _pPlaylistContainer;
    std::vector<MediaObjectModel*>      _playlistItems;
};


class PlaylistEditorView : public Gui::View
{
    friend class PlaylistEditorViewHideButtonController;
    friend class ControllerWidget;

    PlaylistEditorView(PlaylistEditor* pPlaylistEditor);
};


class PlaylistEditorObjectView : public Gui::ListItemView
{
    friend class PlaylistEditorDeleteObjectController;

public:
    PlaylistEditorObjectView(PlaylistEditor* pPlaylistEditor, View* pParent = 0);

private:
    PlaylistEditor*      _pPlaylistEditor;
    Gui::Button*         _pDeleteButton;
};


class PlaylistEditorDeleteObjectController : public Gui::ButtonController
{
public:
    PlaylistEditorDeleteObjectController(PlaylistEditorObjectView* pPlaylistEditorObjectView);

private:
    // ButtonController interface
    virtual void pushed();

    PlaylistEditorObjectView*    _pPlaylistEditorObjectView;
};


}  // namespace Omm


#endif
