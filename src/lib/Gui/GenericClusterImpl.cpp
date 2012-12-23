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
    GenericClusterController(ClusterView* pClusterView, View* pView) : _pClusterView(pClusterView), _pView(pView) {}

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
    }

private:
    ClusterView*    _pClusterView;
    View*           _pView;
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
    if (!_pViewImpl->_views.size()) {
        return;
    }

    int handleHeight = (_pViewImpl->_handleBarHidden ? 0 : _pViewImpl->_handleHeight);
    int handleWidth = _pView->width() / _pViewImpl->_handles.size();

    int handleIndex = 0;
    for (ClusterView::SubviewIterator itv = _pViewImpl->_views.begin(); itv != _pViewImpl->_views.end(); ++handleIndex, ++itv) {
        ListItemView* pHandle = _pViewImpl->_handles[*itv];
        pHandle->resize(handleWidth, handleHeight);
        pHandle->move(handleIndex * handleWidth, 0);
        (*itv)->resize(_pView->width(), _pView->height() - handleHeight);
        (*itv)->move(0, handleHeight);
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
    ClusterView* pView = static_cast<ClusterView*>(_pView);
    pView->setLayout(new GenericClusterStackedLayout(this));
}


void
GenericClusterViewImpl::insertView(View* pView, const std::string& name, int index)
{
    LOG(gui, debug, "generic cluster insert view: " + pView->getName());
    pView->setParent(_pView);
    pView->resize(_pView->width(), _pView->height() - _handleHeight);
    pView->move(0, _handleHeight);
    pView->show();

    std::string handleName = "handle " + Poco::NumberFormatter::format(_views.size());
    LabelModel* pHandleLabelModel = new LabelModel;
    pHandleLabelModel->setLabel(name);
    ListItemModel* pHandleModel = new ListItemModel;
    pHandleModel->setLabelModel(pHandleLabelModel);
    ListItemView* pHandle = new ListItemView(_pView);
    pHandle->setModel(pHandleModel);
    pHandle->setName(handleName);
    HandleController* pHandleController = new HandleController(this, pView);
    pHandle->attachController(pHandleController);
    pHandle->setAcceptDrops(true);
    pHandle->attachController(new GenericClusterController(static_cast<ClusterView*>(_pView), pView));
    pHandle->show();

    _handles[pView] = pHandle;
    _views.insert(_views.begin() + index, pView);
    _currentViewIndex = index;
    _pView->updateLayout();
}


void
GenericClusterViewImpl::removeView(View* pView)
{
    ClusterView::SubviewIterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        ListItemView* pHandle = _handles[pView];
        pHandle->hide();
        _handles.erase(pView);
        _views.erase(it);
    }
    _pView->updateLayout();
    if (!_views.size()) {
        _pView->hide();
    }
}


int
GenericClusterViewImpl::getViewCount()
{
    return _views.size();
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
    LOG(gui, debug, "cluster set current view index: " + Poco::NumberFormatter::format(index));

    if (index == -1) {
        return;
    }

    View* pCurrentView = _views[_currentViewIndex];
    if (pCurrentView) {
        View* pHandle = _handles[pCurrentView];
        if (pHandle) {
            pHandle->setBackgroundColor(Color("white"));
        }
    }
    _currentViewIndex = index;
    pCurrentView = _views[_currentViewIndex];
    if (pCurrentView) {
        _views[_currentViewIndex]->raise();
        View* pHandle = _handles[pCurrentView];
        if (pHandle) {
            pHandle->setBackgroundColor(Color(200, 200, 200, 255));
        }
    }
}



int
GenericClusterViewImpl::getIndexFromView(View* pView)
{
    std::vector<View*>::iterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        return it - _views.begin();
    }
    else {
        return -1;
    }
}


void
GenericClusterViewImpl::setHandlesHidden(bool hidden)
{
    _handleBarHidden = hidden;
    for (View::SubviewIterator it = _views.begin(); it != _views.end(); ++it) {
        (*it)->resize(_pView->width(), _pView->height() - (hidden ? 0 : _handleHeight));
        (*it)->move(0, (hidden ? 0 : _handleHeight));
    }
}


const int
GenericClusterViewImpl::getHandleHeight()
{
    return _handleHeight;
}



} // namespace Gui
} // namespace Omm
