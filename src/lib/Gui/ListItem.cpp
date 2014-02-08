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
#include "Log.h"

#include "GuiImages.h"

namespace Omm {
namespace Gui {


class ListItemLayout : public Layout
{
    virtual void layoutView()
    {
        ListItemView* pItemView = static_cast<ListItemView*>(_pView);

        int hspace = 5;
        int vspace = 1;
        int itemViewWidth = _pView->width();
        int itemViewHeight = _pView->height() - 2 * vspace;
        int arrowSize = itemViewHeight * 0.5;
        int rightViewWidth = pItemView->_pRightView ? pItemView->_pRightView->width() : 0;

        pItemView->_pImageView->resize(itemViewHeight, itemViewHeight);
        pItemView->_pLabelView->resize(itemViewWidth - 2 * itemViewHeight - 2 * hspace - rightViewWidth, itemViewHeight);
        pItemView->_pArrowView->resize(arrowSize, arrowSize);

        pItemView->_pImageView->move(hspace, vspace);
        pItemView->_pLabelView->move(2 * hspace + itemViewHeight, vspace);
        pItemView->_pArrowView->move(itemViewWidth - arrowSize - hspace, vspace + (itemViewHeight - arrowSize) * 0.5);

        if (pItemView->_pRightView) {
            pItemView->_pRightView->setHeight(itemViewHeight);
            pItemView->_pRightView->move(itemViewWidth - itemViewHeight - rightViewWidth, vspace);
        }
    }
};


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

// FIXME: there is a double delete when implementing dtor of ListItemModel
//ListItemModel::~ListItemModel()
//{
//    if (_pLabelModel) {
//        delete _pLabelModel;
//        _pLabelModel = 0;
//    }
//    if (_pImageModel) {
//        delete _pImageModel;
//        _pImageModel = 0;
//    }
//}


LabelModel*
ListItemModel::getLabelModel()
{
    return _pLabelModel;
}


void
ListItemModel::setLabelModel(LabelModel* pLabelModel)
{
    _pLabelModel = pLabelModel;
}


ImageModel*
ListItemModel::getImageModel()
{
    return _pImageModel;
}


void
ListItemModel::setImageModel(ImageModel* pImageModel)
{
    _pImageModel = pImageModel;
}

ImageModel* ListItemView::_pArrowModel = 0;

ListItemView::ListItemView(View* pParent) :
View(pParent),
_pRightView(0)
{
    setName("list item view");
    setBackgroundColor(Color("white"));

    _pImageView = new ImageView(this);
    _pImageView->setBackgroundColor(Color("white"));

    _pLabelView = new LabelView(this);
    _pLabelView->setBackgroundColor(Color("white"));

    _pArrowView = new ImageView(this);
    _pArrowView->setBackgroundColor(Color("white"));

    if (!_pArrowModel) {
        _pArrowModel = new ImageModel;
        _pArrowModel->setData(GuiImages::instance()->getResource("right_arrow.gif"));
    }
    _pArrowView->setModel(_pArrowModel);
    _pArrowView->hide(false);

    _pLayout = new ListItemLayout;
    setLayout(_pLayout);
}


void
ListItemView::setModel(Model* pModel)
{
//    LOG(gui, debug, "list item view set model: " + Poco::NumberFormatter::format(pModel) + " ...");
    if (!pModel) {
        LOG(gui, error, "list item view set model failed, model is null");
        return;
    }
    ListItemModel* pListItemModel = static_cast<ListItemModel*>(pModel);
    _pImageView->setModel(pListItemModel->_pImageModel);
    _pLabelView->setModel(pListItemModel->_pLabelModel);
    View::setModel(pModel);
//    LOG(gui, debug, "list item view set model finished.");
}


void
ListItemView::setSpacing(int hSpace)
{
    _pImageView->setSizeConstraint(width(View::Pref) + hSpace, height(View::Pref));
}


void
ListItemView::showRightArrow(bool show)
{
    if (show) {
        _pArrowView->show();
    }
    else {
        _pArrowView->hide();
    }
}


void
ListItemView::setRightView(View* pView)
{
    _pRightView = pView;
    pView->setParent(this);
}


void
ListItemView::syncViewImpl()
{
//    LOG(gui, debug, "list item view sync view impl: " + getName() + " ...");
    ListItemModel* pItemModel = static_cast<ListItemModel*>(_pModel);
    if (pItemModel->_pLabelModel) {
        _pLabelView->syncViewImpl();
    }
    if (pItemModel->_pImageModel) {
        _pImageView->setStretchFactor(-1.0);
        _pImageView->syncViewImpl();
    }
    else {
        _pImageView->setStretchFactor(0.2);
    }
    if (_pArrowModel) {
        _pArrowView->syncViewImpl();
    }

//    LOG(gui, debug, "list item view sync view impl: " + getName() + " finished.");
}


} // namespace Gui
} // namespace Omm
