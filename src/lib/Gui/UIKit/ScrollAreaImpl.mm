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
//    Omm::LOGNS(Gui, gui, debug, "OmmGuiListView initWithImpl ...");
    if (self = [super init]) {
        self.delegate = self;
        _pViewImpl = pImpl;
    }
    return self;
}

- (void)scrollViewDidScroll:(UIScrollView*)scrollView
{
//    Omm::LOGNS(Gui, gui, debug, "list view impl scrolling ...");
    _pViewImpl->viewScrolled();
}


- (void)layoutSubviews
{
    _pViewImpl->resized(self.frame.size.width, self.frame.size.height);
}

@end


namespace Omm {
namespace Gui {


class AreaViewImpl : public ViewImpl
{
public:
    AreaViewImpl(View* pView, UIScrollView* pNativeView);

    virtual int widthView();
    virtual int heightView();
    virtual void resizeView(int width, int height);
};


AreaViewImpl::AreaViewImpl(View* pView, UIScrollView* pNativeView)
{
    initViewImpl(pView, pNativeView);
}


int
AreaViewImpl::widthView()
{
    return static_cast<UIScrollView*>(getNativeView()).contentSize.width;
}


int
AreaViewImpl::heightView()
{
    return static_cast<UIScrollView*>(getNativeView()).contentSize.height;
}


void
AreaViewImpl::resizeView(int width, int height)
{
    static_cast<UIScrollView*>(getNativeView()).contentSize = CGSizeMake(width, height);
}


class AreaView : public View
{
public:
    AreaView(View* pParent, UIScrollView* pNativeView);
};


AreaView::AreaView(View* pParent, UIScrollView* pNativeView) :
View(pParent, false)
{
    _pImpl = new AreaViewImpl(this, pNativeView);
}


ScrollAreaViewImpl::ScrollAreaViewImpl(View* pView)
{
    OmmGuiScrollView* pNativeView = [[OmmGuiScrollView alloc] initWithImpl:this];
    initViewImpl(pView, pNativeView);
//    _pAreaView = new AreaView(pView->getParent(), pNativeView);
    _pAreaView = new AreaView(0, pNativeView);
}


ScrollAreaViewImpl::~ScrollAreaViewImpl()
{
}


View*
ScrollAreaViewImpl::getAreaView()
{
    return _pAreaView;
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


void
ScrollAreaViewImpl::scrollContentsTo(int x, int y)
{
    [static_cast<UIScrollView*>(getNativeView()) setContentOffset:CGPointMake(x, y) animated:YES];
}


void
ScrollAreaViewImpl::showScrollBars(bool show)
{
    static_cast<UIScrollView*>(getNativeView()).showsHorizontalScrollIndicator = show;
    static_cast<UIScrollView*>(getNativeView()).showsVerticalScrollIndicator = show;
}


void
ScrollAreaViewImpl::setAreaResizable(bool resize)
{
    // is there something similar in UIKit?
}


void
ScrollAreaViewImpl::viewScrolled()
{
    IMPL_NOTIFY_CONTROLLER(ScrollAreaController, scrolled, getXOffset(), getYOffset());
}


}  // namespace Omm
}  // namespace Gui
