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
    _pMainView = Omm::Gui::ApplicationImpl::_pApplication->createMainView();
    _pMainView->resize(appFrame.size.width, appFrame.size.height);
    _pMainView->move(appFrame.origin.x, appFrame.origin.y);
    if (Omm::Gui::ApplicationImpl::_pToolBar) {
        _pMainView->addSubview(Omm::Gui::ApplicationImpl::_pToolBar);
        int toolBarHeight = _pMainView->height() / 4;
        Omm::Gui::ApplicationImpl::_pToolBar->resize(_pMainView->width(), toolBarHeight);
        Omm::Gui::ApplicationImpl::_pToolBar->move(0, _pMainView->height());
        _toolBarVisible = false;

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
//    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
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
    int offset = _toolBarVisible ? 0 : Omm::Gui::ApplicationImpl::_pToolBar->height();
    _toolBarVisible = !_toolBarVisible;
    UIView* pToolBar = static_cast<UIView*>(Omm::Gui::ApplicationImpl::_pToolBar->getNativeView());
    [UIView beginAnimations:@"pToolBar" context:nil];
    [UIView setAnimationDuration:0.4];
    [UIView setAnimationCurve:UIViewAnimationCurveEaseOut];
    Omm::Gui::ApplicationImpl::_pToolBar->move(0, _pMainView->height() - offset);
    [UIView commitAnimations];
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
View* ApplicationImpl::_pToolBar = 0;

ApplicationImpl::ApplicationImpl(Application* pApplication)
{
//    Omm::Gui::Log::instance()->gui().debug("application impl ctor");

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
ApplicationImpl::setToolBar(View* pView)
{
    _pToolBar = pView;
}


void
ApplicationImpl::showToolBar(bool show)
{
}


void
ApplicationImpl::setStatusBar(View* pView)
{
    // TODO: implement UIKit status bar
}


void
ApplicationImpl::showStatusBar(bool show)
{
    // TODO: implement UIKit status bar
}


int
ApplicationImpl::run(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    NSAutoreleasePool* pAutoreleasePool = [[NSAutoreleasePool alloc] init];
    int ret = UIApplicationMain(argc, argv, nil, @"OmmGuiAppDelegate");
    [pAutoreleasePool release];
    return ret;
}



}  // namespace Omm
}  // namespace Gui
