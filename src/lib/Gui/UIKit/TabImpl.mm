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

#include "TabImpl.h"
#include "Gui/Tab.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


TabViewImpl::TabViewImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("tab view impl ctor");

    UITabBarController* pNativeViewController = [[UITabBarController alloc] init];
    initViewImpl(pView, pNativeViewController);
}


TabViewImpl::~TabViewImpl()
{
}


int
TabViewImpl::addView(View* pView, const std::string& tabName, bool show)
{
    Omm::Gui::Log::instance()->gui().debug("tab view implementation add view");

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pTabName = [[NSString alloc] initWithUTF8String:tabName.c_str()];
    pViewController.title = pTabName;

    if ([pNativeViewController.viewControllers containsObject:pViewController]) {
        if (!show) {
            NSMutableArray *array = [NSMutableArray arrayWithArray:pNativeViewController.viewControllers];
            [array removeObject:pViewController];
            pNativeViewController.viewControllers = array;
//            pNativeViewController.viewControllers = [NSArray arrayWithArray:array];
            return -1;
        }
    }
    else if (show) {
        pNativeViewController.viewControllers = [pNativeViewController.viewControllers arrayByAddingObject:pViewController];
        return 1;
    }
    return 0;
}


int
TabViewImpl::getCurrentTab()
{
    return static_cast<UITabBarController*>(getNativeViewController()).selectedIndex;
}


void
TabViewImpl::setTabBarHidden(bool hidden)
{
    static_cast<UITabBarController*>(getNativeViewController()).tabBar.hidden = hidden;
}


void
TabViewImpl::setCurrentView(View* pView)
{
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    static_cast<UITabBarController*>(getNativeViewController()).selectedViewController = pViewController;
}


void
TabViewImpl::setCurrentTab(int index)
{
    static_cast<UITabBarController*>(getNativeViewController()).selectedIndex = index;
}


}  // namespace Omm
}  // namespace Gui
