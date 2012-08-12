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

#include "SliderImpl.h"
#include "Gui/Slider.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiSlider : UISlider {
    Omm::Gui::SliderViewImpl* _pSliderViewImpl;
}

@end


@implementation OmmGuiSlider

- (void)setImpl:(Omm::Gui::SliderViewImpl*)pImpl
{
    _pSliderViewImpl = pImpl;
    [self addTarget:self action:@selector(moved) forControlEvents:UIControlEventTouchDragInside];
}


- (void)moved
{
    _pSliderViewImpl->valueChanged(self.value);
}

@end


namespace Omm {
namespace Gui {


SliderViewImpl::SliderViewImpl(View* pView)
{
//    LOG(gui, debug, "slider view impl ctor");

    OmmGuiSlider* pNativeView = [[OmmGuiSlider alloc] init];
    [pNativeView setImpl:this];
    pNativeView.maximumValue = 100.0;

    initViewImpl(pView, pNativeView);
}


SliderViewImpl::~SliderViewImpl()
{
}


void
SliderViewImpl::setValue(int value)
{
    UISlider* pNativeView = static_cast<UISlider*>(_pNativeView);

    pNativeView.value = value;
}


void
SliderViewImpl::setEnabled(bool enabled)
{
    static_cast<UISlider*>(_pNativeView).enabled = enabled;
}


void
SliderViewImpl::valueChanged(int value)
{
//    LOG(gui, debug, "slider implementation, calling value changed virtual method");
    static_cast<SliderView*>(getView())->valueChangedView(value);
}


}  // namespace Omm
}  // namespace Gui
