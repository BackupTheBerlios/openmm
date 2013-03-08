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


@interface OmmNavigationController : UINavigationController<UISearchBarDelegate, UINavigationControllerDelegate, UINavigationBarDelegate>
{
    Omm::Gui::NavigatorViewImpl* _pNavigatorViewImpl;
}

@end


@implementation OmmNavigationController

- (void)setImpl:(Omm::Gui::NavigatorViewImpl*)pImpl
{
    _pNavigatorViewImpl = pImpl;
    self.delegate = self;
}


- (void)searchBar:(UISearchBar*)searchBar textDidChange:(NSString*)searchText
{
    _pNavigatorViewImpl->textEdited(std::string([searchText UTF8String]));
}


- (void)searchBarCancelButtonClicked:(UISearchBar*)searchBar
{
    [searchBar resignFirstResponder];
}


- (void)navigationController:(UINavigationController*)navigationController didShowViewController:(UIViewController*)viewController animated:(BOOL)animated
{
    _pNavigatorViewImpl->removeViewsUpto(viewController);
}


- (void)popView
{
    [self popViewControllerAnimated:YES];
    if ([self.viewControllers count] <= 1) {
        _pNavigatorViewImpl->poppedToRoot();
    }
}


- (void)popToRootView
{
    [self popToRootViewControllerAnimated:YES];
    _pNavigatorViewImpl->poppedToRoot();
}


- (void)navigationBar:(UINavigationBar *)navigationBar didPopItem:(UINavigationItem*)item
{
    if ([self.viewControllers count] <= 1) {
        _pNavigatorViewImpl->poppedToRoot();
    }
}


@end


namespace Omm {
namespace Gui {


NavigatorViewImpl::NavigatorViewImpl(View* pView)
{
    LOG(gui, debug, "navigator view impl ctor");

    OmmNavigationController* pNativeView = [[OmmNavigationController alloc] init];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
    pNativeView.navigationBar.barStyle = UIBarStyleBlack;
}


NavigatorViewImpl::~NavigatorViewImpl()
{
}


void
NavigatorViewImpl::pushView(View* pView, const std::string label)
{
    LOG(gui, debug, "navigator view implementation push view");

    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());
    UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
    NSString* pName = [[NSString alloc] initWithUTF8String:label.c_str()];
    pViewController.title = pName;

    [pNativeViewController pushViewController:pViewController animated:YES];
    _viewStack.push(pView);
}


void
NavigatorViewImpl::popView(bool keepRootView)
{
    // UIKit can't pop the root controller, so we have no choice here.
    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
    [pNativeViewController performSelectorOnMainThread:@selector(popView) withObject:nil waitUntilDone:YES];
}


void
NavigatorViewImpl::popToRootView()
{
    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
    [pNativeViewController performSelectorOnMainThread:@selector(popToRootView) withObject:nil waitUntilDone:YES];
}


View*
NavigatorViewImpl::getVisibleView()
{
//    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
//    UIViewController* topViewController = pNativeViewController.topViewController;
    return _viewStack.top();
}


int
NavigatorViewImpl::viewCount()
{
    return _viewStack.size();
}


void
NavigatorViewImpl::showNavigatorBar(bool show)
{
    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
    pNativeViewController.navigationBarHidden = !show;
}


void
NavigatorViewImpl::showSearchBox(bool show)
{
// TODO: maybe better use setToolbarItems:animated: for the UIViewController of the container view that is pushed

    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());

    UISearchBar* searchBar = [[UISearchBar alloc] init];
    searchBar.barStyle = UIBarStyleBlack;
    searchBar.showsCancelButton = YES;
    searchBar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
//    searchBar.showsSearchResultsButton = YES;
    searchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
//    UISearchBar* searchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 30.0, 10.0)];
    [searchBar sizeToFit];
    searchBar.delegate = pNativeViewController;

//    pNativeViewController.topViewController;
//    UIBarButtonItem* buttonItem = [[UIBarButtonItem alloc] initWithCustomView:searchBar];
//    buttonItem.frame = CGRectMake(0.0, 0.0, 30.0, 10.0);
    // FIXME: adding a search bar slows down scrolling
//    pNativeViewController.topViewController.navigationItem.rightBarButtonItem = buttonItem;


//    pNativeViewController.navigationItem.titleView = searchBar;
    [[pNativeViewController.navigationBar topItem] setTitleView:searchBar];

//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    pNativeViewController.navigationItem.titleView = searchBar;
//    [pNativeViewController setRightBarButtonItem:[[UISearchBar alloc] init]];
}


void
NavigatorViewImpl::textEdited(const std::string& searchText)
{
    LOG(gui, debug, "search text changed: " + searchText);
    IMPL_NOTIFY_CONTROLLER(NavigatorController, changedSearchText, searchText);
}


void
NavigatorViewImpl::removeViewsUpto(void* pViewController)
{
    while (!_viewStack.empty() && pViewController != _viewStack.top()->getViewImpl()->getNativeViewController()) {
        _viewStack.pop();
        LOG(gui, debug, "navigator impl view stack pop view, stack size: " + Poco::NumberFormatter::format(_viewStack.size()));
    }
}


void
NavigatorViewImpl::poppedToRoot()
{
    LOG(gui, debug, "navigator popped to root");
    IMPL_NOTIFY_CONTROLLER(NavigatorController, poppedToRoot);
}


} // namespace Gui
} // namespace Omm
