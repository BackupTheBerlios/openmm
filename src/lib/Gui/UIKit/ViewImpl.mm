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

#include "ViewImpl.h"
#include "Gui/View.h"
#include "Gui/GuiLogger.h"


//@interface OmmGuiViewActionTarget : NSObject
//{
//    Omm::Gui::ViewImpl* _pViewImpl;
//}
//
//@end
//
//
//@implementation OmmGuiViewActionTarget
//
//- (id)initWithImpl:(Omm::Gui::ViewImpl*)pImpl
//{
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiViewActionTarget initWithImpl ...");
//    if (self = [super init]) {
//        _pViewImpl = pImpl;
//    }
//    return self;
//}
//
//
//-(id)selectedAction
//{
//    _pViewImpl->selected();
//}
//
//@end


@interface OmmGuiPlainView : UIView
{
    Omm::Gui::ViewImpl* _pViewImpl;
}

@end


@implementation OmmGuiPlainView

- (id)initWithImpl:(Omm::Gui::ViewImpl*)pImpl
{
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiPlainView initWithImpl ...");
    if (self = [super init]) {
        _pViewImpl = pImpl;
    }
    return self;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiPlainView touch began");
    _pViewImpl->selected();
}

@end


@interface OmmGuiViewSelectorDispatcher : NSObject
{
    Omm::Gui::ViewImpl* _pViewImpl;
}

@end


@implementation OmmGuiViewSelectorDispatcher

- (id)initWithImpl:(Omm::Gui::ViewImpl*)pImpl
{
    if (self = [super init]) {
        _pViewImpl = pImpl;
    }
    return self;
}

- (void)showView
{
    static_cast<UIView*>(_pViewImpl->getNativeView()).hidden = NO;
}

- (void)hideView
{
    static_cast<UIView*>(_pViewImpl->getNativeView()).hidden = YES;
}

@end


namespace Omm {
namespace Gui {


ViewImpl::~ViewImpl()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl dtor");
//    delete _pNativeView;
}


void
ViewImpl::initViewImpl(View* pView, void* pNative)
{
//    Omm::Gui::Log::instance()->gui().debug("init view impl view: " + Poco::NumberFormatter::format(pView) + ", native: " + Poco::NumberFormatter::format(pNative));

    _pView = pView;

    UIView* pNativeView;
    UIViewController* pNativeViewController;
    if ([static_cast<NSObject*>(pNative) isKindOfClass:[UIView class]]) {
//        Omm::Gui::Log::instance()->gui().debug("init view impl native is of type UIView");
        pNativeView = static_cast<UIView*>(pNative);
        pNativeViewController = [[UIViewController alloc] init];
        pNativeViewController.view = pNativeView;
    }
    else if ([static_cast<NSObject*>(pNative) isKindOfClass:[UIViewController class]]) {
//        Omm::Gui::Log::instance()->gui().debug("init view impl native is of type UIViewController");
        pNativeViewController = static_cast<UIViewController*>(pNative);
        pNativeView = pNativeViewController.view;
    }

    if (pView->getParent()) {
        UIView* pParentView = static_cast<UIView*>(pView->getParent()->getNativeView());
        pNativeView.frame = pParentView.frame;
        [pParentView addSubview:pNativeView];
    }
    else {
        pNativeView.frame = CGRectMake(0.0, 0.0, 250.0, 400.0);
    }

//    OmmGuiViewActionTarget* pActionTarget = [[OmmGuiViewActionTarget alloc] initWithImpl:this];
//    [pNativeView addTarget:pActionTarget action:@selector(selectedAction) forControlEvents:UIControlEventTouchUpInside];
//    _pNativeViewSelectorDispatcher = [[OmmGuiViewSelectorDispatcher alloc] initWithImpl:this];
    _pNativeViewSelectorDispatcher = [[OmmGuiViewSelectorDispatcher alloc] initWithImpl:this];

    _pNativeView = pNativeViewController.view;
    _pNativeViewController = pNativeViewController;

//    Omm::Gui::Log::instance()->gui().debug("init view impl view finished.");
}


void
ViewImpl::triggerViewSync(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl trigger view sync");
    // FIXME: implement triggerViewSync with UIKit
}


View*
ViewImpl::getView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


void*
ViewImpl::getNativeView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl get native view: " + Poco::NumberFormatter::format(_pNativeView));

    return _pNativeView;
}


uint32_t
ViewImpl::getNativeWindowId()
{
    // FIXME: return window id on UIKit?
    return 0;
}


void*
ViewImpl::getNativeViewController()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl get native view controller: " + Poco::NumberFormatter::format(_pNativeViewController));

    return _pNativeViewController;
}


void
ViewImpl::setNativeView(void* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl set native view: " + Poco::NumberFormatter::format(pView));
    _pNativeView = pView;
}


void
ViewImpl::addSubview(View* pView)
{
    [static_cast<UIView*>(getNativeView()) addSubview:static_cast<UIView*>(pView->getNativeView())];
}


void
ViewImpl::showView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(showView) withObject:nil waitUntilDone:YES];
//    Omm::Gui::Log::instance()->gui().debug("view impl show finished.");
}


void
ViewImpl::hideView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:YES];
//    Omm::Gui::Log::instance()->gui().debug("view impl hide finished.");
}


int
ViewImpl::widthView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl width.");

    return static_cast<UIView*>(getNativeView()).frame.size.width;
}


int
ViewImpl::heightView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl height.");
    return static_cast<UIView*>(getNativeView()).frame.size.height;
}


void
ViewImpl::resizeView(int width, int height)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl resize.");
    CGRect frame = static_cast<UIView*>(getNativeView()).frame;
    frame.size.width = width;
    frame.size.height = height;
    static_cast<UIView*>(getNativeView()).frame = frame;
}


float
ViewImpl::getFontSize()
{
    // FIXME: get font size with UIKit
    return 0.0;
}


void
ViewImpl::setFontSize(float fontSize)
{
    // FIXME: set font size with UIKit
}


void
ViewImpl::moveView(int x, int y)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl move.");
    CGRect frame = static_cast<UIView*>(getNativeView()).frame;
    frame.origin.x = x;
    frame.origin.y = y;
    static_cast<UIView*>(getNativeView()).frame = frame;
}


void
ViewImpl::setHighlighted(bool highlighted)
{
    Omm::Gui::Log::instance()->gui().debug("view impl set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
    if (highlighted) {
        static_cast<UIView*>(getNativeView()).backgroundColor = [UIColor colorWithRed:0.5 green:0.8 blue:1.0 alpha:1.0];
    }
    else {
        static_cast<UIView*>(getNativeView()).backgroundColor = [UIColor whiteColor];
    }
}


void
ViewImpl::setBackgroundColor(const Color& color)
{
    static_cast<UIView*>(getNativeView()).backgroundColor = static_cast<UIColor*>(color.getNativeColor());
}


void
ViewImpl::presented()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl presented.");
    IMPL_NOTIFY_CONTROLLER(Controller, presented);
}


void
ViewImpl::resized(int width, int height)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl resized.");
    IMPL_NOTIFY_CONTROLLER(Controller, resized, width, height);
}


void
ViewImpl::selected()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl selected.");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
}


PlainViewImpl::PlainViewImpl(View* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("plain view impl ctor.");
    OmmGuiPlainView* pNativeView = [[OmmGuiPlainView alloc] initWithImpl:this];
    pNativeView.backgroundColor = [UIColor whiteColor];

    initViewImpl(pView, pNativeView);
}

}  // namespace Omm
}  // namespace Gui
