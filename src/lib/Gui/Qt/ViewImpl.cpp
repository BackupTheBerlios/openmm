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

#include "ViewImpl.h"
#include "Gui/View.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtEventFilter : public QObject
{
public:
    QtEventFilter(ViewImpl* pViewImpl) : _pViewImpl(pViewImpl)
    {
    }

private:
    virtual bool eventFilter(QObject* object, QEvent* event)
    {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* pKeyEvent = static_cast<QKeyEvent*>(event);
            _pViewImpl->keyPressed(pKeyEvent->key());
        }
    }

    ViewImpl*      _pViewImpl;
};


ViewImpl::~ViewImpl()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl dtor");
//    delete _pNativeView;
}


void
ViewImpl::initViewImpl(View* pView, QWidget* pNative)
{
    _pView = pView;
    _pNativeView = pNative;
    connect(this, SIGNAL(showViewSignal()), _pNativeView, SLOT(show()));
    connect(this, SIGNAL(hideViewSignal()), _pNativeView, SLOT(hide()));
    connect(this, SIGNAL(triggerViewSyncSignal()), this, SLOT(triggerViewSyncSlot()));
    _pNativeView->setAutoFillBackground(true);

    _pEventFilter = new QtEventFilter(this);
    _pNativeView->installEventFilter(_pEventFilter);

    if (pView->getParent()) {
        QWidget* pParentWidget = static_cast<QWidget*>(pView->getParent()->getNativeView());
        _pNativeView->resize(pParentWidget->size());
        _pNativeView->setParent(pParentWidget);
    }
    else {
        _pNativeView->resize(250, 400);
    }
}


void
ViewImpl::triggerViewSync()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl trigger view sync");
    emit triggerViewSyncSignal();
}


void
ViewImpl::triggerViewSyncSlot()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl trigger view sync slot");
    _pView->syncViewImpl();
}


View*
ViewImpl::getView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl get view: " + Poco::NumberFormatter::format(_pView));
    return _pView;
}


QWidget*
ViewImpl::getNativeView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl get native view: " + Poco::NumberFormatter::format(_pNativeView));
    return _pNativeView;
}


WId
ViewImpl::getNativeWindowId()
{
//    Omm::Gui::Log::instance()->gui().debug("view get native view, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pNativeView->winId();
}


void
ViewImpl::setNativeView(QWidget* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl set native view: " + Poco::NumberFormatter::format(pView));
    _pNativeView = pView;
}


void
ViewImpl::addSubview(View* pView)
{
    static_cast<QWidget*>(pView->getNativeView())->setParent(_pNativeView);
}


void
ViewImpl::showView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl show _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    emit showViewSignal();
//    Omm::Gui::Log::instance()->gui().debug("view impl show finished.");
}


void
ViewImpl::hideView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl hide _pNativeView: " + Poco::NumberFormatter::format(_pNativeView) + " ...");
    emit hideViewSignal();
//    Omm::Gui::Log::instance()->gui().debug("view impl hide finished.");
}


int
ViewImpl::widthView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl width.");
    return _pNativeView->width();
}


int
ViewImpl::heightView()
{
//    Omm::Gui::Log::instance()->gui().debug("view impl height.");
    return _pNativeView->height();
}


void
ViewImpl::resizeView(int width, int height)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl resize.");
    _pNativeView->resize(width, height);
}


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
//    Omm::Gui::Log::instance()->gui().debug("view impl move.");
    _pNativeView->move(x, y);
}


void
ViewImpl::setHighlighted(bool highlighted)
{
//    Omm::Gui::Log::instance()->gui().debug("view impl set highlighted: " + (highlighted ? std::string("true") : std::string("false")));
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
    _pNativeView->setPalette(QPalette(*static_cast<QColor*>(color.getNativeColor())));
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


void
ViewImpl::keyPressed(int key)
{
    Omm::Gui::Log::instance()->gui().debug("view impl key pressed: " + Poco::NumberFormatter::format(key));
    switch (key) {
        case Qt::Key_M:
        case Qt::Key_Menu:
        case Qt::Key_HomePage:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyMenu);
            break;
        case Qt::Key_Backspace:
        case Qt::Key_Backtab:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyBack);
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Select:
        case Qt::Key_Yes:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyReturn);
            break;
        case Qt::Key_Left:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyLeft);
            break;
        case Qt::Key_Right:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyRight);
            break;
        case Qt::Key_Up:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyUp);
            break;
        case Qt::Key_Down:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyDown);
            break;
        case Qt::Key_Back:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyBack);
            break;
        case Qt::Key_Forward:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyForward);
            break;
        case Qt::Key_Stop:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyStop);
            break;
        case Qt::Key_VolumeDown:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyVolDown);
            break;
        case Qt::Key_VolumeMute:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyMute);
            break;
        case Qt::Key_VolumeUp:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyVolUp);
            break;
        case Qt::Key_MediaPlay:
        case Qt::Key_Play:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyPlay);
            break;
        case Qt::Key_MediaStop:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyStop);
            break;
        case Qt::Key_MediaPrevious:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyBackward);
            break;
        case Qt::Key_MediaNext:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyForward);
            break;
        case Qt::Key_MediaRecord:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyRecord);
            break;
#if (QT_VERSION & 0xFFFFFFFF) > 0x040500
        case Qt::Key_MediaPause:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyPause);
            break;
        case Qt::Key_MediaTogglePlayPause:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyPlayPause);
            break;
        case Qt::Key_PowerOff:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyPowerOff);
            break;
        case Qt::Key_WakeUp:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyWakeUp);
            break;
        case Qt::Key_Eject:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyEject);
            break;
        case Qt::Key_MediaLast:
            IMPL_NOTIFY_CONTROLLER(Controller, keyPressed, Controller::KeyLast);
            break;
#endif
    }
}


PlainViewImpl::PlainViewImpl(View* pView)
{
    initViewImpl(pView, new QtViewImpl<QWidget>(this));
}


}  // namespace Omm
}  // namespace Gui
