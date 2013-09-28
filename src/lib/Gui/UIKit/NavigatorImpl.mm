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
    Omm::Gui::NavigatorViewImpl*    _pNavigatorViewImpl;
    UISearchBar*                    _pSearchBox;
    BOOL                            _visibleSearchBox;
    UIBarButtonItem*                _pRightButton;
    BOOL                            _visibleRightButton;
}

@property (nonatomic, retain) UISearchBar* _pSearchBox;
@property (nonatomic, retain) UIBarButtonItem* _pRightButton;

@end


@implementation OmmNavigationController
@synthesize _pSearchBox;
@synthesize _pRightButton;

- (void)setImpl:(Omm::Gui::NavigatorViewImpl*)pImpl
{
    _pNavigatorViewImpl = pImpl;
    self.delegate = self;

    _pSearchBox = [[UISearchBar alloc] init];
    _pSearchBox.barStyle = UIBarStyleBlack;
    _pSearchBox.showsCancelButton = YES;
    _pSearchBox.autoresizingMask = UIViewAutoresizingFlexibleWidth;
    _pSearchBox.autocapitalizationType = UITextAutocapitalizationTypeNone;
    _visibleSearchBox = NO;

    _pRightButton = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStyleBordered target:self action:@selector(rightButtonPushed)];
    _visibleRightButton = NO;
}


- (void)searchBar:(UISearchBar*)searchBar textDidChange:(NSString*)searchText
{
    _pNavigatorViewImpl->textEdited(std::string([searchText UTF8String]));
}


- (void)searchBarCancelButtonClicked:(UISearchBar*)searchBar
{
    [searchBar resignFirstResponder];
}


- (void)navigationController:(UINavigationController*)navigationController willShowViewController:(UIViewController*)viewController animated:(BOOL)animated
{
}


- (void)navigationController:(UINavigationController*)navigationController didShowViewController:(UIViewController*)viewController animated:(BOOL)animated
{
    if (_visibleSearchBox) {
        [[self.navigationBar topItem] setTitleView:_pSearchBox];
    }
    else {
        [[self.navigationBar topItem] setTitleView:nil];
    }
    if (_visibleRightButton) {
        [[self.navigationBar topItem] setRightBarButtonItem:_pRightButton animated:NO];
    }
    else {
        [[self.navigationBar topItem] setRightBarButtonItem:nil animated:NO];
    }
    _pNavigatorViewImpl->removeViewsUpto(viewController);
//    _pNavigatorViewImpl->poppedToView();
}


- (void)popView
{
    [self popViewControllerAnimated:YES];
    if ([self.viewControllers count] <= 1) {
        _pNavigatorViewImpl->poppedToRoot();
    }
//    else {
//        _pNavigatorViewImpl->poppedToView();
//    }
}


- (void)popToRootView
{
    [self popToRootViewControllerAnimated:YES];
    _pNavigatorViewImpl->poppedToRoot();
}


- (void)navigationBar:(UINavigationBar*)navigationBar didPopItem:(UINavigationItem*)item
{
    if ([self.viewControllers count] <= 1) {
        _pNavigatorViewImpl->poppedToRoot();
    }
    else {
        _pNavigatorViewImpl->poppedToView();
    }
}


- (void)showSearchBox:(BOOL)show
{
    _visibleSearchBox = show;
    if (show) {
        [[self.navigationBar topItem] setTitleView:_pSearchBox];
//        for (id item in [self.navigationBar items]) {
//            [item setTitleView:_pSearchBox animated:NO];
//        }
    }
    else {
        [[self.navigationBar topItem] setTitleView:nil];
//        for (id item in [self.navigationBar items]) {
//            [item setTitleView:nil animated:NO];
//        }
    }
}


- (void)showRightButton:(BOOL)show
{
    _visibleRightButton = show;
    if (show) {
//        [[self.navigationBar topItem] setRightBarButtonItem:_pRightButton animated:NO];
        for (id item in [self.navigationBar items]) {
            [item setRightBarButtonItem:_pRightButton animated:NO];
        }
    }
    else {
//        [[self.navigationBar topItem] setRightBarButtonItem:nil animated:NO];
        for (id item in [self.navigationBar items]) {
            [item setRightBarButtonItem:nil animated:NO];
        }
    }
}

- (void)rightButtonPushed
{
    _pNavigatorViewImpl->rightButtonPushed();
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

    if (!show) {
        return;
    }

//    UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());
//    UISearchBar* searchBar = [[UISearchBar alloc] init];
//    searchBar.barStyle = UIBarStyleBlack;
//    searchBar.showsCancelButton = YES;
//    searchBar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
////    searchBar.showsSearchResultsButton = YES;
//    searchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
////    UISearchBar* searchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 30.0, 10.0)];
//    [searchBar sizeToFit];
//    searchBar.delegate = pNativeViewController;


//    pNativeViewController.topViewController;
//    UIBarButtonItem* buttonItem = [[UIBarButtonItem alloc] initWithCustomView:searchBar];
//    buttonItem.frame = CGRectMake(0.0, 0.0, 30.0, 10.0);
    // FIXME: adding a search bar slows down scrolling
//    pNativeViewController.topViewController.navigationItem.rightBarButtonItem = buttonItem;

    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
    [pNativeViewController showSearchBox:show];

//    pNativeViewController.navigationItem.titleView = searchBar;
//    [[pNativeViewController.navigationBar topItem] setTitleView:pNativeViewController._pSearchBox];

//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    [pNativeViewController.navigationBar pushNavigationItem:searchBar];
//    pNativeViewController.navigationItem.titleView = searchBar;
//    [pNativeViewController setRightBarButtonItem:[[UISearchBar alloc] init]];
}


void
NavigatorViewImpl::clearSearchText()
{
}


void
NavigatorViewImpl::showRightButton(bool show)
{
//    http://stackoverflow.com/questions/13341562/how-to-set-button-for-navigationitem-titleview
    OmmNavigationController* pNativeViewController = static_cast<OmmNavigationController*>(getNativeViewController());
    [pNativeViewController showRightButton:show];

//    UIBarButtonItem* pRightButton = [[UIBarButtonItem alloc] initWithTitle:@"" style:UIBarButtonItemStyleBordered target:pNativeViewController action:@selector(saveFileToDocuments)];
//    [[pNativeViewController.navigationBar topItem] setRightBarButtonItem:pNativeViewController._pRightButton animated:NO];
}


void
NavigatorViewImpl::setRightButtonLabel(const std::string& label)
{
}


void
NavigatorViewImpl::setRightButtonColor(const Color& color)
{

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


void
NavigatorViewImpl::poppedToView()
{
    View* pView = _viewStack.top();
    LOG(gui, debug, "navigator popped to view: " + (pView ? pView->getName() : ""));
    IMPL_NOTIFY_CONTROLLER(NavigatorController, poppedToView, pView);
}


void
NavigatorViewImpl::rightButtonPushed()
{
    LOG(gui, debug, "navigator right button pushed");
    IMPL_NOTIFY_CONTROLLER(NavigatorController, rightButtonPushed);
}

} // namespace Gui
} // namespace Omm
