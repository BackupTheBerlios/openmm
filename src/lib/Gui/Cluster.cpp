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
#include "Gui/GuiLogger.h"
#include "Gui/Cluster.h"
#include "Gui/Layout.h"
#include "Gui/HorizontalLayout.h"
#include "Gui/VerticalLayout.h"
#include "Gui/Label.h"
#include "Gui/Button.h"
#include "Gui/ListItem.h"
#include "Gui/Drag.h"


namespace Omm {
namespace Gui {


//class ClusterView : public View
//{
//    friend class ClusterViewImpl;
//    friend class ClusterStackedLayout;
//
//public:
//    ClusterView(ClusterView* pParent = 0);
//    virtual ~ClusterView();
//
//    ClusterView* createClusterWithView(View* pView, bool horizontalLayout);
//    void addView(View* pView, const std::string& name = "", bool show = true);
//    void removeView(View* pView);
//    int getTabCount();
//    int getCurrentTab();
//    const int getHandleHeight() const;
//    void setTabBarHidden(bool hidden = true);
//    void setCurrentView(View* pView);
//    void setCurrentTab(int index);
//
//private:
//    ClusterView*                        _pClusterView;
//    int                                 _handleHeight;
//    int                                 _handleWidth;
//    bool                                _handleBarHidden;
//    std::map<View*, ListItemView*>      _handles;
//    std::vector<View*>                  _views;
//    int                                 _currentView;
//};


class ClusterController : public Controller
{
public:
    ClusterController(ClusterView* pClusterView, View* pView) : _pClusterView(pClusterView), _pView(pView) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "platter drag controller drag started in view: " + _pView->getName());
        _pClusterView->removeView(_pView);
        _pClusterView->updateLayout();
        Omm::Gui::Drag* pDrag = new Omm::Gui::Drag(_pView, _pView->getModel());
        pDrag->start();
    }

    virtual void dragEntered(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "platter drag controller drag entered in view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "platter drag controller drag moved in view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dragLeft()
    {
        LOG(gui, debug, "platter drag controller drag left in view: " + _pView->getName());
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "platter drag controller dropped in view: " + _pView->getName() + ", source view: " + pDrag->getSource()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
//        _pClusterView->removeView(pDrag->getSource());
        if (_pView == _pClusterView) {
            // dropped onto a cluster view
            bool horizontal = pos.y() < _pClusterView->getHandleHeight() + _pClusterView->height() / 2;
//            ClusterView* pClusterView = _pClusterView->createClusterWithView(pDrag->getSource(), horizontal);
//            pClusterView->updateLayout();
        }
        else {
            // dropped onto a handle
            _pClusterView->addView(pDrag->getSource(), pDrag->getSource()->getName());
            _pClusterView->updateLayout();
            _pClusterView->show();
            pDrag->getSource()->show();
        }
    }

private:
//    ClusterView*    _pClusterView;
    ClusterView*    _pClusterView;
    View*           _pView;
};


class ClusterStackedLayout : public Layout
{
public:
    virtual void layoutView();
    virtual void layoutViewEquiDistant();
};


void
ClusterStackedLayout::layoutView()
{
    ClusterView* pView = static_cast<ClusterView*>(_pView);
    if (!pView->_views.size()) {
        return;
    }

    int handleHeight = (pView->_handleBarHidden ? 0 : pView->_handleHeight);
    int handleWidth = pView->width() / pView->_handles.size();

    int handleIndex = 0;
    for (std::map<View*, ListItemView*>::iterator ith = pView->_handles.begin(); ith != pView->_handles.end(); ++handleIndex, ++ith) {
        ith->second->resize(handleWidth, handleHeight);
        ith->second->move(handleIndex * handleWidth, 0);
    }

    for (ClusterView::SubviewIterator itv = pView->_views.begin(); itv != pView->_views.end(); ++itv) {
        (*itv)->resize(pView->width(), pView->height() - handleHeight);
        (*itv)->move(0, handleHeight);
    }
}


void
ClusterStackedLayout::layoutViewEquiDistant()
{
    layoutView();
}


class HandleController : public Controller
{
public:
    HandleController(ClusterView* pClusterView, View* pView) : _pClusterView(pClusterView), _pView(pView) {}

    virtual void selected()
    {
        _pClusterView->setCurrentView(_pView);
    }

private:
    ClusterView*    _pClusterView;
    View*           _pView;
};


ClusterView::ClusterView(View* pParent) :
View(pParent, true),
//_pClusterView(pParent),
_handleHeight(25),
_handleWidth(150),
_handleBarHidden(false),
_currentView(-1)
{
    setAcceptDrops(true);
    attachController(new ClusterController(this, this));
    setLayout(new ClusterStackedLayout);
}


ClusterView::~ClusterView()
{
}


void
ClusterView::addView(View* pView, const std::string& name, bool show)
{
    if (show) {
        LOG(gui, debug, "cluster add view: " + name);
        pView->setParent(this);
        pView->resize(width(), height() - _handleHeight);
        pView->move(0, _handleHeight);

        std::string handleName = "handle " + Poco::NumberFormatter::format(_views.size());
        LabelModel* pHandleLabelModel = new LabelModel;
        pHandleLabelModel->setLabel(name);
        ListItemModel* pHandleModel = new ListItemModel;
        pHandleModel->setLabelModel(pHandleLabelModel);
        ListItemView* pHandle = new ListItemView(this);
        pHandle->setModel(pHandleModel);
        pHandle->setName(handleName);
        HandleController* pHandleController = new HandleController(this, pView);
        pHandle->attachController(pHandleController);
        pHandle->setAcceptDrops(true);
        pHandle->attachController(new ClusterController(this, pView));
        pHandle->show();

        _handles[pView] = pHandle;
        _views.push_back(pView);
        _currentView = _views.size() - 1;
    }
    else {
        SubviewIterator it = std::find(_views.begin(), _views.end(), pView);
        if (it != _views.end()) {
            _views.erase(it);
            _currentView = 0;
        }
    }
}


void
ClusterView::removeView(View* pView)
{
    std::map<View*, ListItemView*>::iterator ith = _handles.find(pView);
    std::vector<View*>::iterator itv = std::find(_views.begin(), _views.end(), pView);
    if (ith != _handles.end()) {
        // TODO: maintain a pool of handles and reuse them
        _handles[pView]->hide();
        _handles.erase(ith);
        _currentView = 0;
        _views.erase(itv);
    }
}


int
ClusterView::getTabCount()
{
    return _views.size();
}


int
ClusterView::getCurrentTab()
{
    LOG(gui, debug, "cluster get current tab: " + Poco::NumberFormatter::format(_currentView));

    return _currentView;
}


const int
ClusterView::getHandleHeight() const
{
    return _handleHeight;
}


void
ClusterView::setTabBarHidden(bool hidden)
{
    _handleBarHidden = hidden;
    for (SubviewIterator it = _views.begin(); it != _views.end(); ++it) {
        (*it)->resize(width(), height() - (hidden ? 0 : _handleHeight));
        (*it)->move(0, (hidden ? 0 : _handleHeight));
    }
}


void
ClusterView::setCurrentView(View* pView)
{
    SubviewIterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        _handles[_views[_currentView]]->setBackgroundColor(Color("white"));
        _currentView = it - _views.begin();
        pView->raise();
        _handles[pView]->setBackgroundColor(Color(200, 200, 200, 255));
    }
}


void
ClusterView::setCurrentTab(int index)
{
    LOG(gui, debug, "cluster set current tab: " + Poco::NumberFormatter::format(index));

    _handles[_views[_currentView]]->setBackgroundColor(Color("white"));
    _currentView = index;
    _views[_currentView]->raise();
    _handles[_views[_currentView]]->setBackgroundColor(Color(200, 200, 200, 255));
}


void
ClusterView::syncViewImpl()
{
    LOG(gui, debug, "cluster sync view impl");

    ClusterModel* pModel = static_cast<ClusterModel*>(_pModel);
}








//ClusterView*
//ClusterView::createClusterWithView(View* pView, bool horizontalLayout)
//{
//    LOG(gui, debug, "create cluster with view: " + pView->getName());
//
//    ClusterView* pClusterView = static_cast<ClusterView*>(getParent());
//
//    ClusterView* pClusterView = pClusterView->addCluster(horizontalLayout);
//    pClusterView->addView(pView, pView->getName());
//    pClusterView->show();
//    pView->show();
////    pClusterView->updateLayout();
//    return pClusterView;
//}


//const std::string ClusterView::Flat("FlatClusterView");
//const std::string ClusterView::Tree("TreeClusterView");

//ClusterView::ClusterView(View* pParent) :
////View(pParent, true),
//SplitterView(pParent),
//_pCluster(0),
//_pFirstCluster(0),
//_pSecondCluster(0)
//{
//    setName("platter view");
//    addCluster(true);
//}
//
//
//ClusterView::~ClusterView()
//{
//    if (!_pCluster) {
//        return;
//    }
//    delete _pCluster;
//}
//
//
//void
//ClusterView::addView(View* pView, const std::string& name, bool show)
//{
//    if (!_pCluster) {
//        return;
//    }
//    _pCluster->addView(pView, name, show);
//}
//
//
//void
//ClusterView::removeView(View* pView)
//{
//    if (!_pCluster) {
//        return;
//    }
//    _pCluster->removeView(pView);
//}
//
//
//int
//ClusterView::getTabCount()
//{
//    if (!_pCluster) {
//        return 0;
//    }
//    return _pCluster->getTabCount();
//}
//
//
//int
//ClusterView::getCurrentTab()
//{
//    if (!_pCluster) {
//        return -1;
//    }
//    return _pCluster->getCurrentTab();
//}
//
//
//void
//ClusterView::setTabBarHidden(bool hidden)
//{
//    if (!_pCluster) {
//        return;
//    }
//    return _pCluster->setTabBarHidden(hidden);
//}
//
//
//void
//ClusterView::setCurrentView(View* pView)
//{
//    if (!_pCluster) {
//        return;
//    }
//    _pCluster->setCurrentView(pView);
//}
//
//
//void
//ClusterView::setCurrentTab(int index)
//{
//    if (!_pCluster) {
//        return;
//    }
//    _pCluster->setCurrentTab(index);
//}
//
//
//ClusterView*
//ClusterView::addCluster(bool horizontalLayout)
//{
//    LOG(gui, debug, getName() + " add cluster");
//
//    ClusterView* pRes = _pCluster;
//    if (!_pCluster) {
////        _pCluster = new ClusterView(this);
//        _pCluster = new ClusterView;
//        _pCluster->setName("first cluster");
//        addSubview(_pCluster);
//    }
//    else if (!_pFirstCluster) {
////        _pFirstCluster = new ClusterView(this, false);
////        _pFirstCluster = new ClusterView(this);
//        _pFirstCluster = new ClusterView;
//        addSubview(_pFirstCluster);
//        _pFirstCluster->removeSubview(_pFirstCluster->_pCluster);
//        _pFirstCluster->_pCluster->hide();
//        _pFirstCluster->setName("first platter");
//        _pFirstCluster->addSubview(_pCluster);
//        _pFirstCluster->_pCluster = _pCluster;
//        removeSubview(_pCluster);
////        _pCluster->hide();
//        _pCluster = 0;
////        _pSecondCluster = new ClusterView(this);
//        _pSecondCluster = new ClusterView;
//        addSubview(_pSecondCluster);
//        _pSecondCluster->setName("second platter");
//        _pSecondCluster->_pCluster->setName("second cluster");
//        pRes = _pSecondCluster->_pCluster;
//        pRes->show();
//        _pFirstCluster->show();
//        _pFirstCluster->_pCluster->updateLayout();
//        _pSecondCluster->show();
//        _pSecondCluster->_pCluster->show();
//        _pSecondCluster->_pCluster->updateLayout();
//    }
//    else {
//        return 0;
//    }
////    if (getLayout()) {
////
////    }
//    if (horizontalLayout) {
//        setOrientation(Horizontal);
////        setLayout(new HorizontalLayout);
//    }
//    else {
//        setOrientation(Vertical);
////        setLayout(new VerticalLayout);
//    }
//    return pRes;
//}


} // namespace Gui
} // namespace Omm
