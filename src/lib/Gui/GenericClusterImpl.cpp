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
#include "UIKit/ViewImpl.h"


namespace Omm {
namespace Gui {


class HandleBarView : public View
{
    friend class HandleBarController;
    friend class HandleBarLayout;
    friend class GenericClusterViewImpl;


public:
    typedef ListItemView HandleView;

    HandleBarView(GenericClusterViewImpl* pCluster);

    virtual void insertView(View* pView, const std::string& label = "", int index = 0);
    virtual void removeView(View* pView);
    virtual void setCurrentView(View* pView);
    virtual const int getHandleHeight();

private:
    virtual void syncViewImpl();
    void updateCurrentView(View* pView);

    GenericClusterViewImpl*             _pCluster;
    View*                               _pCurrentView;
    int                                 _handleHeight;
    int                                 _handleWidth;
    std::map<View*, HandleView*>        _handles;
    SelectionView*                      _pSelection;
};


class HandleBarController : public Controller
{
public:
    HandleBarController(HandleBarView* pHandleBarView, View* pView) : _pHandleBarView(pHandleBarView), _pView(pView) {}

    virtual void selected()
    {
        _pHandleBarView->setCurrentView(_pView);
    }

    virtual void dragStarted()
    {
        LOG(gui, debug, "generic cluster controller drag started in source view: " + _pView->getName());
//        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(_pView, _pView->getModel());
        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(_pView, _pHandleBarView->_handles[_pView]->getModel());
        pDrag->start();
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "generic cluster controller drag moved in target view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "generic cluster controller dropped in target view: " + _pView->getName() + ", source view: " + pDrag->getSource()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");

        std::string label = pDrag->getSource()->getName();
        ListItemModel* pHandleModel = dynamic_cast<ListItemModel*>(pDrag->getModel());
        if (pHandleModel) {
            label = pHandleModel->getLabelModel()->getLabel();
        }

        int index = _pHandleBarView->_pCluster->getIndexFromView(_pView);

        _pHandleBarView->_pCluster->insertView(pDrag->getSource(), label, index);
        _pHandleBarView->_pCluster->changedConfiguration();
    }

private:
    HandleBarView*      _pHandleBarView;
    View*               _pView;
};


class HandleBarLayout : public Layout
{
public:
    virtual void layoutView();
};


void
HandleBarLayout::layoutView()
{
    LOG(gui, debug, "handle bar view \"" + _pView->getName() + "\" update layout with current size [" + Poco::NumberFormatter::format(_pView->width()) + ", " + Poco::NumberFormatter::format(_pView->height()) + "]");

    HandleBarView* pView = static_cast<HandleBarView*>(_pView);
    GenericClusterViewImpl* pCluster = pView->_pCluster;

    if (!pCluster->_visibleViews.size()) {
        // cluster contains no visible views, nothing to layout
        return;
    }

    int handleHeight = pView->_handleHeight;
    int handleWidth = pView->width() / pCluster->_visibleViews.size();
    pView->_pSelection->resize(handleWidth, handleHeight);
    pView->_pSelection->move(pCluster->getCurrentViewIndex() * handleWidth, 0);

    int handleIndex = 0;
    for (std::vector<View*>::iterator it = pCluster->_visibleViews.begin(); it != pCluster->_visibleViews.end(); ++handleIndex, ++it) {
        HandleBarView::HandleView* pHandle = pView->_handles[*it];
        pHandle->resize(handleWidth, handleHeight);
        pHandle->move(handleIndex * handleWidth, 0);
    }
    handleIndex = 0;
    for (std::set<View*>::iterator it = pCluster->_hiddenViews.begin(); it != pCluster->_hiddenViews.end(); ++handleIndex, ++it) {
        HandleBarView::HandleView* pHandle = pView->_handles[*it];
        pHandle->hide(false);
    }
}


HandleBarView::HandleBarView(GenericClusterViewImpl* pCluster) :
_pCluster(pCluster),
_pCurrentView(0),
_handleHeight(25),
_handleWidth(150)
{
    setName("cluster handle bar");
    setLayout(new HandleBarLayout);
    setSizeConstraint(800, _handleHeight, View::Pref);
    setStretchFactor(-1);

    _pSelection = new SelectionView;
    _pSelection->setParentView(this);
    _pSelection->resize(_handleWidth, _handleHeight);
    _pSelection->hide(false);
}


void
HandleBarView::insertView(View* pView, const std::string& label, int index)
{
//    LOG(gui, debug, "cluster " + _pView->getName() + " insert view: " + pView->getName());
    std::string handleName = "handle " + Poco::NumberFormatter::format(_handles.size());
    LabelModel* pHandleLabelModel = new LabelModel;
    pHandleLabelModel->setLabel(label);
    ListItemModel* pHandleModel = new ListItemModel;
    pHandleModel->setLabelModel(pHandleLabelModel);
    HandleView* pHandle = new HandleView(this);
    pHandle->setModel(pHandleModel);
    pHandle->setName(handleName);
    pHandle->attachController(new HandleBarController(this, pView));
    pHandle->setAcceptDrops(true);
    pHandle->show();
    _handles[pView] = pHandle;
}


void
HandleBarView::removeView(View* pView)
{
    HandleView* pHandle = _handles[pView];
    pHandle->hide();
    _handles.erase(pView);
}


void
HandleBarView::setCurrentView(View* pView)
{
    _pCluster->setCurrentView(pView);
}


const int
HandleBarView::getHandleHeight()
{
    return _handleHeight;
}


void
HandleBarView::syncViewImpl()
{
    LOG(gui, debug, "handle bar sync view impl");

    View* pHandle = _handles[_pCurrentView];
    if (pHandle) {
        _pSelection->move(pHandle->posX(), pHandle->posY());
        _pSelection->show(false);
        _pSelection->raise(false);
    }
}


void
HandleBarView::updateCurrentView(View* pView)
{
    _pCurrentView = pView;
    syncView();
}


class StackController : public Controller
{
    friend class StackView;

    StackController(StackView* pView) : _pView(pView) {}

    virtual void removedSubview(View* pView)
    {
        _pView->_views.erase(pView);
    }

    StackView*  _pView;
};


class StackLayout : public Layout
{
public:
    virtual void layoutView();
};


void
StackLayout::layoutView()
{
    LOG(gui, debug, "stack view update layout");

    StackView* pView = static_cast<StackView*>(_pView);
    for (std::set<View*>::iterator it = pView->_views.begin(); it != pView->_views.end(); ++it) {
        (*it)->resize(pView->width(), pView->height());
    }
}


StackView::StackView(View* pParent) :
View(pParent, true)
{
    setName("stack");
    setLayout(new StackLayout);
    attachController(new StackController(this));
}


void
StackView::insertView(View* pView)
{
    _views.insert(pView);
    pView->setParent(this);
    pView->show(false);
    updateLayout();
}


void
StackView::setCurrentView(View* pView)
{
    pView->raise(true);
}


class ClusterStackController : public Controller
{
    friend class GenericClusterViewImpl;

    ClusterStackController(GenericClusterViewImpl* pViewImpl) : _pViewImpl(pViewImpl) {}

    virtual void removedSubview(View* pView)
    {
        _pViewImpl->removeView(pView);
    }

    GenericClusterViewImpl*  _pViewImpl;
};


GenericClusterViewImpl::GenericClusterViewImpl(View* pView) :
PlainViewImpl(pView)
{
    _pHandleBarView = new HandleBarView(this);
    _pStackView = new StackView;
}


void
GenericClusterViewImpl::init()
{
    _pView->setName("generic cluster");
    _pView->setLayout(new VerticalLayout);
    _pHandleBarView->setParent(_pView);
    _pStackView->setParent(_pView);
    _pStackView->attachController(new ClusterStackController(this));
}


void
GenericClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
    LOG(gui, debug, "cluster " + _pView->getName() + " insert view: " + pView->getName());

    if (index < 0 || index > _visibleViews.size()) {
        return;
    }

    if (_views.find(pView->getName()) == _views.end()) {
        // cluster does not contain view

        // insert view into generic cluster view
        // TODO: should move into update layout of handle and stack
        //       then implementation specific and visible changes are done once at the end
        //       also store label and images in a std::map<View*, ...>
        _pHandleBarView->insertView(pView, label, index);
        _pStackView->insertView(pView);

        _views.insert(std::make_pair(pView->getName(), pView));
        _visibleViews.insert(_visibleViews.begin() + index, pView);
    }
    else {
        // view is already in the cluster
        if (_hiddenViews.find(pView) == _hiddenViews.end()) {
            // view is visible
            std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
            if (it != _visibleViews.begin() + index) {
                // move position of view, if not already there
                _visibleViews.erase(it);
                _visibleViews.insert(_visibleViews.begin() + index, pView);
            }
        }
        else {
            // view is hidden, make it visible
            _hiddenViews.erase(pView);
            _visibleViews.insert(_visibleViews.begin() + index, pView);
        }
    }

    // update generic cluster view layouts
    _pHandleBarView->updateLayout();

    setCurrentViewIndex(index);
}


void
GenericClusterViewImpl::removeView(View* pView)
{
    LOG(gui, debug, "cluster " + _pView->getName() + " remove view: " + pView->getName());

    if (_views.find(pView->getName()) == _views.end()) {
        // cluster does not contain view
        return;
    }
    _views.erase(pView->getName());

    std::vector<View*>::iterator it = std::find(_visibleViews.begin(), _visibleViews.end(), pView);
    if (it != _visibleViews.end()) {
        // view is visible

        // remove view from generic cluster view
        // TODO: move this into update layout of handle and stack
        _pHandleBarView->removeView(pView);

        _visibleViews.erase(it);
        if (!_visibleViews.size()) {
            _pView->hide();
        }
        else {
            // update generic cluster view layouts
            _pHandleBarView->updateLayout();

            setCurrentViewIndex(0);
        }
    }
    else {
        // view is hidden
        _hiddenViews.erase(pView);
    }
}


std::string
GenericClusterViewImpl::getConfiguration()
{
    std::string res = "tab ";
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
    if (configuration == getConfiguration()) {
        return;
    }

    // parse layout configuration string ("tab" and "col" types are valid, "tab" can also be the simple V1,V2,...,Vn form w/o prefix)
    Poco::StringTokenizer columnTokens(configuration, " ");
    if (!columnTokens.count()) {
        LOG(gui, error, "generic cluster configuration empty ");
        return;
    }
    std::string configType = *columnTokens.begin();
    bool simpleType = (columnTokens.count() == 1);
    if (!simpleType && (configType != "tab" || configType != "col")) {
        LOG(gui, error, "generic cluster cannot parse configuration of type: " + configType);
        return;
    }

    // save current view
    View* pSelectedView = getViewFromIndex(_currentViewIndex);

    // reset visible/hidden views
    _visibleViews.clear();
    for (ViewIterator it = _views.begin(); it != _views.end(); ++it) {
        _hiddenViews.insert(it->second);
    }

    for (Poco::StringTokenizer::Iterator cit = columnTokens.begin() + (simpleType ? 0 : 1); cit != columnTokens.end(); ++cit) {
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

    // update generic cluster view layouts
    _pHandleBarView->updateLayout();

    // maintain selected view, if still visible
    int currentViewIndex = getIndexFromView(pSelectedView);
    if (currentViewIndex == -1) {
        // view is not visible anymore, select first view
        // FIXME: setCurrentViewIndex(0) seems to trigger that video is not visible on first play
        setCurrentViewIndex(0);
    }
    else if (currentViewIndex != _currentViewIndex) {
        // view is still visible, but order has changed
        setCurrentViewIndex(currentViewIndex);
    }
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
    if (index < 0 || index >= _visibleViews.size()) {
        return;
    }
    _currentViewIndex = index;
    View* pCurrentView = _visibleViews[_currentViewIndex];
    LOG(gui, debug, "cluster \"" + _pView->getName() + "\" set current view index: " + Poco::NumberFormatter::format(index) + ", name: " + pCurrentView->getName());

    // generic cluster view: push view to front
    // set current view in stack
    _pStackView->setCurrentView(pCurrentView);
    // set current view in handle view bar
    _pHandleBarView->updateCurrentView(pCurrentView);
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
    // generic cluster view: hide/show handles
    if (hidden) {
        _pHandleBarView->setSizeConstraint(_pView->width(), 0, View::Pref);
//        _pHandleBarView->setStretchFactor(0.0);
    }
    else {
        _pHandleBarView->setSizeConstraint(_pView->width(), _pHandleBarView->getHandleHeight(), View::Pref);
//        _pHandleBarView->setStretchFactor(-1.0);
    }
    _pView->updateLayout();
}


const int
GenericClusterViewImpl::getHandleHeight()
{
    // generic cluster view: get handle height
    return _pHandleBarView->getHandleHeight();
}


void
GenericClusterViewImpl::changedConfiguration()
{
    IMPL_NOTIFY_CONTROLLER(ClusterController, changedConfiguration);
}


void
GenericClusterViewImpl::setCurrentView(View* pView)
{
    setCurrentViewIndex(getIndexFromView(pView));
}


} // namespace Gui
} // namespace Omm
