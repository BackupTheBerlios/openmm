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
    UIWindow* window;
}

@property (nonatomic, retain) UIWindow* window;

@end


@implementation OmmGuiAppDelegate

@synthesize window;

-(void)applicationDidFinishLaunching:(UIApplication*)application
{
    window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    window.backgroundColor = [UIColor whiteColor];

    Omm::Gui::View* pMainView = Omm::Gui::ApplicationImpl::_pApplication->createMainView();
    pMainView->resize(window.frame.size.width, window.frame.size.height);
    [window addSubview:static_cast<UIView*>(pMainView->getNativeView())];
    [window makeKeyAndVisible];
    Omm::Gui::ApplicationImpl::_pApplication->presentedMainView();
}


-(void)dealloc
{
    [window release];
    [super dealloc];
}

@end



namespace Omm {
namespace Gui {


Application* ApplicationImpl::_pApplication = 0;

ApplicationImpl::ApplicationImpl(Application* pApplication)
{
    Omm::Gui::Log::instance()->gui().debug("application impl ctor");

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
ApplicationImpl::run(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    NSAutoreleasePool* pAutoreleasePool = [[NSAutoreleasePool alloc] init];
    int ret = UIApplicationMain(argc, argv, nil,@"OmmGuiAppDelegate");
    [pAutoreleasePool release];
    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
    _pApplication->finishedEventLoop();
    return ret;
}



}  // namespace Omm
}  // namespace Gui
