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
//    if (Omm::Gui::ApplicationImpl::_pToolBar) {
//        _pMainView->addSubview(Omm::Gui::ApplicationImpl::_pToolBar);
//        Omm::Gui::ApplicationImpl::_pToolBar->resize(_pMainView->width(), _pMainView->height() / 3);
//        Omm::Gui::ApplicationImpl::_pToolBar->move(0, 2 * _pMainView->height() / 3);
//        UIPanGestureRecognizer* pPanGesture = [[UIPanGestureRecognizer alloc]
//                initWithTarget:self action:@selector(handlePanGesture:)];
//        pPanGesture.minimumNumberOfTouches = 2;
//        [_pMainView->getNativeView() addGestureRecognizer:pPanGesture];
//        [pPanGesture release];
//    }
    _pMainView->resize(appFrame.size.width, appFrame.size.height);
    _pMainView->move(appFrame.origin.x, appFrame.origin.y);
    [_pWindow addSubview:static_cast<UIView*>(_pMainView->getNativeView())];
    [_pWindow makeKeyAndVisible];
    Omm::Gui::ApplicationImpl::_pApplication->presentedMainView();
}


- (void)handlePanGesture:(UIPanGestureRecognizer*)sender
{
    UIView* pMainView = static_cast<UIView*>(_pMainView->getNativeView());
    UIView* pToolBarView = static_cast<UIView*>(Omm::Gui::ApplicationImpl::_pToolBar->getNativeView());
    CGPoint translate = [sender translationInView:pMainView];

    CGRect newFrame = pToolBarView.frame;
    newFrame.origin.y += translate.y;
    pToolBarView.frame = newFrame;

//    if (sender.state == UIGestureRecognizerStateEnded) {
//        pToolBarView.frame = newFrame;
//        pMainView.frame = newFrame;
//    }
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
ApplicationImpl::addToolBar(View* pView)
{
    _pToolBar = pView;
}


void
ApplicationImpl::showToolBar(bool show)
{
}


int
ApplicationImpl::run(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    NSAutoreleasePool* pAutoreleasePool = [[NSAutoreleasePool alloc] init];
    int ret = UIApplicationMain(argc, argv, nil, @"OmmGuiAppDelegate");
    [pAutoreleasePool release];
    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
    _pApplication->finishedEventLoop();
    return ret;
}



}  // namespace Omm
}  // namespace Gui
