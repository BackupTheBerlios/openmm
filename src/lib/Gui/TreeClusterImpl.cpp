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
#include "Gui/Splitter.h"
#include "TreeClusterImpl.h"


namespace Omm {
namespace Gui {


class TreeClusterController : public Controller
{
public:
    TreeClusterController(TreeClusterViewImpl* pClusterViewImpl, View* pView) : _pClusterViewImpl(pClusterViewImpl), _pView(pView) {}

    virtual void dragStarted()
    {
        LOG(gui, debug, "tree cluster drag controller drag started in source view: " + _pView->getName());
    }

    virtual void dragEntered(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "tree cluster drag controller drag entered in target view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dragMoved(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "tree cluster drag controller drag moved in target view: " + _pView->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    }

    virtual void dragLeft()
    {
        LOG(gui, debug, "tree cluster drag controller drag left in target view: " + _pView->getName());
    }

    virtual void dropped(const Position& pos, Drag* pDrag)
    {
        LOG(gui, debug, "tree cluster drag controller dropped in target view: " + _pClusterViewImpl->_pView->getName() + ", source view: " + pDrag->getSource()->getName()
                + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
        bool horizontal = pos.y() < _pClusterViewImpl->_pCluster->getHandleHeight() + _pClusterViewImpl->_pCluster->height() / 2;

        _pClusterViewImpl->split(horizontal);
        _pClusterViewImpl->_pSecondCluster->insertView(pDrag->getSource(), pDrag->getSource()->getName());
        _pClusterViewImpl->moveFirstCluster();
//        _pClusterViewImpl->updateLayout();
        _pClusterViewImpl->updateClusterController();
    }

    TreeClusterViewImpl*    _pClusterViewImpl;
    View*                   _pView;
};


int TreeClusterViewImpl::_genericClusterNumber = 0;
int TreeClusterViewImpl::_treeClusterNumber = 0;

TreeClusterViewImpl::TreeClusterViewImpl(View* pView, bool createInitialCluster) :
//PlainViewImpl(pView),
SplitterViewImpl(pView, View::Horizontal),
_createInitialCluster(createInitialCluster),
_pCluster(0),
_pFirstCluster(0),
_pSecondCluster(0)
{
}


void
TreeClusterViewImpl::init()
{
    ClusterView* pView = static_cast<ClusterView*>(_pView);
    pView->setName("tree cluster " + Poco::NumberFormatter::format(_treeClusterNumber));
    _treeClusterNumber++;
//    pView->setLayout(new HorizontalLayout);

    if (_createInitialCluster) {
        _pCluster = new ClusterView(0, ClusterView::Generic);
        _pCluster->setParent(pView);
        _pCluster->setAcceptDrops(true);
        _pCluster->attachController(new TreeClusterController(this, pView));
        _pCluster->setName("generic cluster " + Poco::NumberFormatter::format(_genericClusterNumber));
        _genericClusterNumber++;
    }

}


void
TreeClusterViewImpl::insertView(View* pView, const std::string& name, int index)
{
    LOG(gui, debug, "tree cluster insert view: " + name);
    if (!_pCluster) {
        return;
    }
    _pCluster->insertView(pView, name, index);
}


void
TreeClusterViewImpl::removeView(View* pView)
{

}


int
TreeClusterViewImpl::getViewCount()
{
}


int
TreeClusterViewImpl::getCurrentViewIndex()
{
//    LOG(gui, debug, "cluster get current tab: " + Poco::NumberFormatter::format(_currentView));

}


void
TreeClusterViewImpl::setCurrentViewIndex(int index)
{
    LOG(gui, debug, "cluster set current tab: " + Poco::NumberFormatter::format(index));

    if (index == -1) {
        return;
    }
}


int
TreeClusterViewImpl::getIndexFromView(View* pView)
{
}


void
TreeClusterViewImpl::setHandlesHidden(bool hidden)
{
}


const int
TreeClusterViewImpl::getHandleHeight()
{
}


void
TreeClusterViewImpl::split(bool horizontal)
{
    ClusterView* pView = static_cast<ClusterView*>(_pView);

//    _pCluster->hide();

    if (horizontal) {
        setOrientation(View::Horizontal);
    }
    else {
        setOrientation(View::Vertical);
    }

//    _pFirstCluster = new ClusterView(0, ClusterView::Tree, false);
    _pFirstCluster = new ClusterView(0, ClusterView::Tree);
    _pFirstCluster->setParent(_pView);
    _pFirstCluster->show();

    _pSecondCluster = new ClusterView(0, ClusterView::Tree);
    _pSecondCluster->setParent(_pView);
    _pSecondCluster->show();
}


void
TreeClusterViewImpl::moveFirstCluster()
{
    TreeClusterViewImpl* pFirstClusterViewImpl = static_cast<TreeClusterViewImpl*>(_pFirstCluster->getViewImpl());
    _pCluster->setParent(_pFirstCluster);
//    _pCluster->updateLayout();
    pFirstClusterViewImpl->_pCluster = _pCluster;
    _pCluster = 0;
}


void
TreeClusterViewImpl::updateClusterController()
{
    TreeClusterViewImpl* pFirstClusterViewImpl = static_cast<TreeClusterViewImpl*>(_pFirstCluster->getViewImpl());
    for (View::ControllerIterator it = pFirstClusterViewImpl->_pCluster->beginController(); it != pFirstClusterViewImpl->_pCluster->endController(); ++it) {
        TreeClusterController* pClusterController = dynamic_cast<TreeClusterController*>(*it);
        if (pClusterController) {
            pClusterController->_pClusterViewImpl = pFirstClusterViewImpl;
            pClusterController->_pView = _pFirstCluster;
        }
    }
}


void
TreeClusterViewImpl::updateLayout()
{
    _pView->updateLayout();

    _pFirstCluster->show();
    _pFirstCluster->updateLayout();
    static_cast<TreeClusterViewImpl*>(_pFirstCluster->getViewImpl())->_pCluster->show();
    static_cast<TreeClusterViewImpl*>(_pFirstCluster->getViewImpl())->_pCluster->updateLayout();

    _pSecondCluster->show();
    _pSecondCluster->updateLayout();
    static_cast<TreeClusterViewImpl*>(_pSecondCluster->getViewImpl())->_pCluster->show();
    static_cast<TreeClusterViewImpl*>(_pSecondCluster->getViewImpl())->_pCluster->updateLayout();
}


} // namespace Gui
} // namespace Omm
