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

#include <Poco/NumberFormatter.h>

#include "Gui/ListItem.h"
#include "Gui/GuiLogger.h"
#include "Gui/Label.h"
#include "Gui/HorizontalLayout.h"


namespace Omm {
namespace Gui {


std::string
ListItemModel::getLabel()
{
    return _label;
}


void
ListItemModel::setLabel(const std::string& label)
{
//    Omm::Gui::Log::instance()->gui().debug("list item model set label");
    _label = label;

    syncViews();
}


ListItemView::ListItemView(View* pParent) :
View(pParent)
{
    _pLabel = new Label(this);
    _pLayout = new HorizontalLayout;
    setLayout(_pLayout);
}


void
ListItemView::syncView(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("list item view sync view: " + getName());
    ListItemModel* pItemModel = static_cast<ListItemModel*>(pModel);
    _pLabel->setLabel(pItemModel->getLabel());
}


void
ListItemView::setSelected(bool selected)
{
    Omm::Gui::Log::instance()->gui().debug("list item view set selected: " + (selected ? std::string("true") : std::string("false")));
    _pLabel->setSelected(selected);
}

//void
//ListItemController::selected()
//{
//    Omm::Gui::Log::instance()->gui().debug("list item controller selected row: " + Poco::NumberFormatter::format(_row));
//}



} // namespace Gui
} // namespace Omm
