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

#include <QtGui>
#include <Poco/NumberFormatter.h>

#include "ViewImpl.h"
#include "ColorImpl.h"
#include "QtViewImpl.h"
#include "Gui/View.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ViewImpl::~ViewImpl()
{
//    LOG(gui, debug, "view impl dtor");
//    delete _pNativeView;
}


void
ViewImpl::initViewImpl(View* pView, QWidget* pNative, SignalProxy* pSignalProxy)
{
//    LOG(gui, debug, "view impl init");

    _pView = pView;
    _pNativeView = pNative;

    if (!pSignalProxy) {
        _pSignalProxy = new SignalProxy(this);
    }
    else {
        _pSignalProxy = pSignalProxy;
    }
    _pSignalProxy->init();

    _pNativeView->setAutoFillBackground(true);

//    _pEventFilter = new QtEventFilter(this);
//    _pNativeView->installEventFilter(_pEventFilter);

    if (pView->getParent()) {
        QWidget* pParentWidget = static_cast<QWidget*>(pView->getParent()->getNativeView());
        _pNativeView->resize(pParentWidget->size());
        _pNativeView->setParent(pParentWidget);
    }
//    else {
//        _pNativeView->resize(250, 400);
//    }
}


void
ViewImpl::triggerViewSync()
{
//    LOG(gui, debug, "view impl trigger view sync");
    _pSignalProxy->syncView();
}


View*
ViewImpl::getView()
{
//    LOG(gui, debug, "view impl get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


QWidget*
ViewImpl::getNativeView()
{
//    LOG(gui, debug, "view impl get native view: " + Poco::NumberFormatter::format(_pNativeView));
    return _pNativeView;
}


#ifdef __WINDOWS__
    void*
#else
    uint32_t
#endif
ViewImpl::getNativeWindowId()
{
//    LOG(gui, debug, "view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pNativeView->winId();
}


void
ViewImpl::setNativeView(QWidget* pView)
{
//    LOG(gui, debug, "view impl set native view: " + Poco::NumberFormatter::format(pView));
    _pNativeView = pView;
}


void
ViewImpl::setParent(View* pView)
{
    _pNativeView->setParent(static_cast<QWidget*>(pView->getNativeView()));
}


void
ViewImpl::showView(bool async)
{
//    LOG(gui, debug, "view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        _pSignalProxy->showView();
    }
    else {
        _pNativeView->show();
    }
//    LOG(gui, debug, "view impl show finished.");
}


void
ViewImpl::hideView(bool async)
{
//    LOG(gui, debug, "view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    if (async) {
        _pSignalProxy->hideView();
    }
    else {
        _pNativeView->hide();
    }
//    LOG(gui, debug, "view impl hide finished.");
}


void
ViewImpl::raise(bool async)
{
    if (async) {
        LOG(gui, debug, "view impl raise async");
        _pSignalProxy->raiseView();
    }
    else {
        LOG(gui, debug, "view impl raise");
        _pNativeView->raise();
    }
}


bool
ViewImpl::isVisible()
{
    return _pNativeView->isVisible();
}


int
ViewImpl::posXView()
{
    return _pNativeView->x();
}


int
ViewImpl::posYView()
{
    return _pNativeView->y();
}


int
ViewImpl::widthView()
{
//    LOG(gui, debug, "view impl width.");
    return _pNativeView->width();
}


int
ViewImpl::heightView()
{
//    LOG(gui, debug, "view impl height.");
    return _pNativeView->height();
}


void
ViewImpl::resizeView(int width, int height)
{
//    LOG(gui, debug, "view impl resize.");
    _pNativeView->resize(width, height);
}


//void
//ViewImpl::setSizeConstraint(int width, int height, View::SizeConstraint size)
//{
//    switch(size) {
//        case View::Min:
////            _pNativeView->setMinimumSize(width, height);
//            break;
//        case View::Pref:
//            break;
//        case View::Max:
////            _pNativeView->setMaximumSize(width, height);
//            break;
//    }
//}


float
ViewImpl::getFontSize()
{
    return _pNativeView->font().pointSizeF();
}


void
ViewImpl::setFontSize(float fontSize)
{
    QFont font = _pNativeView->font();
    font.setPointSizeF(fontSize);
    _pNativeView->setFont(font);
}


void
ViewImpl::moveView(int x, int y)
{
//    LOG(gui, debug, "view impl move.");
    _pNativeView->move(x, y);
}


void
ViewImpl::setHighlighted(bool highlighted)
{
//    LOG(gui, debug, "view impl set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
    if (highlighted) {
        _pNativeView->setBackgroundRole(QPalette::Highlight);
    }
    else {
        _pNativeView->setBackgroundRole(QPalette::Window);
    }
}


void
ViewImpl::setBackgroundColor(const Color& color)
{
    _pNativeView->setPalette(QPalette(*static_cast<QColor*>(color._pImpl->getNativeColor())));
}


bool
ViewImpl::getEnableRedraw()
{
    return _pNativeView->updatesEnabled();
}


void
ViewImpl::setEnableRedraw(bool enable)
{
    _pNativeView->setUpdatesEnabled(enable);
}


void
ViewImpl::setAcceptDrops(bool accept)
{
    _pNativeView->setAcceptDrops(accept);
}


int
ViewImpl::getDragMode()
{
    return _pView->_dragMode;
}


void
ViewImpl::setEnableHover(bool enable)
{
    _pNativeView->setMouseTracking(enable);
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
    LOG(gui, debug, "view impl selected view: " + _pView->getName());
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
ViewImpl::keyPressed(int key)
{
    LOG(gui, debug, "view impl key pressed: " + Poco::NumberFormatter::format(key));
    IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, keyTranslateNativeCode(key));
}


bool
ViewImpl::keyPressedNonFullscreen(int key, int modifiers)
{
    LOG(gui, debug, "view impl key pressed non fullscreen: " + Poco::NumberFormatter::format(key) + ", modifiers: " + Poco::NumberFormatter::formatHex(modifiers));
    bool propagate = true;
    IMPL_NOTIFY_CONTROLLER(Controller, keyPressedNonFullscreen, keyTranslateNativeCode(key), keyTranslateNativeModifiers(modifiers), propagate);
    return propagate;
}


Controller::KeyCode
ViewImpl::keyTranslateNativeCode(int key)
{
    switch (key) {
        case Qt::Key_Menu:
        case Qt::Key_HomePage:
            return Controller::KeyMenu;
        case Qt::Key_Backspace:
        case Qt::Key_Backtab:
            return Controller::KeyBack;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Select:
        case Qt::Key_Yes:
            return Controller::KeyReturn;
        case Qt::Key_Left:
            return Controller::KeyLeft;
        case Qt::Key_Right:
            return Controller::KeyRight;
        case Qt::Key_Up:
            return Controller::KeyUp;
        case Qt::Key_Down:
            return Controller::KeyDown;
        case Qt::Key_Back:
            return Controller::KeyBack;
        case Qt::Key_Forward:
            return Controller::KeyForward;
        case Qt::Key_Stop:
            return Controller::KeyStop;
        case Qt::Key_VolumeDown:
            return Controller::KeyVolDown;
        case Qt::Key_VolumeMute:
            return Controller::KeyMute;
        case Qt::Key_VolumeUp:
            return Controller::KeyVolUp;
        case Qt::Key_MediaPlay:
        case Qt::Key_Play:
            return Controller::KeyPlay;
        case Qt::Key_MediaStop:
            return Controller::KeyStop;
        case Qt::Key_MediaPrevious:
            return Controller::KeyBackward;
        case Qt::Key_MediaNext:
            return Controller::KeyForward;
        case Qt::Key_MediaRecord:
            return Controller::KeyRecord;
        case Qt::Key_A:
            return Controller::KeyA;
        case Qt::Key_B:
            return Controller::KeyB;
        case Qt::Key_C:
            return Controller::KeyC;
        case Qt::Key_D:
            return Controller::KeyD;
        case Qt::Key_E:
            return Controller::KeyE;
        case Qt::Key_F:
            return Controller::KeyF;
        case Qt::Key_G:
            return Controller::KeyG;
        case Qt::Key_H:
            return Controller::KeyH;
        case Qt::Key_I:
            return Controller::KeyI;
        case Qt::Key_J:
            return Controller::KeyJ;
        case Qt::Key_K:
            return Controller::KeyK;
        case Qt::Key_L:
            return Controller::KeyL;
        case Qt::Key_M:
            return Controller::KeyM;
        case Qt::Key_N:
            return Controller::KeyN;
        case Qt::Key_O:
            return Controller::KeyO;
        case Qt::Key_P:
            return Controller::KeyP;
        case Qt::Key_Q:
            return Controller::KeyQ;
        case Qt::Key_R:
            return Controller::KeyR;
        case Qt::Key_S:
            return Controller::KeyS;
        case Qt::Key_T:
            return Controller::KeyT;
        case Qt::Key_U:
            return Controller::KeyU;
        case Qt::Key_V:
            return Controller::KeyV;
        case Qt::Key_W:
            return Controller::KeyW;
        case Qt::Key_X:
            return Controller::KeyX;
        case Qt::Key_Y:
            return Controller::KeyY;
        case Qt::Key_Z:
            return Controller::KeyZ;
#if (QT_VERSION & 0xFFFFFFFF) >= 0x040700
        case Qt::Key_MediaPause:
            return Controller::KeyPause;
        case Qt::Key_MediaTogglePlayPause:
            return Controller::KeyPlayPause;
        case Qt::Key_PowerOff:
            return Controller::KeyPowerOff;
        case Qt::Key_WakeUp:
            return Controller::KeyWakeUp;
        case Qt::Key_Eject:
            return Controller::KeyEject;
        case Qt::Key_MediaLast:
            return Controller::KeyLast;
#endif
    }
}


Controller::Modifiers
ViewImpl::keyTranslateNativeModifiers(int modifiers)
{
//    Controller::Modifiers res = Controller::NoModifier;
    int res = Controller::NoModifier;

    if (modifiers & Qt::ShiftModifier) {
        res |= Controller::ShiftModifier;
    }
    else if (modifiers & Qt::ControlModifier) {
        res |= Controller::ControlModifier;
    }
    else if (modifiers & Qt::AltModifier) {
        res |= Controller::AltModifier;
    }
    else if (modifiers & Qt::MetaModifier) {
        res |= Controller::MetaModifier;
    }
    else if (modifiers & Qt::KeypadModifier) {
        res |= Controller::KeypadModifier;
    }
    else if (modifiers & Qt::GroupSwitchModifier) {
        res |= Controller::GroupSwitchModifier;
    }

//    return res;
    return (Controller::Modifiers)res;
}


void
ViewImpl::mouseHovered(const Position& pos)
{
    LOG(gui, debug, "view impl mouse hovered in view: " + _pView->getName() + " [" + Poco::NumberFormatter::format(pos.x()) + ", " + Poco::NumberFormatter::format(pos.y()) + "]");
    IMPL_NOTIFY_CONTROLLER(Controller, mouseHovered, pos);
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
    initViewImpl(pView, new QtViewImpl<QWidget>(this));
}


}  // namespace Omm
}  // namespace Gui
