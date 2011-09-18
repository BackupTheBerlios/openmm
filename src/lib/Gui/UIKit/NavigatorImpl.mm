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


namespace Omm {
namespace Gui {


NavigatorViewImpl::NavigatorViewImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("navigator view impl ctor");

    UINavigationController* pNativeView = [[UINavigationController alloc] init];
//    [pNativeView setImpl:this];

    init(pView, pNativeView);
}


NavigatorViewImpl::~NavigatorViewImpl()
{
}


void
NavigatorViewImpl::pushView(View* pView, const std::string name)
{
    Omm::Gui::Log::instance()->gui().debug("navigator view implementation push view");

//    if ([static_cast<NSObject*>(pView->getNativeView()) isKindOfClass:[UIViewController class]]) {
        UINavigationController* pNativeViewController = static_cast<UINavigationController*>(getNativeViewController());
        UIViewController* pViewController = static_cast<UIViewController*>(pView->getViewImpl()->getNativeViewController());
        NSString* pName = [[NSString alloc] initWithUTF8String:name.c_str()];
        pViewController.title = pName;

        [pNativeViewController pushViewController:pViewController animated:YES];
//    }
//    else {
//        Omm::Gui::Log::instance()->gui().error("navigator view implementation cannot push view, must be a UIViewController.");
//    }
}


} // namespace Gui
} // namespace Omm
