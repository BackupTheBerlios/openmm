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
#include <Poco/Environment.h>

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
#include "ClusterImpl.h"
#include "GenericClusterImpl.h"
#include "ColumnClusterImpl.h"
#include "AbstractClusterImpl.h"


namespace Omm {
namespace Gui {


const std::string ClusterView::Native("Native");
const std::string ClusterView::Generic("Generic");
const std::string ClusterView::Column("Column");

ClusterView::ClusterView(View* pParent, const std::string& type) :
View(pParent, false)
{
#ifdef __IPHONE__
    std::string clusterViewImpl = "Native";
#else
    std::string clusterViewImpl = (type == "" ? clusterViewImpl = Poco::Environment::get("OMMGUI_CLUSTER", Column) : type);
#endif
    
    if (clusterViewImpl == Native) {
        _pClusterImpl = new ClusterViewImpl(this);
        _pImpl = reinterpret_cast<ClusterViewImpl*>(_pClusterImpl);
    }
    else if (clusterViewImpl == Generic) {
        _pClusterImpl = new GenericClusterViewImpl(this);
        _pImpl = reinterpret_cast<GenericClusterViewImpl*>(_pClusterImpl);
    }
    else {
        _pClusterImpl = new ColumnClusterViewImpl(this);
        _pImpl = reinterpret_cast<ColumnClusterViewImpl*>(_pClusterImpl);
    }
    _pImpl->init();
}


ClusterView::~ClusterView()
{
}


void
ClusterView::insertView(View* pView, const std::string& label, int index)
{
    // NOTE: this is somewhat crude, a dynamic_cast wouldn't work with GenericClusterViewImpl
    // or ColumnClusterViewImpl: there's no direct inheritance, but the same interface.
//    LOG(gui, debug, "cluster view insert view: " + pView->getName());
//    static_cast<ClusterViewImpl*>(_pImpl)->insertView(pView, label, index);
    _pClusterImpl->insertView(pView, label, index);
    NOTIFY_CONTROLLER(ClusterController, insertedView, pView);
}


std::string
ClusterView::getConfiguration()
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getConfiguration();
    return _pClusterImpl->getConfiguration();
}


void
ClusterView::setConfiguration(const std::string& configuration)
{
//    static_cast<ClusterViewImpl*>(_pImpl)->setConfiguration(configuration);
    _pClusterImpl->setConfiguration(configuration);
}


int
ClusterView::getViewCount()
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getViewCount();
    return _pClusterImpl->getViewCount();
}


int
ClusterView::getCurrentViewIndex()
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getCurrentViewIndex();
    return _pClusterImpl->getCurrentViewIndex();
}


void
ClusterView::setCurrentViewIndex(int index)
{
//    static_cast<ClusterViewImpl*>(_pImpl)->setCurrentViewIndex(index);
    _pClusterImpl->setCurrentViewIndex(index);
}


int
ClusterView::getIndexFromView(View* pView)
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getIndexFromView(pView);
    return _pClusterImpl->getIndexFromView(pView);
}


View*
ClusterView::getViewFromIndex(int index)
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getViewFromIndex(index);
    return _pClusterImpl->getViewFromIndex(index);
}


void
ClusterView::setHandlesHidden(bool hidden)
{
//    static_cast<ClusterViewImpl*>(_pImpl)->setHandlesHidden(hidden);
    _pClusterImpl->setHandlesHidden(hidden);
}


const int
ClusterView::getHandleHeight()
{
//    return static_cast<ClusterViewImpl*>(_pImpl)->getHandleHeight();
    return _pClusterImpl->getHandleHeight();
}


} // namespace Gui
} // namespace Omm
