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
    LOGNS(Omm::Gui, gui, debug, "OmmGuiViewActionTarget initWithImpl ...");
//    if (self = [super initWithTarget:self action:@selector(handleGesture)]) {
    if (self = [super init]) {
        _pViewImpl = pImpl;

        UITapGestureRecognizer* pSingleFingerDoubleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapGesture:)];
        pSingleFingerDoubleTap.numberOfTapsRequired = 2;
        pSingleFingerDoubleTap.cancelsTouchesInView = NO;
        [static_cast<UIView*>(_pViewImpl->getNativeView()) addGestureRecognizer:pSingleFingerDoubleTap];
        [pSingleFingerDoubleTap release];

        UILongPressGestureRecognizer* pLongPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleDragGesture:)];
        [static_cast<UIView*>(_pViewImpl->getNativeView()) addGestureRecognizer:pLongPressGesture];
        [pLongPressGesture release];

        UIPanGestureRecognizer* pSwipeGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handleDragGesture:)];
        pSwipeGesture.minimumNumberOfTouches = 2;
//        pPanGesture.cancelsTouchesInView = NO;
        [static_cast<UIView*>(_pViewImpl->getNativeView()) addGestureRecognizer:pSwipeGesture];
        [pSwipeGesture release];
    }
    return self;
}


- (Omm::Gui::View*)getDropView:(UIGestureRecognizer*)pGestureRecognizer
{
    UIView* pMainView = static_cast<UIView*>(Omm::Gui::UIDrag::instance()->getMainView()->getNativeView());
    CGPoint position = [pGestureRecognizer locationInView:pMainView];
    LOGNS(Omm::Gui, gui, debug, "OmmGuiViewActionTarget drag in point: (" + Poco::NumberFormatter::format(position.x) + ", " + Poco::NumberFormatter::format(position.y) + ")");
    Omm::Gui::UIDrag::instance()->getPointerView()->move(position.x - 70, position.y - 25);
    UIView* pNativeDropView = [pMainView hitTest:position withEvent:nil];
    return Omm::Gui::ViewRegistry::instance()->getViewForNative(pNativeDropView);
}


- (void)handleDragGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    CGPoint position = [pGestureRecognizer locationInView:static_cast<UIView*>(_pViewImpl->getNativeView())];
    bool accept;
    LOGNS(Omm::Gui, gui, debug, "OmmGuiViewActionTarget drag gesture [" + Poco::NumberFormatter::format(position.x) + ", " + Poco::NumberFormatter::format(position.y) + "]");
    if (pGestureRecognizer.state == UIGestureRecognizerStateBegan) {
        Omm::Gui::UIDrag::instance()->getPointerView()->show();
        _pViewImpl->dragStarted();
    }
    else if (pGestureRecognizer.state == UIGestureRecognizerStateChanged) {
        Omm::Gui::View* pDropView = [self getDropView:pGestureRecognizer];
        _pViewImpl->mouseMoved(Omm::Gui::Position(position.x, position.y));
        if (pDropView != Omm::Gui::UIDrag::instance()->getDropView()) {
            Omm::Gui::UIDrag::instance()->setDropView(pDropView);
            pDropView->getViewImpl()->dragEntered(Omm::Gui::Position(position.x, position.y), Omm::Gui::UIDrag::instance()->getDrag(), accept);
        }
        else {
            pDropView->getViewImpl()->dragMoved(Omm::Gui::Position(position.x, position.y), Omm::Gui::UIDrag::instance()->getDrag(), accept) ;
        }
    }
    else if (pGestureRecognizer.state == UIGestureRecognizerStateEnded) {
        Omm::Gui::View* pDropView = [self getDropView:pGestureRecognizer];
        Omm::Gui::UIDrag::instance()->getPointerView()->hide();
        pDropView->getViewImpl()->dropped(Omm::Gui::Position(position.x, position.y), Omm::Gui::UIDrag::instance()->getDrag(), accept);
        Omm::Gui::UIDrag::instance()->setDrag(0);
    }
}


- (void)handleTapGesture:(UIGestureRecognizer*)pGestureRecognizer
{
    LOGNS(Omm::Gui, gui, debug, "OmmGuiViewActionTarget single finger double tap gesture");
    _pViewImpl->activated();
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
//    LOGNS(Omm::Gui, gui, debug, "OmmGuiPlainView initWithImpl ...");
    if (self = [super init]) {
        _pViewImpl = pImpl;

    }
    return self;
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
//    LOGNS(Omm::Gui, gui, debug, "OmmGuiPlainView touch began");
    _pViewImpl->selected();
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
//    LOGNS(Omm::Gui, gui, debug, "OmmGuiPlainView touch began");
    _pViewImpl->released();
}


- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
//    LOGNS(Omm::Gui, gui, debug, "OmmGuiPlainView touch moved");
    CGPoint position = [[touches anyObject] locationInView:static_cast<UIView*>(_pViewImpl->getNativeView())];
    _pViewImpl->mouseMoved(Omm::Gui::Position(position.x, position.y));
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

- (void)raiseView
{
    [static_cast<UIView*>(_pViewImpl->getView()->getParent()->getNativeView()) bringSubviewToFront:static_cast<UIView*>(_pViewImpl->getNativeView())];
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
//    LOG(gui, debug, "view impl dtor");
//    delete _pNativeView;
}


void
ViewImpl::initViewImpl(View* pView, void* pNative)
{
//    LOG(gui, debug, "init view impl view: " + Poco::NumberFormatter::format(pView) + ", native: " + Poco::NumberFormatter::format(pNative));

    _pView = pView;

    UIView* pNativeView;
    UIViewController* pNativeViewController;
    if ([static_cast<NSObject*>(pNative) isKindOfClass:[UIView class]]) {
//        LOG(gui, debug, "init view impl native is of type UIView");
        pNativeView = static_cast<UIView*>(pNative);
        pNativeViewController = [[UIViewController alloc] init];
        pNativeViewController.view = pNativeView;
    }
    else if ([static_cast<NSObject*>(pNative) isKindOfClass:[UIViewController class]]) {
//        LOG(gui, debug, "init view impl native is of type UIViewController");
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
//    LOG(gui, debug, "init view impl view finished.");
}


void
ViewImpl::triggerViewSync()
{
//    LOG(gui, debug, "view impl trigger view sync, view \"" + _pView->getName() + "\" ...");
    [_pNativeViewSelectorDispatcher performSelectorOnMainThread:@selector(syncView) withObject:nil waitUntilDone:[NSThread isMainThread]];
//    LOG(gui, debug, "view impl trigger view sync, view \"" + _pView->getName() + "\" finished.");
}


View*
ViewImpl::getView()
{
//    LOG(gui, debug, "view impl get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


void*
ViewImpl::getNativeView()
{
//    LOG(gui, debug, "view impl get native view: " + Poco::NumberFormatter::format(_pNativeView));

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
//    LOG(gui, debug, "view impl get native view controller: " + Poco::NumberFormatter::format(_pNativeViewController));

    return _pNativeViewController;
}


void
ViewImpl::setNativeView(void* pView)
{
//    LOG(gui, debug, "view impl set native view: " + Poco::NumberFormatter::format(pView));
    _pNativeView = pView;
}


void
ViewImpl::setParent(View* pView)
{
    [static_cast<UIView*>(_pNativeView) removeFromSuperview];
    [static_cast<UIView*>(pView->getNativeView()) addSubview:static_cast<UIView*>(_pNativeView)];
}


void
ViewImpl::showView(bool async)
{
//    LOG(gui, debug, "view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(showView) withObject:nil waitUntilDone:YES];
    }
    else {
        static_cast<UIView*>(getNativeView()).hidden = NO;
    }
    // NOTE: there seem to be no event for showing/hiden of an UIView, so we have to call it explicitly on invocation of show()/hide()
    IMPL_NOTIFY_CONTROLLER(Controller, shown);
//    LOG(gui, debug, "view impl show finished.");
}


void
ViewImpl::hideView(bool async)
{
//    LOG(gui, debug, "view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(hideView) withObject:nil waitUntilDone:YES];
    }
    else {
        static_cast<UIView*>(getNativeView()).hidden = YES;
    }
    // NOTE: there seem to be no event for showing/hiden of an UIView, so we have to call it explicitly on invocation of show()/hide()
    IMPL_NOTIFY_CONTROLLER(Controller, hidden);
//    LOG(gui, debug, "view impl hide finished.");
}


void
ViewImpl::raise(bool async)
{
    if (async) {
        [static_cast<OmmGuiViewSelectorDispatcher*>(_pNativeViewSelectorDispatcher) performSelectorOnMainThread:@selector(raiseView) withObject:nil waitUntilDone:YES];
    }
    else {
        [static_cast<UIView*>(getView()->getParent()->getNativeView()) bringSubviewToFront:static_cast<UIView*>(getNativeView())];
    }
}


bool
ViewImpl::isVisible()
{
    return !static_cast<UIView*>(getNativeView()).hidden;
}


int
ViewImpl::posXView()
{
    return static_cast<UIView*>(getNativeView()).frame.origin.x;
}


int
ViewImpl::posYView()
{
    return static_cast<UIView*>(getNativeView()).frame.origin.y;
}


int
ViewImpl::widthView()
{
//    LOG(gui, debug, "view impl width.");

    return static_cast<UIView*>(getNativeView()).frame.size.width;
}


int
ViewImpl::heightView()
{
//    LOG(gui, debug, "view impl height.");
    return static_cast<UIView*>(getNativeView()).frame.size.height;
}


void
ViewImpl::resizeView(int width, int height)
{
//    LOG(gui, debug, "view impl resize.");
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
//    LOG(gui, debug, "view impl move.");
    CGRect frame = static_cast<UIView*>(getNativeView()).frame;
    frame.origin.x = x;
    frame.origin.y = y;
    static_cast<UIView*>(getNativeView()).frame = frame;
}


void
ViewImpl::setHighlighted(bool highlighted)
{
    LOG(gui, debug, "view impl set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
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
ViewImpl::shown()
{
//    LOG(gui, debug, "view impl shown.");
    IMPL_NOTIFY_CONTROLLER(Controller, shown);
}


void
ViewImpl::hidden()
{
//    LOG(gui, debug, "view impl hidden.");
    IMPL_NOTIFY_CONTROLLER(Controller, hidden);
}


void
ViewImpl::resized(int width, int height)
{
//    LOG(gui, debug, "view impl resized.");
    _pView->updateLayout();
    IMPL_NOTIFY_CONTROLLER(Controller, resized, width, height);
}


void
ViewImpl::selected()
{
//    LOG(gui, debug, "view impl selected.");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
}


void
ViewImpl::released()
{
    LOG(gui, debug, "view impl released view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, released);
}


void
ViewImpl::activated()
{
    LOG(gui, debug, "view impl activated view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, activated);
}


void
ViewImpl::mouseMoved(const Position& pos)
{
    LOG(gui, debug, "view impl mouse moved in view: " + _pView->getName() + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    IMPL_NOTIFY_CONTROLLER(Controller, mouseMoved, pos);
}


void
ViewImpl::dragStarted()
{
    LOG(gui, debug, "view impl drag started in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragStarted);
}


void
ViewImpl::dragEntered(const Position& pos, Drag* pDrag, bool& accept)
{
    LOG(gui, debug, "view impl drag entered in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragEntered, pos, pDrag, accept);
}


void
ViewImpl::dragMoved(const Position& pos, Drag* pDrag, bool& accept)
{
    LOG(gui, debug, "view impl drag moved in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragMoved, pos, pDrag, accept);
}


void
ViewImpl::dragLeft()
{
    LOG(gui, debug, "view impl drag left view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dragLeft);
}


void
ViewImpl::dropped(const Position& pos, Drag* pDrag, bool& accept)
{
    LOG(gui, debug, "view impl drop in view: " + _pView->getName());
    IMPL_NOTIFY_CONTROLLER(Controller, dropped, pos, pDrag, accept);
}


PlainViewImpl::PlainViewImpl(View* pView)
{
//    LOG(gui, debug, "plain view impl ctor.");
    OmmGuiPlainView* pNativeView = [[OmmGuiPlainView alloc] initWithImpl:this];
    pNativeView.backgroundColor = [UIColor whiteColor];

    initViewImpl(pView, pNativeView);
}

}  // namespace Omm
}  // namespace Gui
