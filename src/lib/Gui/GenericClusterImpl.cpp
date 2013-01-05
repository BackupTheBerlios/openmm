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
#include <map>
#include <vector>
#include <Poco/StringTokenizer.h>

#include "Log.h"
#include "Gui/Cluster.h"
#include "Gui/GuiLogger.h"
#include "Gui/Layout.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/Label.h"
#include "Gui/Button.h"
#include "Gui/ListItem.h"
#include "Gui/Drag.h"
#include "GenericClusterImpl.h"


namespace Omm {
namespace Gui {


class GenericClusterController : public Controller
{
public:
    GenericClusterController(GenericClusterViewImpl* pViewImpl, ClusterView* pClusterView, View* pView) : _pViewImpl(pViewImpl), _pClusterView(pClusterView), _pView(pView) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "generic cluster drag controller drag started in source view: " + _pView->getName());
        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(_pView, _pView->getModel());
        pDrag->start();
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "generic cluster drag controller drag moved in target view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "generic cluster drag controller dropped in target view: " + _pView->getName() + ", source view: " + pDrag->getSource()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
        int index = _pClusterView->getIndexFromView(_pView);
        _pClusterView->insertView(pDrag->getSource(), pDrag->getSource()->getName(), index);
        _pViewImpl->changedConfiguration();
    }

private:
    GenericClusterViewImpl*     _pViewImpl;
    ClusterView*                _pClusterView;
    View*                       _pView;
};


class GenericClusterStackedLayout : public Layout
{
public:
    GenericClusterStackedLayout(GenericClusterViewImpl* pViewImpl) : _pViewImpl(pViewImpl) {}

    virtual void layoutView();

    GenericClusterViewImpl*     _pViewImpl;
};


void
GenericClusterStackedLayout::layoutView()
{
    LOG(gui, debug, "generic cluster view " + _pView->getName() + " update layout with current size [" + Poco::NumberFormatter::format(_pView->width()) + ", " + Poco::NumberFormatter::format(_pView->height()) + "]");

    if (!_pViewImpl->_visibleViews.size()) {
        return;
    }

    int handleHeight = (_pViewImpl->_handleBarHidden ? 0 : _pViewImpl->_handleHeight);
    int handleWidth = _pView->width() / _pViewImpl->_handles.size();

    int handleIndex = 0;
    for (ClusterView::SubviewIterator itv = _pViewImpl->_visibleViews.begin(); itv != _pViewImpl->_visibleViews.end(); ++handleIndex, ++itv) {
        ListItemView* pHandle = _pViewImpl->_handles[*itv];
        pHandle->resize(handleWidth, handleHeight);
        pHandle->move(handleIndex * handleWidth, 0);
        (*itv)->resize(_pView->width(), _pView->height() - handleHeight);
        (*itv)->move(0, handleHeight);
//        if (_pView->width() < (*itv)->width(View::Min)
//                || _pView->width() > (*itv)->width(View::Max)
//                || _pView->height() - handleHeight < (*itv)->height(View::Min)
//                || _pView->height() - handleHeight > (*itv)->height(View::Max))  {
//            _pViewImpl->sizeConstraintReached(*itv);
//        }
    }
}


class HandleController : public Controller
{
public:
    HandleController(GenericClusterViewImpl* pClusterViewImpl, View* pView) : _pClusterViewImpl(pClusterViewImpl), _pView(pView) {}

    virtual void selected()
    {
        _pClusterViewImpl->setCurrentViewIndex(_pClusterViewImpl->getIndexFromView(_pView));
    }

private:
    GenericClusterViewImpl*     _pClusterViewImpl;
    View*                       _pView;
};


GenericClusterViewImpl::GenericClusterViewImpl(View* pView) :
PlainViewImpl(pView),
_handleHeight(25),
_handleWidth(150),
_handleBarHidden(false),
_currentViewIndex(-1)
{
}


void
GenericClusterViewImpl::init()
{
    _pView->setLayout(new GenericClusterStackedLayout(this));
}


void
GenericClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
    LOG(gui, debug, "cluster " + _pView->getName() + " insert view: " + pView->getName());
    pView->setParent(_pView);
    pView->resize(_pView->width(), _pView->height() - _handleHeight);
    pView->move(0, _handleHeight);
    pView->show();

    std::string handleName = "handle " + Poco::NumberFormatter::format(_visibleViews.size());
    LabelModel* pHandleLabelModel = new LabelModel;
    pHandleLabelModel->setLabel(label);
    ListItemModel* pHandleModel = new ListItemModel;
    pHandleModel->setLabelModel(pHandleLabelModel);
    ListItemView* pHandle = new ListItemView(_pView);
    pHandle->setModel(pHandleModel);
    pHandle->setName(handleName);
    HandleController* pHandleController = new HandleController(this, pView);
    pHandle->attachController(pHandleController);
    pHandle->setAcceptDrops(true);
    pHandle->attachController(new GenericClusterController(this, static_cast<ClusterView*>(_pView), pView));
    pHandle->show();

    _handles[pView] = pHandle;
    _visibleViews.insert(_visibleViews.begin() + index, pView);
    _views.insert(std::make_pair(pView->getName(), pView));
    _currentViewIndex = index;
//    updateSizeConstraints();
    _pView->updateLayout();
}


void
GenericClusterViewImpl::removeView(View* pView)
{
    ClusterView::SubviewIterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (it != _visibleViews.end()) {
        ListItemView* pHandle = _handles[pView];
        pHandle->hide();
        _handles.erase(pView);
        _visibleViews.erase(it);
    }
    _views.erase(pView->getName());
    if (!_visibleViews.size()) {
        _pView->hide();
    }
    else {
//        updateSizeConstraints();
        _pView->updateLayout();
    }
}


std::string
GenericClusterViewImpl::getConfiguration()
{

}


void
GenericClusterViewImpl::setConfiguration(const std::string& configuration)
{
    _visibleViews.clear();
    Poco::StringTokenizer views(configuration, ",");
    for (Poco::StringTokenizer::Iterator it = views.begin(); it != views.end(); ++it) {
        _visibleViews.push_back(_views[*it]);
    }
    _pView->updateLayout();
}


int
GenericClusterViewImpl::getViewCount()
{
    return _visibleViews.size();
}


int
GenericClusterViewImpl::getCurrentViewIndex()
{
    LOG(gui, debug, "cluster get current view index: " + Poco::NumberFormatter::format(_currentViewIndex));

    return _currentViewIndex;
}


void
GenericClusterViewImpl::setCurrentViewIndex(int index)
{
    if (index == -1) {
        return;
    }

    View* pCurrentView = _visibleViews[_currentViewIndex];
    if (pCurrentView) {
        View* pHandle = _handles[pCurrentView];
        if (pHandle) {
            pHandle->setBackgroundColor(Color("white"));
        }
    }
    _currentViewIndex = index;
    pCurrentView = _visibleViews[_currentViewIndex];
    LOG(gui, debug, "cluster " + _pView->getName() + " set current view index: " + Poco::NumberFormatter::format(index) + ", view: " + pCurrentView->getName());
    if (pCurrentView) {
        _visibleViews[_currentViewIndex]->raise();
        View* pHandle = _handles[pCurrentView];
        if (pHandle) {
            pHandle->setBackgroundColor(Color(200, 200, 200, 255));
        }
    }
}



int
GenericClusterViewImpl::getIndexFromView(View* pView)
{
    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (it != _visibleViews.end()) {
        return it - _visibleViews.begin();
    }
    else {
        return -1;
    }
}


View*
GenericClusterViewImpl::getViewFromIndex(int index)
{
    return _visibleViews[index];
}


void
GenericClusterViewImpl::setHandlesHidden(bool hidden)
{
    _handleBarHidden = hidden;
    for (View::SubviewIterator it = _visibleViews.begin(); it != _visibleViews.end(); ++it) {
        (*it)->resize(_pView->width(), _pView->height() - (hidden ? 0 : _handleHeight));
        (*it)->move(0, (hidden ? 0 : _handleHeight));
    }
}


const int
GenericClusterViewImpl::getHandleHeight()
{
    return _handleHeight;
}


void
GenericClusterViewImpl::changedConfiguration()
{
    IMPL_NOTIFY_CONTROLLER(ClusterController, changedConfiguration);
}


//void
//GenericClusterViewImpl::sizeConstraintReached(View::SizeConstraint& width, View::SizeConstraint& height)
//{
//    width = View::None;
//    height = View::None;
//    for (View::SubviewIterator it = _views.begin(); it != _views.end(); ++it) {
//        View* pView = (*it);
//        LOG(gui, debug, "cluster " + _pView->getName() + " subview " + pView->getName() + " current size ["
//            + Poco::NumberFormatter::format(pView->width(View::Current)) + ", "
//            + Poco::NumberFormatter::format(pView->height(View::Current)) + "]");
//        LOG(gui, debug, "cluster " + _pView->getName() + " subview " + pView->getName() + " size constraint Min ["
//            + Poco::NumberFormatter::format(pView->width(View::Min)) + ", "
//            + Poco::NumberFormatter::format(pView->height(View::Min)) + "] : Max ["
//            + Poco::NumberFormatter::format(pView->width(View::Max)) + ", "
//            + Poco::NumberFormatter::format(pView->height(View::Max)) + "]");
//
//        if ((*it)->width(View::Current) <= (*it)->width(View::Pref) * 0.66) {
//            width = View::Min;
//        }
////        else if ((*it)->width(View::Current) > (*it)->width(View::Max)) {
////            width = View::Max;
//        else if ((*it)->width(View::Current) > (*it)->width(View::Pref) * 1.33) {
//            width = View::Max;
//        }
//        if ((*it)->height(View::Current) <= (*it)->height(View::Pref) * 0.66) {
//            height = View::Min;
//        }
////        else if ((*it)->height(View::Current) > (*it)->height(View::Max)) {
////            height = View::Max;
//        else if ((*it)->height(View::Current) > (*it)->height(View::Pref) * 1.33) {
//            height = View::Max;
//        }
//    }
//}


//std::string
//GenericClusterViewImpl::writeLayout()
//{
//    std::string res;
//    int index = 0;
//    for (View::SubviewIterator it = _views.begin(); it != _views.end(); ++index, ++it) {
//        res += "{" + Poco::NumberFormatter::format(index) + ":\"" + (*it)->getName() + "\"}";
//    }
//    return res;
//}


//void
//GenericClusterViewImpl::updateSizeConstraints()
//{
//    int minWidth = 0;
//    int maxWidth = (*_views.begin())->width(View::Max);
//    int minHeight = 0;
//    int maxHeight = (*_views.begin())->height(View::Max);
//    for (View::SubviewIterator it = _views.begin(); it != _views.end(); ++it) {
//        minWidth = minWidth > (*it)->width(View::Min) ? minWidth : (*it)->width(View::Min);
//        maxWidth = maxWidth < (*it)->width(View::Max) ? maxWidth : (*it)->width(View::Max);
//        minHeight = minHeight > (*it)->height(View::Min) ? minHeight : (*it)->height(View::Min);
//        maxHeight = maxHeight < (*it)->height(View::Max) ? maxHeight : (*it)->height(View::Max);
//    }
//    _pView->setSizeConstraint(minWidth, minHeight + _handleHeight, View::Min);
//    _pView->setSizeConstraint(maxWidth, maxHeight + _handleHeight, View::Max);
//}


} // namespace Gui
} // namespace Omm
