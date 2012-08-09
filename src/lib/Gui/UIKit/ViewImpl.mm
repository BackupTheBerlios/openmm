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
#include "UIDrag.h"
#include "ViewRegistry.h"
#include "Gui/View.h"
#include "Gui/GuiLogger.h"

// There are three options to intercept user events in UIKit:
// UIResponder: touchesBegan() ...
// UIControl: via action -> target
// UIGestureRecognizer: via action -> target
//
// inheritance: UIControl -> UIView -> UIResponder
// only UIKit widgets with user input are UIControls, e.g. UIButton, UISlider
// UIGestureRecognizers must be attached to a view (addGestureRecognizer) and does not participate in the responder chain

@interface OmmGuiViewActionTarget : NSObject
{
    Omm::Gui::ViewImpl* _pViewImpl;
}

@end


@implementation OmmGuiViewActionTarget

- (id)initWithImpl:(Omm::Gui::ViewImpl*)pImpl
{
    Omm::Gui::Log::instance()->gui().debug("OmmGuiViewActionTarget initWithImpl ...");
//    if (self = [super initWithTarget:self action:@selector(handleGesture)]) {
    if (self = [super init]) {
        _pViewImpl = pImpl;

//        UITapGestureRecognizer* pSingleFingerDTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapGesture:)];
//        pSingleFingerDTap.numberOfTapsRequired = 2;
//        pSingleFingerDTap.cancelsTouchesInView = NO;
//        [static_cast<UIView*>(_pViewImpl->getNativeView()) addGestureRecognizer:pSingleFingerDTap];
//        [pSingleFingerDTap release];

        UIPanGestureRecognizer* pPanGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleDragGesture:)];
        pPanGesture.minimumNumberOfTouches = 2;
//        pPanGesture.cancelsTouchesInView = NO;
        [static_cast<UIView*>(_pViewImpl->getNativeView()) addGestureRecognizer:pPanGesture];
        [pPanGesture release];
    }
    return self;
}


- (Omm::Gui::View*)getDropView:(UIGestureRecognizer*)pGestureRecognizer
{
    UIView* pMainView = static_cast<UIView*>(Omm::Gui::UIDrag::instance()->getMainView()->getNativeView());
    CGPoint position = [pGestureRecognizer locationInView:pMainView];
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiViewActionTarget drag in point: (" + Poco::NumberFormatter::format(position.x) + ", " + Poco::NumberFormatter::format(position.y) + ")");
    Omm::Gui::UIDrag::instance()->getPointerView()->move(position.x - 70, position.y - 25);
    UIView* pNativeDropView = [pMainView hitTest:position withEvent:nil];
    return Omm::Gui::ViewRegistry::instance()->getViewForNative(pNativeDropView);
}


- (void)handleDragGesture:(UIGestureRecognizer*)pGestureRecognizer
{
//    Omm::Gui::Log::instance()->gui().debug("OmmGuiViewActionTarget drag gesture");
    if (pGestureRecognizer.state == UIGestureRecognizerStateBegan) {
        Omm::Gui::UIDrag::instance()->getPointerView()->show();
        _pViewImpl->dragStarted();
    }
    else if (pGestureRecognizer.state == UIGestureRecognizerStateChanged) {
        Omm::Gui::View* pDropView = [self getDropView:pGestureRecognizer];
        if (pDropView != Omm::Gui::UIDrag::instance()->getDropView()) {
            Omm::Gui::UIDrag::instance()->setDropView(pDropView);
            pDropView->getViewImpl()->dragEntered(Omm::Gui::UIDrag::instance()->getDrag());
        }
        else {
            pDropView->getViewImpl()->dragMoved(Omm::Gui::UIDrag::instance()->getDrag());
        }
    }
    else if (pGestureRecognizer.state == UIGestureRecognizerStateEnded) {
        Omm::Gui::View* pDropView = [self getDropView:pGestureRecognizer];
        Omm::Gui::UIDrag::instance()->getPointerView()->hide();
        pDropView->getViewImpl()->dropped(Omm::Gui::UIDrag::instance()->getDrag());
        Omm::Gui::UIDrag::instance()->setDrag(0);
    }
}


- (void)handleTapGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    Omm::Gui::Log::instance()->gui().debug("OmmGuiViewActionTarget single tap gesture");
}


- (id)selectedAction
{
    _pViewImpl->selected();
}

@end


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


/// OmmGuiViewSelectorDispatcher is needed to enable asynchronous implementation
/// of some methods: showView(), hideView(), syncView()
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

- (void)syncView
{
    _pViewImpl->getView()->syncViewImpl();
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
//        pNativeView.frame = CGRectMake(0.0, 0.0, 250.0, 400.0);
        pNativeView.frame = CGRectMake(0.0, 0.0, 320.0, 480.0);
    }
    _pNativeView = pNativeViewController.view;
    _pNativeViewController = pNativeViewController;

//    OmmGuiViewActionTarget* pActionTarget = [[OmmGuiViewActionTarget alloc] initWithImpl:this];
//    [pNativeView addTarget:pActionTarget action:@selector(selectedAction) forControlEvents:UIControlEventTouchUpInside];
//    _pNativeViewSelectorDispatcher = [[OmmGuiViewSelectorDispatcher alloc] initWithImpl:this];
    _pNativeViewSelectorDispatcher = [[OmmGuiViewSelectorDispatcher alloc] initWithImpl:this];
    _pNativeViewActionTarget = [[OmmGuiViewActionTarget alloc] initWithImpl:this];

    ViewRegistry::instance()->registerView(_pView, pNativeViewController.view);
//    Omm::Gui::Log::instance()->gui().debug("init view impl view finished.");
}


void
ViewImpl::triggerViewSync()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl trigger view sync, view \"" + _pView->getName() + "\" ...");
    [_pNativeViewSelectorDispatcher performSelectorOnMainThread:@selector(syncView) withObject:nil waitUntilDone:[NSThread isMainThread]];
//    Omm::Gui::Log::instance()->gui().debug("view impl trigger view sync, view \"" + _pView->getName() + "\" finished.");
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
ViewImpl::showView(bool async)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(showView) withObject:nil waitUntilDone:YES];
    }
    else {
        static_cast<UIView*>(getNativeView()).hidden = NO;
    }
//    Omm::Gui::Log::instance()->gui().debug("view impl show finished.");
}


void
ViewImpl::hideView(bool async)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:YES];
    }
    else {
        static_cast<UIView*>(getNativeView()).hidden = YES;
    }
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
ViewImpl::setAcceptDrops(bool accept)
{
//    _pNativeView->setAcceptDrops(accept);
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
    _pView->updateLayout();
    IMPL_NOTIFY_CONTROLLER(Controller, resized, width, height);
}


void
ViewImpl::selected()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl selected.");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
}


void
ViewImpl::dragStarted()
{
    Omm::Gui::Log::instance()->gui().debug("view impl drag started in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragStarted);
}


void
ViewImpl::dragEntered(Drag* pDrag)
{
    Omm::Gui::Log::instance()->gui().debug("view impl drag entered in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragEntered, pDrag);
}


void
ViewImpl::dragMoved(Drag* pDrag)
{
    Omm::Gui::Log::instance()->gui().debug("view impl drag moved in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragMoved, pDrag);
}


void
ViewImpl::dragLeft()
{
    Omm::Gui::Log::instance()->gui().debug("view impl drag left view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragLeft);
}


void
ViewImpl::dropped(Drag* pDrag)
{
    Omm::Gui::Log::instance()->gui().debug("view impl drop in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dropped, pDrag);
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
