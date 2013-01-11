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
}
@end


@implementation SlideView

- (void)addView:(Omm::Gui::View*)pSubView position:(NSInteger)index
{
    UIView* subView = static_cast<UIView*>(pSubView->getNativeView());
    CGRect frame = subView.frame;
    frame.origin.x = index * self.frame.size.width;
    frame.origin.y = 0;
    frame.size.width = self.frame.size.width;
    frame.size.height = self.frame.size.height;
    subView.frame = frame;
    [self addSubview:subView];
}


- (void)scrollToPage:(NSInteger)index animated:(BOOL)animated
{
    CGRect frame = self.frame;
    frame.origin.x = index * frame.size.width;
    frame.origin.y = 0;
    [self scrollRectToVisible:frame animated:animated];
}


- (void)initViewWithNumPages:(NSInteger)numPages
{
    self.pagingEnabled = YES;
    self.showsHorizontalScrollIndicator = NO;
    self.showsVerticalScrollIndicator = NO;
    self.scrollsToTop = NO;
    self.contentSize = CGSizeMake(numPages * (self.frame.size.width), self.frame.size.height);
    self.backgroundColor = [UIColor whiteColor];
}

@end


@interface ControlPanel : UIView<UIScrollViewDelegate>
{
    NSInteger                       _numPages;
    NSInteger                       _pBorderHeight;
    SlideView*                      _pSlideView;
    UIPageControl*                  _pPager;
    UIButton*                       _leftButton;
    UIButton*                       _rightButton;
    NSInteger                       _pagerHeight;
}
@end


@implementation ControlPanel

- (void)initViewWithNumPages:(NSInteger)numPages
{
    _pBorderHeight = 1;
    UIView* pBorder = [[UIView alloc] initWithFrame:CGRectMake(0.0, 0.0, self.frame.size.width, _pBorderHeight)];
    pBorder.backgroundColor = [UIColor grayColor];
    [self addSubview:pBorder];

    _pagerHeight = 20;
    NSInteger buttonWidth = 40;

    _pPager = [[UIPageControl alloc] initWithFrame:CGRectMake(buttonWidth, self.frame.size.height - _pagerHeight, self.frame.size.width - 2 * buttonWidth, _pagerHeight)];
    _pPager.currentPage = 0;
    _pPager.numberOfPages = numPages;
    _pPager.backgroundColor = [UIColor blackColor];
    [self addSubview:_pPager];
    [_pPager addTarget:self action:@selector(handlePagerEvent) forControlEvents:UIControlEventValueChanged];

    UIButton* pLeftButton = [[UIButton alloc] initWithFrame:CGRectMake(0.0, self.frame.size.height - _pagerHeight, buttonWidth, _pagerHeight)];
    pLeftButton.backgroundColor = [UIColor darkGrayColor];
    [self addSubview:pLeftButton];
    [pLeftButton addTarget:self action:@selector(buttonPushed) forControlEvents:UIControlEventTouchUpInside];

    UIButton* pRightButton = [[UIButton alloc] initWithFrame:CGRectMake(self.frame.size.width - buttonWidth, self.frame.size.height - _pagerHeight, buttonWidth, _pagerHeight)];
    pRightButton.backgroundColor = [UIColor darkGrayColor];
    [self addSubview:pRightButton];
    [pRightButton addTarget:self action:@selector(buttonPushed) forControlEvents:UIControlEventTouchUpInside];

    _pSlideView = [[SlideView alloc] initWithFrame:CGRectMake(0.0, _pBorderHeight, self.frame.size.width, self.frame.size.height - _pagerHeight - _pBorderHeight)];
    _pSlideView.delegate = self;
    [_pSlideView initViewWithNumPages:numPages];
    [self addSubview:_pSlideView];
}


- (void)addView:(Omm::Gui::View*)pSubView position:(NSInteger)index
{
    [_pSlideView addView:pSubView position:index];
}


- (void)scrollViewDidEndDecelerating:(UIScrollView*)scrollView
{
    _pPager.currentPage = floor(scrollView.contentOffset.x / scrollView.frame.size.width);
}


- (void)handlePagerEvent
{
    [_pSlideView scrollToPage:_pPager.currentPage animated:YES];
}


- (void)buttonPushed
{
    [UIView beginAnimations:@"self" context:nil];
    [UIView setAnimationDuration:0.4];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    CGRect frame = self.frame;
    frame.origin.y = Omm::Gui::ApplicationImpl::_pApplication->getMainView()->height();
    self.frame = frame;
    [UIView commitAnimations];
}

@end


@interface OmmGuiAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow*               _pWindow;
    Omm::Gui::View*         _pMainView;
    SlideView*              _pControlPanel;
    bool                    _controlPanelVisible;
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

    // add control panel
    _pControlPanel = [[ControlPanel alloc] initWithFrame:CGRectMake(0.0, _pMainView->height(), _pMainView->width(), Omm::Gui::ApplicationImpl::_controlPanelHeight)];
    [_pControlPanel initViewWithNumPages:Omm::Gui::ApplicationImpl::_controlPanels.size()];
    [pMainView addSubview:_pControlPanel];
    _controlPanelVisible = false;
    int i = 0;
    for (std::vector<Omm::Gui::View*>::iterator it = Omm::Gui::ApplicationImpl::_controlPanels.begin(); it != Omm::Gui::ApplicationImpl::_controlPanels.end(); ++i, ++it) {
        [_pControlPanel addView:(*it) position:i];
    }

    // add gestures for showing/hiding control panel
    UISwipeGestureRecognizer* pGlobalSwipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleGlobalSwipeGesture:)];
    pGlobalSwipeGesture.direction = UISwipeGestureRecognizerDirectionUp;
    [pMainView addGestureRecognizer:pGlobalSwipeGesture];
    [pGlobalSwipeGesture release];
    UISwipeGestureRecognizer* pPanelSwipeGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanelSwipeGesture:)];
    pPanelSwipeGesture.direction = UISwipeGestureRecognizerDirectionDown;
    [_pControlPanel addGestureRecognizer:pPanelSwipeGesture];
    [pPanelSwipeGesture release];

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


- (void)handleGlobalSwipeGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    CGPoint position = [pGestureRecognizer locationInView:static_cast<UIView*>(_pMainView->getNativeView())];
//    LOGNS(Omm::Gui, gui, debug, "main view swipe gesture offset: " + Poco::NumberFormatter::format(position.y));
//    LOGNS(Omm::Gui, gui, debug, "main view height: " + Poco::NumberFormatter::format(_pMainView->height()));
//    LOGNS(Omm::Gui, gui, debug, "difference: " + Poco::NumberFormatter::format(_pMainView->height() - position.y));
    if (position.y < _pMainView->height() - Omm::Gui::ApplicationImpl::_swipeUpStartMargin) {
        return;
    }

    [UIView beginAnimations:@"_pControlPanel" context:nil];
    [UIView setAnimationDuration:0.4];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    CGRect frame = _pControlPanel.frame;
    frame.origin.y = _pMainView->height() - (_controlPanelVisible ? 0 : Omm::Gui::ApplicationImpl::_controlPanelHeight);
    _pControlPanel.frame = frame;
    [UIView commitAnimations];
    _controlPanelVisible = !_controlPanelVisible;
}


- (void)handlePanelSwipeGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    [UIView beginAnimations:@"_pControlPanel" context:nil];
    [UIView setAnimationDuration:0.4];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    CGRect frame = _pControlPanel.frame;
    frame.origin.y = _pMainView->height();
    _pControlPanel.frame = frame;
    [UIView commitAnimations];
    _controlPanelVisible = !_controlPanelVisible;
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
std::vector<View*> ApplicationImpl::_controlPanels;
int ApplicationImpl::_controlPanelHeight = 100;
int ApplicationImpl::_swipeUpStartMargin = 50;

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
    _controlPanels.push_back(pView);
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
