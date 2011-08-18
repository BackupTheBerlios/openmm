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



WidgetImpl::WidgetImpl(Widget* pParent)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor (parent) ...");
    _pNativeWidget = new NativeWidget(this, pParent);
    static_cast<NativeWidget*>(_pNativeWidget)->_pWidgetImpl = this;
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor finished.");
}


WidgetImpl::WidgetImpl(QWidget* pNativeWidget)
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor (native widget) ...");
    _pNativeWidget = pNativeWidget;
    Omm::Gui::Log::instance()->gui().debug("widget implementation ctor finished.");
}


WidgetImpl::~WidgetImpl()
{
    delete _pNativeWidget;
}


QWidget*
WidgetImpl::getNativeWidget()
{
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
    Omm::Gui::Log::instance()->gui().debug("widget implementation show widget ...");
    Omm::Gui::Log::instance()->gui().debug("widget implementation show widget _pNativeWidget: " + Poco::NumberFormatter::format(_pNativeWidget));
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
WidgetImpl::select()
{
    Omm::Gui::Log::instance()->gui().debug("widget implementation select.");
    _pWidget->select();
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
