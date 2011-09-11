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

#include "EventLoopImpl.h"
#include "Gui/GuiLogger.h"


@interface AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end


@implementation AppDelegate

@synthesize window;

-(void)applicationDidFinishLaunching:(UIApplication *)application {
    window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    window.backgroundColor = [UIColor whiteColor];

    // Override point for customization after application launch
    [window makeKeyAndVisible];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"OmmGuiApplicationDidFinishLaunching" object:nil];
}


-(void)dealloc {
//    [window release];
    [super dealloc];
}

@end  


namespace Omm {
namespace Gui {


class PrivateImpl
{
    friend class EventLoopImpl;
    
    int                 _argc;
    char**              _argv;
    NSAutoreleasePool*  _pAutoreleasePool;
};


EventLoopImpl::EventLoopImpl(int argc, char** argv)
{
    Omm::Gui::Log::instance()->gui().debug("event loop implementation ctor");

    _p = new PrivateImpl;
    _p->_argc = argc;
    _p->_argv = argv;

    _p->_pAutoreleasePool = [[NSAutoreleasePool alloc] init];
}


void
EventLoopImpl::run()
{
    Omm::Gui::Log::instance()->gui().debug("event loop exec ...");
    UIApplicationMain(_p->_argc, _p->_argv, @"UIApplication",@"AppDelegate");
    [_p->_pAutoreleasePool release];
    Omm::Gui::Log::instance()->gui().debug("event loop exec finished.");
}


}  // namespace Omm
}  // namespace Gui
