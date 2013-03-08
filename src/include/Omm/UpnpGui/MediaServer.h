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

#ifndef MediaServer_INCLUDED
#define MediaServer_INCLUDED

#include "../Upnp.h"
#include "../UpnpAvCtlServer.h"
#include "../UpnpAvCtlObject.h"

#include "../Gui/ListModel.h"
#include "../Gui/List.h"
#include "../Gui/ListItem.h"

#include "DeviceGroup.h"


namespace Omm {

class MediaServerGroupWidget;
class MediaServerDevice;


class MediaServerGroupWidget : public DeviceGroupWidget
{
public:
    MediaServerGroupWidget();

    // Omm::DeviceGroup interface
    virtual Device* createDevice();

    // ListModel interface
    virtual Gui::View* createItemView();
    virtual Gui::Model* getItemModel(int row);

    // ListController interface
#ifdef __IPHONE__
    virtual void selectedItem(int row);
#else
    virtual void activatedItem(int row);
#endif

    // NavigatorController delegate
    virtual void changedSearchText(const std::string& searchText);

private:
    std::string         _searchString;
};


class MediaServerDevice : public Av::CtlMediaServer, public Gui::ListItemModel
{
public:
    MediaServerDevice(MediaServerGroupWidget* pServerGroupWidget) :
    _pServerGroupWidget(pServerGroupWidget), _featureHandleUpdateIdChange(false) {}

    virtual void initController();

    virtual Av::CtlMediaObject* createMediaObject();

    virtual void newSystemUpdateId(ui4 id);

private:
    MediaServerGroupWidget*     _pServerGroupWidget;
    bool                        _featureHandleUpdateIdChange;
};


class MediaServerView : public Gui::ListItemView
{
public:
    MediaServerView();

};


}  // namespace Omm


#endif
