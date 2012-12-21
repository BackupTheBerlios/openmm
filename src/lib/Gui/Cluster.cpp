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
#include "ClusterImpl.h"
#include "GenericClusterImpl.h"
#include "ColumnClusterImpl.h"
#include "TreeClusterImpl.h"


namespace Omm {
namespace Gui {


const std::string ClusterView::Native("NativeClusterView");
const std::string ClusterView::Generic("GenericClusterView");
const std::string ClusterView::Column("ColumnClusterView");
const std::string ClusterView::Tree("TreeClusterView");

ClusterView::ClusterView(View* pParent, const std::string& type, bool createInitialCluster) :
View(pParent, false)
{
    if (type == Native) {
        _pImpl = new ClusterViewImpl(this);
    }
    else if (type == Generic) {
        _pImpl = new GenericClusterViewImpl(this);
    }
    else if (type == Column) {
        _pImpl = new ColumnClusterViewImpl(this);
    }
    else if (type == Tree) {
        _pImpl = new TreeClusterViewImpl(this, createInitialCluster);
    }
    _pImpl->init();
}


ClusterView::~ClusterView()
{
}


void
ClusterView::insertView(View* pView, const std::string& name, int index)
{
    // NOTE: this is somewhat crude, a dynamic_cast wouldn't work with GenericClusterViewImpl
    // or TreeClusterViewImpl: there's no direct inheritance, but the same interface.
    static_cast<ClusterViewImpl*>(_pImpl)->insertView(pView, name, index);
}


int
ClusterView::getViewCount()
{
    return static_cast<ClusterViewImpl*>(_pImpl)->getViewCount();
}


int
ClusterView::getCurrentViewIndex()
{
    return static_cast<ClusterViewImpl*>(_pImpl)->getCurrentViewIndex();
}


void
ClusterView::setCurrentViewIndex(int index)
{
    static_cast<ClusterViewImpl*>(_pImpl)->setCurrentViewIndex(index);
}


int
ClusterView::getIndexFromView(View* pView)
{
    return static_cast<ClusterViewImpl*>(_pImpl)->getIndexFromView(pView);
}


void
ClusterView::setHandlesHidden(bool hidden)
{
    static_cast<ClusterViewImpl*>(_pImpl)->setHandlesHidden(hidden);
}


const int
ClusterView::getHandleHeight()
{
    return static_cast<ClusterViewImpl*>(_pImpl)->getHandleHeight();
}


void
ClusterView::removedSubview(View* pView)
{
    static_cast<ClusterViewImpl*>(_pImpl)->removeView(pView);
}


} // namespace Gui
} // namespace Omm
