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

#include <QtGui>
#include <qt4/QtGui/qtabwidget.h>

#include "ClusterImpl.h"
#include "Gui/Cluster.h"
#include "Gui/GuiLogger.h"
#include "Log.h"

namespace Omm {
namespace Gui {


class QtClusterWidget : public QTabWidget
{
    friend class ClusterViewImpl;

    void setHidden(bool hidden)
    {
        tabBar()->setHidden(hidden);
    }

    int tabHeight()
    {
        return tabBar()->height();
    }
};


ClusterViewImpl::ClusterViewImpl(View* pView)
{
//    LOG(gui, debug, "QTab widget implementation ctor");

    QtClusterWidget* pNativeView = new QtClusterWidget;

    initViewImpl(pView, pNativeView);
}


ClusterViewImpl::~ClusterViewImpl()
{
}


void
ClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
//    LOG(gui, debug, "QTab widget implementation insert widget");
    static_cast<QtClusterWidget*>(_pNativeView)->insertTab(index, static_cast<QWidget*>(pView->getNativeView()), label.c_str());
    _views.insert(_views.begin() + index, pView);
}


void
ClusterViewImpl::removeView(View* pView)
{
    QtClusterWidget* pClusterWidget = static_cast<QtClusterWidget*>(_pNativeView);
    pClusterWidget->removeTab(pClusterWidget->indexOf(static_cast<QWidget*>(pView->getNativeView())));
    std::vector<View*>::iterator it = std::find(_views.begin(), _views.end(), pView);
    _views.erase(it);
}


std::string
ClusterViewImpl::getConfiguration()
{

}


void
ClusterViewImpl::setConfiguration(const std::string& configuration)
{

}


int
ClusterViewImpl::getViewCount()
{
    return static_cast<QtClusterWidget*>(_pNativeView)->count();
}


int
ClusterViewImpl::getCurrentViewIndex()
{
    static_cast<QtClusterWidget*>(_pNativeView)->currentIndex();
}


void
ClusterViewImpl::setCurrentViewIndex(int index)
{
    static_cast<QtClusterWidget*>(_pNativeView)->setCurrentIndex(index);
}


int
ClusterViewImpl::getIndexFromView(View* pView)
{
    return static_cast<QtClusterWidget*>(_pNativeView)->indexOf(static_cast<QWidget*>(pView->getNativeView()));
}


View*
ClusterViewImpl::getViewFromIndex(int index)
{
    return _views[index];
}


void
ClusterViewImpl::setHandlesHidden(bool hidden)
{
    static_cast<QtClusterWidget*>(_pNativeView)->setHidden(hidden);
}


const int
ClusterViewImpl::getHandleHeight()
{
    return static_cast<QtClusterWidget*>(_pNativeView)->tabHeight();
}


}  // namespace Omm
}  // namespace Gui
