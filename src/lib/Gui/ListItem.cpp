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
#include "Gui/HorizontalLayout.h"


namespace Omm {
namespace Gui {


void
ListItemModel::setLabelModel(LabelModel* pLabelModel)
{
    _pLabelModel = pLabelModel;
}


void
ListItemModel::setImageModel(ImageModel* pImageModel)
{
    _pImageModel = pImageModel;
}


ListItemView::ListItemView(View* pParent) :
View(pParent)
{
    _pImageView = new ImageView(this);
    _pLabelView = new LabelView(this);
    _pLabelView->setBackgroundColor(Color("white"));

    _pLayout = new HorizontalLayout;
    setLayout(_pLayout);
}


void
ListItemView::syncView(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("list item view sync view: " + getName());
    ListItemModel* pItemModel = static_cast<ListItemModel*>(pModel);
    _pLabelView->syncView(pItemModel->_pLabelModel);
    _pImageView->syncView(pItemModel->_pImageModel);
}


void
ListItemView::setHighlighted(bool highlighted)
{
    Omm::Gui::Log::instance()->gui().debug("list item view set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
    _pLabelView->setHighlighted(highlighted);
}


} // namespace Gui
} // namespace Omm
