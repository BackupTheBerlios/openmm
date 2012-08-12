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

#include "TextLineImpl.h"
#include "Gui/TextLine.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiTextLine : UITextField<UITextFieldDelegate>
{
    Omm::Gui::TextLineViewImpl* _pTextLineViewImpl;
}

@end


@implementation OmmGuiTextLine

- (void)setImpl:(Omm::Gui::TextLineViewImpl*)pImpl
{
    _pTextLineViewImpl = pImpl;
}


- (BOOL)textFieldShouldReturn:(UITextField*)textField
{
    _pTextLineViewImpl->editingFinished(std::string([textField.text UTF8String]));
    [textField resignFirstResponder];
    return YES;
}


@end


namespace Omm {
namespace Gui {


TextLineViewImpl::TextLineViewImpl(View* pView)
{
//    LOG(gui, debug, "TextLine view impl ctor");
    OmmGuiTextLine* pNativeView = [[OmmGuiTextLine alloc] init];
    [pNativeView setImpl:this];

    initViewImpl(pView, pNativeView);
}


TextLineViewImpl::~TextLineViewImpl()
{
}


void
TextLineViewImpl::setTextLine(const std::string& line)
{
//    LOG(gui, debug, "TextLine view impl set label");
    NSString* pTextLine = [[NSString alloc] initWithUTF8String:line.c_str()];
    [static_cast<UITextField*>(_pNativeView) setText:pTextLine];
}


void
TextLineViewImpl::setAlignment(View::Alignment alignment)
{
    // FIXME: set label alignment with UIKit
    switch(alignment) {
        case View::AlignLeft:
//            static_cast<UITextField*>(_pNativeView)->setAlignment(Qt::AlignLeft);
            break;
        case View::AlignCenter:
//            static_cast<UITextField*>(_pNativeView)->setAlignment(Qt::AlignCenter);
            break;
        case View::AlignRight:
//            static_cast<UITextField*>(_pNativeView)->setAlignment(Qt::AlignRight);
            break;
    }
}


void
TextLineViewImpl::editingFinished(const std::string& text)
{
//    LOG(gui, debug, "text line view impl, calling editedText virtual method: " + text);
    IMPL_NOTIFY_CONTROLLER(TextLineController, editedText, text);
}


}  // namespace Omm
}  // namespace Gui
