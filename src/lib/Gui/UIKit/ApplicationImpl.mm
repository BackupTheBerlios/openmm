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

#import <UIKit/UIKit.h>

#include "ApplicationImpl.h"
#include "UIDrag.h"
#include "Gui/Application.h"
#include "Gui/GuiLogger.h"


@interface SlideView : UIScrollView<UIScrollViewDelegate>
{
    NSInteger                       numPages;
    NSInteger                       previousPage;
    NSInteger                       currentPage;
}

- (void)initView;

@end


@implementation SlideView

- (void)addView:(Omm::Gui::View*)pSubView position:(NSInteger)index
{
    UIView* subView = static_cast<UIView*>(pSubView->getNativeView());
    [self addSubview:subView];
    CGRect frame = subView.frame;
    frame.origin.x = index * frame.size.width;
    frame.origin.y = 100 - frame.size.height;
    subView.frame = frame;
}


- (void)scrollToPage:(NSInteger)index
{
    CGRect frame = self.frame;
    frame.origin.x = frame.size.width * index;
    frame.origin.y = 0;
    [self scrollRectToVisible:frame animated:NO];
}


- (void)initView:(NSInteger)numPages
{
    numPages = Omm::Gui::ApplicationImpl::_pControlPanels.size();
    previousPage = -1;

    self.delegate = self;
    self.pagingEnabled = YES;
    self.showsHorizontalScrollIndicator = NO;
    self.showsVerticalScrollIndicator = NO;
    self.scrollsToTop = NO;
    self.contentSize = CGSizeMake((self.frame.size.width) * numPages, self.frame.size.height);
//    [self scrollToPage:currentPage];
}


- (void)scrollViewDidScroll:(UIScrollView*)scrollView
{
    // Switch the indicator when more than 50% of the previous/next page is visible
    CGFloat pageWidth = scrollView.frame.size.width;
    currentPage = floor(scrollView.contentOffset.x / pageWidth + 0.5);

    if (fabs(scrollView.contentOffset.x / pageWidth - previousPage) > 1.2 && previousPage != -1) {
        previousPage = currentPage;
    }
}

@end


@interface OmmGuiAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow*               _pWindow;
    Omm::Gui::View*         _pMainView;
    SlideView*              _pControlPanel;
    bool                    _controlBarVisible;
}

@property (nonatomic, retain) UIWindow* _pWindow;
@property (nonatomic, retain) SlideView* _pControlPanel;

@end


@implementation OmmGuiAppDelegate

@synthesize _pWindow;

-(void)applicationDidFinishLaunching:(UIApplication*)application
{
    // create main view
    _pWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _pWindow.backgroundColor = [UIColor whiteColor];
    CGRect appFrame = [[UIScreen mainScreen] applicationFrame];
    _pMainView = Omm::Gui::ApplicationImpl::_pApplication->createAndSetMainView();
    _pMainView->resize(appFrame.size.width, appFrame.size.height);
    _pMainView->move(appFrame.origin.x, appFrame.origin.y);
    Omm::Gui::UIDrag::instance()->setMainView(_pMainView);
    UIView* pMainView = static_cast<UIView*>(_pMainView->getNativeView());

    // add control panels and global gesture for making them visible
    _pControlPanel = [[SlideView alloc] init];
    [pMainView addSubview:_pControlPanel];
    _pControlPanel.frame = CGRectMake(0.0, _pMainView->height(), _pMainView->width(), 100.0);
    [_pControlPanel initView:Omm::Gui::ApplicationImpl::_pControlPanels.size()];

    _controlBarVisible = false;
    int i = 0;
    for (std::vector<Omm::Gui::View*>::iterator it = Omm::Gui::ApplicationImpl::_pControlPanels.begin(); it != Omm::Gui::ApplicationImpl::_pControlPanels.end(); ++i, ++it) {
        (*it)->resize(_pMainView->width(), (*it)->height());
        [_pControlPanel addView:(*it) position:i];
    }
    UISwipeGestureRecognizer* pSwipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeGesture:)];
    pSwipeGesture.direction = UISwipeGestureRecognizerDirectionUp;
    [pMainView addGestureRecognizer:pSwipeGesture];
    [pSwipeGesture release];

    // add main view to app window and show
    [_pWindow addSubview:pMainView];
    [_pWindow makeKeyAndVisible];
    Omm::Gui::ApplicationImpl::_pApplication->presentedMainView();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
//    Omm::LOGNS(Gui, gui, debug, "event loop exec finished.");
    Omm::Gui::ApplicationImpl::_pApplication->finishedEventLoop();
    Omm::Gui::ApplicationImpl::_pApplication->stop();
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // return from suspend
    Omm::Gui::ApplicationImpl::_pApplication->start();
}


- (void)applicationWillResignActive:(UIApplication *)application
{
    // suspend application
    Omm::Gui::ApplicationImpl::_pApplication->stop();
}


- (void)applicationDidEnterBackground:(UIApplication *)application
{

}


- (void)applicationWillEnterForeground:(UIApplication *)application
{

}


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}


- (void)handleSwipeGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    CGPoint position = [pGestureRecognizer locationInView:static_cast<UIView*>(_pMainView->getNativeView())];
//    LOGNS(Omm::Gui, gui, debug, "main view swipe gesture offset: " + Poco::NumberFormatter::format(position.y));
//    LOGNS(Omm::Gui, gui, debug, "main view height: " + Poco::NumberFormatter::format(_pMainView->height()));
//    LOGNS(Omm::Gui, gui, debug, "difference: " + Poco::NumberFormatter::format(_pMainView->height() - position.y));
    int lowerMainViewTolerance = 25;
    if (position.y < _pMainView->height() - lowerMainViewTolerance) {
        return;
    }

    [UIView beginAnimations:@"_pControlPanel" context:nil];
    [UIView setAnimationDuration:0.4];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    CGRect frame = _pControlPanel.frame;
//    frame.origin.y = _pMainView->height() - pControlBar.frame.size.height;
    frame.origin.y = _pMainView->height() - (_controlBarVisible ? 0 : 100);
    _pControlPanel.frame = frame;
    [UIView commitAnimations];
    _controlBarVisible = !_controlBarVisible;
}


-(void)dealloc
{
    [_pWindow release];
    [super dealloc];
}

@end


namespace Omm {
namespace Gui {


Application* ApplicationImpl::_pApplication = 0;
std::vector<View*> ApplicationImpl::_pControlPanels;

ApplicationImpl::ApplicationImpl(Application* pApplication)
{
//    LOG(gui, debug, "application impl ctor");

    _pApplication = pApplication;
}


ApplicationImpl::~ApplicationImpl()
{
}


void
ApplicationImpl::resize(int width, int height)
{
    _width = width;
    _height = height;
}


int
ApplicationImpl::width()
{
    return _width;
}


int
ApplicationImpl::height()
{
    return _height;
}


void
ApplicationImpl::setFullscreen(bool fullscreen)
{
    // Do nothing on UIKit, all apps are fullscreen by default
}


void
ApplicationImpl::addControlPanel(View* pView)
{
    _pControlPanels.push_back(pView);
}


void
ApplicationImpl::showControlPanels(bool show)
{
}


int
ApplicationImpl::run(int argc, char** argv)
{
    LOG(gui, debug, "event loop exec ...");
    NSAutoreleasePool* pAutoreleasePool = [[NSAutoreleasePool alloc] init];
    int ret = UIApplicationMain(argc, argv, nil, @"OmmGuiAppDelegate");
    [pAutoreleasePool release];
    return ret;
}



}  // namespace Omm
}  // namespace Gui
