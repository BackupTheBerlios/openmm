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

#include "MainWindowImpl.h"
#include "Gui/MainWindow.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiWindow : UIWindow {
}

@end


@implementation OmmGuiWindow

- (void)show:(NSNotification*)notification
{
    if (notification.name == @"OmmGuiApplicationDidFinishLaunching") {
        Omm::Gui::Log::instance()->gui().debug("main window make key and visible");
//        self.backgroundColor = [UIColor whiteColor];
//        [self makeKeyAndVisible];
    }
}


-(void)dealloc {
//    [window release];
    [super dealloc];
}

@end


namespace Omm {
namespace Gui {


MainWindowImpl::MainWindowImpl(View* pView)
{
    _pView = pView;
    Omm::Gui::Log::instance()->gui().debug("main window impl ctor");
    OmmGuiWindow* pNativeView = [[OmmGuiWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    pNativeView.backgroundColor = [UIColor whiteColor];
    _pNativeView = pNativeView;
    [[NSNotificationCenter defaultCenter] addObserver:pNativeView selector:@selector(show:) name:@"OmmGuiApplicationDidFinishLaunching" object:nil];
}


MainWindowImpl::~MainWindowImpl()
{
}


void
MainWindowImpl::setMainView(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("main window impl set main widget");
    UIView* pMainView = static_cast<UIView*>(pView->getNativeView());
//    UIWindow* pWindow = static_cast<UIWindow*>(_pNativeView);
    OmmGuiWindow* pWindow = static_cast<OmmGuiWindow*>(_pNativeView);
//    [pWindow addSubview:pMainView];
}


}  // namespace Omm
}  // namespace Gui
