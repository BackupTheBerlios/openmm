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

#include "Gui/Widget.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/WidgetImpl.h"
#endif


namespace Omm {
namespace Gui {


Widget::Widget(Widget* pParent) :
_pImpl(new WidgetImpl(this)),
_pParent(pParent)
{
    Omm::Gui::Log::instance()->gui().debug("widget ctor (parent).");
}


Widget::Widget(WidgetImpl* pWidgetImpl, Widget* pParent) :
_pImpl(pWidgetImpl),
_pParent(pParent)
{
    Omm::Gui::Log::instance()->gui().debug("widget ctor (widget impl, parent).");
}


Widget::~Widget()
{
    if (_pImpl) {
//        delete _pImpl;
    }
}


void*
Widget::getNativeWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget get native widget, impl:" + Poco::NumberFormatter::format(_pImpl));
    return _pImpl->getNativeWidget();
}


Widget*
Widget::getParent()
{
    Omm::Gui::Log::instance()->gui().debug("widget get parent: " + Poco::NumberFormatter::format(_pParent));
    return _pParent;
}


void
Widget::show()
{
    Omm::Gui::Log::instance()->gui().debug("widget show widget ...");
     _pImpl->showWidget();
    Omm::Gui::Log::instance()->gui().debug("widget show widget finished.");
}


void
Widget::hide()
{
    Omm::Gui::Log::instance()->gui().debug("widget hide widget.");
    _pImpl->hideWidget();
}


void
Widget::resize(int width, int height)
{
    Omm::Gui::Log::instance()->gui().debug("widget resize widget.");
    _pImpl->resizeWidget(width, height);
}


void
Widget::connect(const Poco::AbstractObserver& observer)
{
    Omm::Gui::Log::instance()->gui().debug("widget register notification handler.");
    _eventNotificationCenter.addObserver(observer);
}


void
Widget::select()
{
    Omm::Gui::Log::instance()->gui().debug("widget select.");
    _eventNotificationCenter.postNotification(new SelectNotification);
}


} // namespace Gui
} // namespace Omm
