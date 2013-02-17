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

#include "Gui/Image.h"
#include "SwitchImpl.h"
#include "Gui/Switch.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiSwitch : UISwitch {
    Omm::Gui::SwitchViewImpl* _pSwitchViewImpl;
}

@end


@implementation OmmGuiSwitch

- (void)setImpl:(Omm::Gui::SwitchViewImpl*)pImpl
{
    _pSwitchViewImpl = pImpl;
    [self addTarget:self action:@selector(switched) forControlEvents:UIControlEventValueChanged];
}


- (void)switched
{
    _pSwitchViewImpl->switched(self.on);
}

@end


namespace Omm {
namespace Gui {


SwitchViewImpl::SwitchViewImpl(View* pView)
{
    OmmGuiSwitch* pNativeView = [[OmmGuiSwitch alloc] init];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


bool
SwitchViewImpl::getStateOn()
{
    return static_cast<UISwitch*>(_pNativeView).on;
}


void
SwitchViewImpl::setState(bool on)
{
    static_cast<UISwitch*>(_pNativeView).on = on;
}


void
SwitchViewImpl::switched(bool on)
{
    LOG(gui, debug, "switch impl: " + std::string(on ? "on" : "off"));
    IMPL_NOTIFY_CONTROLLER(SwitchController, switched, on);
}

}  // namespace Omm
}  // namespace Gui
