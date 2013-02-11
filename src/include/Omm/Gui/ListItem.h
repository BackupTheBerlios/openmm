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

#ifndef ListItem_INCLUDED
#define ListItem_INCLUDED

#include "View.h"
#include "Image.h"
#include "Label.h"

namespace Omm {
namespace Gui {

class HorizontalLayout;


class ListItemModel : public Model
{
    friend class ListItemView;

public:
    ListItemModel();
    ListItemModel(const ListItemModel& model);
    ~ListItemModel();

    LabelModel* getLabelModel();
    void setLabelModel(LabelModel* pLabelModel);
    ImageModel* getImageModel();
    void setImageModel(ImageModel* pImageModel);

private:
    ImageModel*     _pImageModel;
    LabelModel*     _pLabelModel;
};


class ListItemView : public View
{
public:
    ListItemView(View* pParent = 0);
    virtual void setModel(Model* pModel = 0);
    void setSpacing(int hSpace);

private:
    virtual void syncViewImpl();

    ImageView*          _pImageView;
    LabelView*          _pLabelView;
    HorizontalLayout*   _pLayout;
};


//class ListItem : public Widget<ListItemView, ListItemController, ListItemModel>
//{
//public:
//    ListItem() : Widget<ListItemView, ListItemController, ListItemModel>() {}
//};


}  // namespace Omm
}  // namespace Gui

#endif
