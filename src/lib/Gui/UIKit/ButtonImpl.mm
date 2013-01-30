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
#include "ImageImpl.h"


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
    [self addTarget:self action:@selector(pushed) forControlEvents:UIControlEventTouchUpInside];
}


- (void)pushed
{
    _pButtonViewImpl->pushed();
}

@end


namespace Omm {
namespace Gui {


ButtonViewImpl::ButtonViewImpl(View* pView)
{
//    LOG(gui, debug, "button view impl ctor");
//    OmmGuiButton* pNativeView = [[OmmGuiButton buttonWithType:UIButtonTypeRoundedRect] init];
    OmmGuiButton* pNativeView = [[OmmGuiButton alloc] init];
//    pNativeView.backgroundColor = [UIColor yellowColor];
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
//    LOG(gui, debug, "button view impl set label");
    NSString* pLabel = [[NSString alloc] initWithUTF8String:label.c_str()];
    [static_cast<UIButton*>(_pNativeView) setTitle:pLabel forState:UIControlStateNormal];
}


void
ButtonViewImpl::setEnabled(bool enabled)
{
    static_cast<UIButton*>(_pNativeView).enabled = enabled;
}


void
ButtonViewImpl::setImage(Image* pImage)
{
    UIImage* pUiImage = static_cast<UIImage*>(pImage->getModel()->getNativeModel());
    if (pUiImage) {
        [static_cast<UIButton*>(_pNativeView) setImage:(UIImage*)pUiImage forState:UIControlStateNormal];
    }
}


void
ButtonViewImpl::pushed()
{
    LOG(gui, debug, "button implementation, calling pushed virtual method");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
    IMPL_NOTIFY_CONTROLLER(ButtonController, pushed);
}


}  // namespace Omm
}  // namespace Gui
