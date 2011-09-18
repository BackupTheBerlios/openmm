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

#include "LabelImpl.h"
#include "Gui/Label.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiLabel : UILabel
{
    Omm::Gui::LabelViewImpl* _pLabelViewImpl;
}

@end


@implementation OmmGuiLabel

- (void)setImpl:(Omm::Gui::LabelViewImpl*)pImpl
{
    _pLabelViewImpl = pImpl;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    Omm::Gui::Log::instance()->gui().debug("Label view impl touch began");
    _pLabelViewImpl->pushed();
    [super touchesBegan:touches withEvent:event];
}

@end


namespace Omm {
namespace Gui {


LabelViewImpl::LabelViewImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("Label view impl ctor");
    OmmGuiLabel* pNativeView = [[OmmGuiLabel alloc] init];
    pNativeView.backgroundColor = [UIColor greenColor];
    [pNativeView setTextColor:[UIColor blackColor]];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


LabelViewImpl::~LabelViewImpl()
{
}


void
LabelViewImpl::setLabel(const std::string& label)
{
    Omm::Gui::Log::instance()->gui().debug("Label view impl set label");
    NSString* pLabel = [[NSString alloc] initWithUTF8String:label.c_str()];
    [static_cast<UILabel*>(_pNativeView) performSelectorOnMainThread:@selector(setText:) withObject:pLabel waitUntilDone:YES];
}


void
LabelViewImpl::pushed()
{
    Omm::Gui::Log::instance()->gui().debug("Label implementation, calling pushed virtual method");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
}


}  // namespace Omm
}  // namespace Gui
