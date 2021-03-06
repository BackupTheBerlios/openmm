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

#import <UIKit/UIKit.h>

#include "ClusterImpl.h"
#include "Gui/Cluster.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ClusterViewImpl::ClusterViewImpl(View* pView)
{
    LOG(gui, debug, "cluster view impl ctor");

    UITabBarController* pNativeViewController = [[UITabBarController alloc] init];
    initViewImpl(pView, pNativeViewController);
}


ClusterViewImpl::~ClusterViewImpl()
{
}


void
ClusterViewImpl::insertView(View* pView, const std::string& label, int index)
{
    LOG(gui, debug, "cluster view implementation add view");

    _labels[pView] = label;

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pName = [[NSString alloc] initWithUTF8String:label.c_str()];
    pViewController.title = pName;

    NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
    [array insertObject:pViewController atIndex:index];
    pNativeViewController.viewControllers = array;
}


void
ClusterViewImpl::removeView(View* pView)
{
    _labels.erase(pView);

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
    [array removeObject:pViewController];
    pNativeViewController.viewControllers = array;
}


void
ClusterViewImpl::showViewAtIndex(View* pView, int index)
{
    std::map<View*,std::string>::iterator it = _labels.find(pView);
    if (it == _labels.end()) {
        // cluster does not contain view
        return;
    }
    std::string label = it->second;

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pName = [[NSString alloc] initWithUTF8String:label.c_str()];
    pViewController.title = pName;

    NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
    [array insertObject:pViewController atIndex:index];
    pNativeViewController.viewControllers = array;

    setCurrentViewIndex(index);
}


void
ClusterViewImpl::hideView(View* pView)
{
    std::map<View*,std::string>::iterator it = _labels.find(pView);
    if (it == _labels.end()) {
        // cluster does not contain view
        return;
    }

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
    [array removeObject:pViewController];
    pNativeViewController.viewControllers = array;

    setCurrentViewIndex(0);
}


std::string
ClusterViewImpl::getConfiguration()
{
    return "";
}


void
ClusterViewImpl::setConfiguration(const std::string& configuration)
{

}


int
ClusterViewImpl::getViewCount()
{
    return [static_cast<UITabBarController*>(getNativeViewController()).viewControllers count];
}


int
ClusterViewImpl::getCurrentViewIndex()
{
    return static_cast<UITabBarController*>(getNativeViewController()).selectedIndex;
}


void
ClusterViewImpl::setCurrentViewIndex(int index)
{
    static_cast<UITabBarController*>(getNativeViewController()).selectedIndex = index;
}


int
ClusterViewImpl::getIndexFromView(View* pView)
{
    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    return [array indexOfObject:pViewController];
}


View*
ClusterViewImpl::getViewFromIndex(int index)
{
    // TODO: implement UIKit implementation of getViewFromIndex()
    return 0;
}


void
ClusterViewImpl::setHandlesHidden(bool hidden)
{
    static_cast<UITabBarController*>(getNativeViewController()).tabBar.hidden = hidden;
}


const int
ClusterViewImpl::getHandleHeight()
{
    return static_cast<UITabBarController*>(getNativeViewController()).tabBar.frame.size.height;
}


}  // namespace Omm
}  // namespace Gui
