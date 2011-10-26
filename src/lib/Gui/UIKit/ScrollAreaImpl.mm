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

#include <Poco/NumberFormatter.h>

#import <UIKit/UIKit.h>

#include "ScrollAreaImpl.h"
#include "Gui/ScrollArea.h"
#include "Gui/GuiLogger.h"


@interface OmmGuiScrollView : UIScrollView<UIScrollViewDelegate>
{
    Omm::Gui::ScrollAreaViewImpl* _pViewImpl;
}

@end


@implementation OmmGuiScrollView

- (id)initWithImpl:(Omm::Gui::ScrollAreaViewImpl*)pImpl
{
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiListView initWithImpl ...");
    if (self = [super init]) {
        self.delegate = self;
        _pViewImpl = pImpl;
        self.backgroundColor = [UIColor greenColor];
//        self.frame = CGRectMake(0.0, 0.0, 100.0, 100.0);
    }
    return self;
}

- (void)scrollViewDidScroll:(UIScrollView*)scrollView
{
//    Omm::Gui::Log::instance()->gui().debug("list view impl scrolling ...");
    _pViewImpl->viewScrolled();
}


- (void)layoutSubviews
{
    _pViewImpl->resized(self.frame.size.width, self.frame.size.height);
}

@end


namespace Omm {
namespace Gui {


ScrollAreaViewImpl::ScrollAreaViewImpl(View* pView)
{
    OmmGuiScrollView* pNativeView = [[OmmGuiScrollView alloc] initWithImpl:this];
    initViewImpl(pView, pNativeView);
}


ScrollAreaViewImpl::~ScrollAreaViewImpl()
{
}


int
ScrollAreaViewImpl::getViewportWidth()
{
    return static_cast<UIScrollView*>(getNativeView()).frame.size.width;
}


int
ScrollAreaViewImpl::getViewportHeight()
{
    return static_cast<UIScrollView*>(getNativeView()).frame.size.height;
}


int
ScrollAreaViewImpl::getXOffset()
{
    return static_cast<UIScrollView*>(getNativeView()).contentOffset.x;
}


int
ScrollAreaViewImpl::getYOffset()
{
    return static_cast<UIScrollView*>(getNativeView()).contentOffset.y;
}


int
ScrollAreaViewImpl::getScrollAreaWidth()
{
    return static_cast<UIScrollView*>(getNativeView()).contentSize.width;
}


int
ScrollAreaViewImpl::getScrollAreaHeight()
{
    return static_cast<UIScrollView*>(getNativeView()).contentSize.height;
}


void
ScrollAreaViewImpl::resizeScrollArea(int width, int height)
{
    static_cast<UIScrollView*>(getNativeView()).contentSize = CGSizeMake(width, height);
}


void
ScrollAreaViewImpl::viewScrolled()
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, getXOffset(), getYOffset());
}


}  // namespace Omm
}  // namespace Gui
