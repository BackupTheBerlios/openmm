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

#include <Poco/NumberFormatter.h>

#include "NavigatorImpl.h"
#include "Gui/Navigator.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {


NavigatorViewImpl::NavigatorViewImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("navigator view impl ctor");

    UINavigationController* pNativeView = [[UINavigationController alloc] init];
    initViewImpl(pView, pNativeView);
}


NavigatorViewImpl::~NavigatorViewImpl()
{
}


void
NavigatorViewImpl::pushView(View* pView, const std::string name)
{
    Omm::Gui::Log::instance()->gui().debug("navigator view implementation push view");

    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pName = [[NSString alloc] initWithUTF8String:name.c_str()];
    pViewController.title = pName;

    [pNativeViewController pushViewController:pViewController animated:YES];
}


void
NavigatorViewImpl::popView()
{
    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());
    [pNativeViewController popViewControllerAnimated:YES];
}


void
NavigatorViewImpl::showSearchBox(bool show)
{
// TODO: implement showSearchBox for UiKit
    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());

    UISearchBar* searchBar = [[UISearchBar alloc] init];
//    UISearchBar* searchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 30.0, 10.0)];
    [searchBar sizeToFit];
    searchBar.delegate = pNativeViewController;
//    pNativeViewController.topViewController;
    UIBarButtonItem* buttonItem = [[UIBarButtonItem alloc] initWithCustomView:searchBar];
//    buttonItem.frame = CGRectMake(0.0, 0.0, 30.0, 10.0);
    pNativeViewController.topViewController.navigationItem.rightBarButtonItem = buttonItem;
//    pNativeViewController.topViewController.navigationItem.rightBarButtonItem = searchBar;

//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    pNativeViewController.navigationItem.titleView = searchBar;
//    [pNativeViewController setRightBarButtonItem:[[UISearchBar alloc] init]];
}


} // namespace Gui
} // namespace Omm
