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


namespace Omm {
namespace Gui {


class Label;
class HorizontalLayout;

//class ListItemController : public Controller
//{
//protected:
//
//private:
//    virtual void selected();
//};


class ListItemModel : public Model
{
public:
    virtual std::string getLabel();
    void setLabel(const std::string& text);

private:
    std::string _label;
};


class ListItemView : public View
{
public:
    ListItemView(View* pParent = 0);

private:
    virtual void syncView(Model* pModel);

    Label*              _pLabel;
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
