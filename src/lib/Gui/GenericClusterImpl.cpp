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

    if (!_pViewImpl->_views.size()) {
        // cluster contains no views
        return;
    }

    int handleHeight = (_pViewImpl->_handleBarHidden ? 0 : _pViewImpl->_handleHeight);
    int handleWidth = _pView->width() / _pViewImpl->_visibleViews.size();
    _pViewImpl->_pSelection->resize(handleWidth, handleHeight);

    int handleIndex = 0;
    for (std::vector<View*>::iterator it = _pViewImpl->_visibleViews.begin(); it != _pViewImpl->_visibleViews.end(); ++handleIndex, ++it) {
        GenericClusterViewImpl::HandleView* pHandle = _pViewImpl->_handles[*it];
        pHandle->resize(handleWidth, handleHeight);
        pHandle->move(handleIndex * handleWidth, 0);
        (*it)->resize(_pView->width(), _pView->height() - handleHeight);
        (*it)->move(0, handleHeight);
    }
    handleIndex = 0;
    for (std::set<View*>::iterator it = _pViewImpl->_hiddenViews.begin(); it != _pViewImpl->_hiddenViews.end(); ++handleIndex, ++it) {
        GenericClusterViewImpl::HandleView* pHandle = _pViewImpl->_handles[*it];
        pHandle->hide(false);
        (*it)->hide(false);
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
    _pSelection = new SelectionView;
}


void
GenericClusterViewImpl::init()
{
    _pView->setLayout(new GenericClusterStackedLayout(this));
    _pSelection->setParentView(_pView);
    _pSelection->resize(_handleWidth, _handleHeight);
    _pSelection->hide(false);
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
    HandleView* pHandle = new HandleView(_pView);
//    pHandle->setModel(pHandleLabelModel);
    pHandle->setModel(pHandleModel);
    pHandle->setName(handleName);
    HandleController* pHandleController = new HandleController(this, pView);
    pHandle->attachController(pHandleController);
    pHandle->setAcceptDrops(true);
    pHandle->attachController(new GenericClusterController(this, static_cast<ClusterView*>(_pView), pView));
    pHandle->show();

    _handles[pView] = pHandle;
    _views.insert(std::make_pair(pView->getName(), pView));
    _visibleViews.insert(_visibleViews.begin() + index, pView);
    _currentViewIndex = index;
    _pView->updateLayout();
}


void
GenericClusterViewImpl::removeView(View* pView)
{
    _views.erase(pView->getName());
    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (it != _visibleViews.end()) {
        HandleView* pHandle = _handles[pView];
        pHandle->hide();
        _handles.erase(pView);
        _visibleViews.erase(it);
    }
    _hiddenViews.erase(pView);
    if (!_visibleViews.size()) {
        _pView->hide();
    }
    else {
        _pView->updateLayout();
    }
}


std::string
GenericClusterViewImpl::getConfiguration()
{
    std::string res;
    for (std::vector<View*>::const_iterator it = _visibleViews.begin(); it != _visibleViews.end(); ++it) {
        res += (*it)->getName();
        if (it + 1 != _visibleViews.end()) {
            res += ",";
        }
    }
    return res;
}


void
GenericClusterViewImpl::setConfiguration(const std::string& configuration)
{
    _visibleViews.clear();
    for (ViewIterator it = _views.begin(); it != _views.end(); ++it) {
        _hiddenViews.insert(it->second);
    }
    Poco::StringTokenizer columnTokens(configuration, " ");
    for (Poco::StringTokenizer::Iterator cit = columnTokens.begin(); cit != columnTokens.end(); ++cit) {
        // remove all metrical and topological information (e.g. from column cluster view)
        if ((*cit)[0] != '[' && (*cit)[0] != '{') {
            Poco::StringTokenizer views(*cit, ",");
            for (Poco::StringTokenizer::Iterator it = views.begin(); it != views.end(); ++it) {
                ViewIterator pos = _views.find(*it);
                if (pos != _views.end()) {
                    _visibleViews.push_back(pos->second);
                    _hiddenViews.erase(pos->second);
                }
            }
        }
    }
    _pView->updateLayout();
    setCurrentViewIndex(0);
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

    _currentViewIndex = index;
    View* pCurrentView = _visibleViews[_currentViewIndex];
    LOG(gui, debug, "cluster " + _pView->getName() + " set current view index: " + Poco::NumberFormatter::format(index) + ", view: " + pCurrentView->getName());
    if (pCurrentView) {
        _visibleViews[_currentViewIndex]->raise();
        View* pHandle = _handles[pCurrentView];
        if (pHandle) {
            _pSelection->move(pHandle->posX(), pHandle->posY());
            _pSelection->show();
            _pSelection->raise();
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


} // namespace Gui
} // namespace Omm
