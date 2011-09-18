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

#include "ListItemImpl.h"
#include "Gui/ListItem.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiListItemView : UIView
{
    Omm::Gui::ListItemImpl* _pListItemImpl;
    UILabel*                _pLabel;
}

@end


@implementation OmmGuiListItemView

- (void)initWithImpl:(Omm::Gui::ListItemImpl*)pImpl
{
    _pListItemImpl = pImpl;
    _pLabel = [[UILabel alloc] initWithFrame:CGRectMake(0.0, 50.0, 100.0, 50.0)];
//    _pLabel = [[UILabel alloc] initWithFrame:superView.frame];
    [self addSubview:_pLabel];
    _pLabel.frame = self.frame;
}


- (void)setLabel:(NSString*)pLabel
{
    _pLabel.text = pLabel;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    Omm::Gui::Log::instance()->gui().debug("button view impl touch began");
    _pListItemImpl->listItemSelected();
}


@end


namespace Omm {
namespace Gui {


ListItemImpl::ListItemImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("list item impl ctor");

    OmmGuiListItemView* pNativeView = [[OmmGuiListItemView alloc] initWithFrame:CGRectMake(0.0, 50.0, 100.0, 50.0)];
    [pNativeView initWithImpl:this];
    pNativeView.backgroundColor = [UIColor yellowColor];

    init(pView, pNativeView);
}


ListItemImpl::~ListItemImpl()
{
//    delete _pNameLabel;
}


void
ListItemImpl::setLabel(const std::string& text)
{
    NSString* pLabel = [[NSString alloc] initWithUTF8String:text.c_str()];
    [static_cast<OmmGuiListItemView*>(_pNativeView) setLabel:pLabel];
}


void
ListItemImpl::listItemSelected()
{
    selected();
}




}  // namespace Omm
}  // namespace Gui
