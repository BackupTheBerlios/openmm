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

//    OmmGuiSlider* pNativeView = [[OmmGuiSlider alloc] initWithFrame:CGRectMake(0.0, 50.0, 150.0, 50.0)];
//    UITabBarController* pNativeView = [[UITabBarController alloc] initWithFrame:CGRectMake(0.0, 0.0, 300.0, 400.0)];
    UITabBarController* pNativeViewController = [[UITabBarController alloc] init];
//    [pNativeView setImpl:this];

    init(pView, pNativeViewController);
}


TabViewImpl::~TabViewImpl()
{
}


void
TabViewImpl::addView(View* pView, const std::string& tabName)
{
    Omm::Gui::Log::instance()->gui().debug("tab view implementation add view");

    UITabBarController* pNativeViewController = static_cast<UITabBarController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pTabName = [[NSString alloc] initWithUTF8String:tabName.c_str()];
    pViewController.title = pTabName;

    pNativeViewController.viewControllers = [pNativeViewController.viewControllers arrayByAddingObject:pViewController];
}


}  // namespace Omm
}  // namespace Gui
