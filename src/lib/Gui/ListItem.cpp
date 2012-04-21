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


ListItemModel::ListItemModel() :
_pLabelModel(0),
_pImageModel(0)
{
}


ListItemModel::ListItemModel(const ListItemModel& model) :
Model(model),
_pLabelModel(model._pLabelModel),
_pImageModel(model._pImageModel)
{
}


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
    setName("list item view");

    _pImageView = new ImageView(this);
    _pImageView->setStretchFactor(-1.0);
    _pLabelView = new LabelView(this);
    _pLabelView->setBackgroundColor(Color("white"));
    _pLabelView->setStretchFactor(-1.0);
    _pLabelView->setSizeConstraint(220, 20, View::Pref);

    _pLayout = new HorizontalLayout;
    setLayout(_pLayout);
}


void
ListItemView::setModel(Model* pModel)
{
    Omm::Gui::Log::instance()->gui().debug("list item view set model: " + Poco::NumberFormatter::format(pModel) + " ...");
    if (!pModel) {
        Omm::Gui::Log::instance()->gui().error("list item view set model failed, model is null");
        return;
    }
    ListItemModel* pListItemModel = static_cast<ListItemModel*>(pModel);
    _pImageView->setModel(pListItemModel->_pImageModel);
    _pLabelView->setModel(pListItemModel->_pLabelModel);
    View::setModel(pModel);
    Omm::Gui::Log::instance()->gui().debug("list item view set model finished.");
}


void
ListItemView::syncViewImpl()
{
    Omm::Gui::Log::instance()->gui().debug("list item view sync view impl: " + getName() + " ...");
    ListItemModel* pItemModel = static_cast<ListItemModel*>(_pModel);
    if (pItemModel->_pLabelModel) {
        _pLabelView->syncViewImpl();
    }
    if (pItemModel->_pImageModel) {
        _pImageView->syncViewImpl();
    }
    Omm::Gui::Log::instance()->gui().debug("list item view sync view impl: " + getName() + " finished.");
}


void
ListItemView::setHighlighted(bool highlighted)
{
    Omm::Gui::Log::instance()->gui().debug("list item view set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
    _pLabelView->setHighlighted(highlighted);
}


} // namespace Gui
} // namespace Omm
