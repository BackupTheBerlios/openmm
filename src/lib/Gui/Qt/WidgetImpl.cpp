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

#include "WidgetImpl.h"
#include "Gui/Widget.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


WidgetImpl::WidgetImpl(Widget* pWidget) :
_pNativeWidget(new NativeWidget(this, pWidget->getParent())),
_pWidget(pWidget)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor (widget).");
}


WidgetImpl::WidgetImpl(QWidget* pNativeWidget) :
_pNativeWidget(pNativeWidget)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor (native widget).");
}


WidgetImpl::WidgetImpl(Widget* pWidget, QWidget* pNativeWidget) :
_pWidget(pWidget),
_pNativeWidget(pNativeWidget)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor (widget, native widget)");
}


WidgetImpl::~WidgetImpl()
{
    delete _pNativeWidget;
}


Widget*
WidgetImpl::getWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation get widget: " + Poco::NumberFormatter::format(_pWidget));
    return _pWidget;
}


QWidget*
WidgetImpl::getNativeWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation get native widget: " + Poco::NumberFormatter::format(_pNativeWidget));
    return _pNativeWidget;
}


void
WidgetImpl::setNativeWidget(QWidget* pWidget)
{
    _pNativeWidget = pWidget;
}


void
WidgetImpl::showWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation show widget _pNativeWidget: " + Poco::NumberFormatter::format(_pNativeWidget) + " ...");
    _pNativeWidget->show();
    Omm::Gui::Log::instance()->gui().debug("widget implementation show widget finished.");
}


void
WidgetImpl::hideWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation hide.");
    _pNativeWidget->hide();
}


void
WidgetImpl::resizeWidget(int width, int height)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation resize.");
    _pNativeWidget->resize(width, height);
}


void
WidgetImpl::select()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation select.");
    _pWidget->select();
}


void
WidgetImpl::postNotification(Poco::Notification::Ptr pNotification)
{
    _pWidget->_eventNotificationCenter.postNotification(pNotification);
}


NativeWidget::NativeWidget(WidgetImpl* pWidgetImpl, Widget* pParent) :
QWidget(static_cast<QWidget*>(pParent->getNativeWidget())),
_pWidgetImpl(pWidgetImpl)
{
    Omm::Gui::Log::instance()->gui().debug("native widget implementation ctor.");
}

void
NativeWidget::mousePressEvent(QMouseEvent* pMouseEvent)
{
    _pWidgetImpl->select();
    QWidget::mousePressEvent(pMouseEvent);
}


}  // namespace Omm
}  // namespace Gui
