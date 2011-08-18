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


Widget::Widget(Widget* pParent)
{
    if (!pParent) {
        return;
    }
    Omm::Gui::Log::instance()->gui().debug("widget ctor ...");
    _pImpl = new WidgetImpl(pParent);
    static_cast<WidgetImpl*>(_pImpl)->_pWidget = this;
    Omm::Gui::Log::instance()->gui().debug("widget ctor finished.");
}


Widget::~Widget()
{
    delete _pImpl;
}


void*
Widget::getNativeWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget get native widget.");
    return _pImpl->getNativeWidget();
}


void
Widget::showWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget show widget ...");
     _pImpl->showWidget();
    Omm::Gui::Log::instance()->gui().debug("widget show widget finished.");
}


void
Widget::hideWidget()
{
    Omm::Gui::Log::instance()->gui().debug("widget hide widget.");
     _pImpl->hideWidget();
}


Widget::SelectNotification::SelectNotification()
{
}


void
Widget::registerEventNotificationHandler(const Poco::AbstractObserver& observer)
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
