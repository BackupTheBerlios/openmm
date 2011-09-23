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

#include "ButtonImpl.h"
#include "Gui/Button.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiButton : UIButton
//@interface OmmGuiButton : UIRoundedRectButton
{
    Omm::Gui::ButtonViewImpl* _pButtonViewImpl;
}

@end


@implementation OmmGuiButton

- (void)setImpl:(Omm::Gui::ButtonViewImpl*)pImpl
{
    _pButtonViewImpl = pImpl;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
//    Omm::Gui::Log::instance()->gui().debug("button view impl touch began");
    _pButtonViewImpl->pushed();
    [super touchesBegan:touches withEvent:event];
}

@end


namespace Omm {
namespace Gui {


ButtonViewImpl::ButtonViewImpl(View* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("button view impl ctor");
//    OmmGuiButton* pNativeView = [[OmmGuiButton buttonWithType:UIButtonTypeRoundedRect] init];
    OmmGuiButton* pNativeView = [[OmmGuiButton alloc] init];
    pNativeView.backgroundColor = [UIColor yellowColor];
    [pNativeView setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


ButtonViewImpl::~ButtonViewImpl()
{
}


void
ButtonViewImpl::setLabel(const std::string& label)
{
//    Omm::Gui::Log::instance()->gui().debug("button view impl set label");
    NSString* pLabel = [[NSString alloc] initWithUTF8String:label.c_str()];
    [static_cast<UIButton*>(_pNativeView) setTitle:pLabel forState:UIControlStateNormal];
}


void
ButtonViewImpl::pushed()
{
//    Omm::Gui::Log::instance()->gui().debug("button implementation, calling pushed virtual method");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
    IMPL_NOTIFY_CONTROLLER(ButtonController, pushed);
}


}  // namespace Omm
}  // namespace Gui
