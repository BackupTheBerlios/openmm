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


@interface OmmGuiAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow*           _pWindow;
    Omm::Gui::View*     _pMainView;
    bool                _toolBarVisible;
}

@property (nonatomic, retain) UIWindow* _pWindow;

@end


@implementation OmmGuiAppDelegate

@synthesize _pWindow;

-(void)applicationDidFinishLaunching:(UIApplication*)application
{
    _pWindow = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    _pWindow.backgroundColor = [UIColor whiteColor];
    CGRect appFrame = [[UIScreen mainScreen] applicationFrame];
    _pMainView = Omm::Gui::ApplicationImpl::_pApplication->createAndSetMainView();
    _pMainView->resize(appFrame.size.width, appFrame.size.height);
    _pMainView->move(appFrame.origin.x, appFrame.origin.y);
    Omm::Gui::UIDrag::instance()->setMainView(_pMainView);
    Omm::Gui::ApplicationImpl::_toolBarIndex = Omm::Gui::ApplicationImpl::_pToolBar.size();
    for (std::vector<Omm::Gui::View*>::iterator it = Omm::Gui::ApplicationImpl::_pToolBar.begin(); it != Omm::Gui::ApplicationImpl::_pToolBar.end(); ++it) {
        (*it)->setParent(_pMainView);
        Omm::Gui::ApplicationImpl::_toolBarHeight = _pMainView->height() / 5;
        (*it)->resize(_pMainView->width(), Omm::Gui::ApplicationImpl::_toolBarHeight);
        (*it)->move(0, _pMainView->height());
//        _toolBarVisible = false;

        UIView* pMainView = static_cast<UIView*>(_pMainView->getNativeView());
        UIButton* pToolBarButton = [UIButton buttonWithType:UIButtonTypeInfoLight];
        [pMainView addSubview:pToolBarButton];
        CGRect buttonFrame = pToolBarButton.frame;
        buttonFrame.origin.x = _pMainView->width() - buttonFrame.size.width;
        buttonFrame.origin.y = _pMainView->height() - buttonFrame.size.height;
        pToolBarButton.frame = buttonFrame;
        [pToolBarButton addTarget:self action:@selector(handleToolBarButtonPressed) forControlEvents:UIControlEventTouchDown];
    }
    [_pWindow addSubview:static_cast<UIView*>(_pMainView->getNativeView())];
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


- (void)handleToolBarButtonPressed
{
    int countToolbars = Omm::Gui::ApplicationImpl::_pToolBar.size();
    int index = Omm::Gui::ApplicationImpl::_toolBarIndex;
    int nextIndex = (index + 1) % (countToolbars + 1);
    int offset = Omm::Gui::ApplicationImpl::_toolBarHeight;
    if (index != countToolbars) {
        UIView* pToolBar = static_cast<UIView*>(Omm::Gui::ApplicationImpl::_pToolBar[index]->getNativeView());
        [UIView beginAnimations:@"pToolBar" context:nil];
        [UIView setAnimationDuration:0.4];
        [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
        Omm::Gui::ApplicationImpl::_pToolBar[index]->move(0, _pMainView->height());
        [UIView commitAnimations];
    }
    if (nextIndex != countToolbars) {
        UIView* pToolBar = static_cast<UIView*>(Omm::Gui::ApplicationImpl::_pToolBar[nextIndex]->getNativeView());
        [UIView beginAnimations:@"pToolBar" context:nil];
        [UIView setAnimationDuration:0.4];
        [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
        Omm::Gui::ApplicationImpl::_pToolBar[nextIndex]->move(0, _pMainView->height() - offset);
        [UIView commitAnimations];
    }
    Omm::Gui::ApplicationImpl::_toolBarIndex = nextIndex;

//    int offset = _toolBarVisible ? 0 : Omm::Gui::ApplicationImpl::_pToolBar->height();
//    _toolBarVisible = !_toolBarVisible;
//    UIView* pToolBar = static_cast<UIView*>(Omm::Gui::ApplicationImpl::_pToolBar->getNativeView());
//    [UIView beginAnimations:@"pToolBar" context:nil];
//    [UIView setAnimationDuration:0.4];
//    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
//    Omm::Gui::ApplicationImpl::_pToolBar->move(0, _pMainView->height() - offset);
//    [UIView commitAnimations];
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
//View* ApplicationImpl::_pToolBar = 0;
std::vector<View*> ApplicationImpl::_pToolBar;
int ApplicationImpl::_toolBarHeight = 0;
int ApplicationImpl::_toolBarIndex = 0;

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
ApplicationImpl::addToolBar(View* pView)
{
    _pToolBar.push_back(pView);
}


void
ApplicationImpl::showToolBars(bool show)
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
