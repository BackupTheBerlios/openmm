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

#include "ButtonImpl.h"
#include "Gui/Button.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ButtonImpl::ButtonImpl(Widget* pParent) :
QPushButton(static_cast<QWidget*>(pParent->getNativeWidget())),
WidgetImpl(this)
{
    connect(this, SIGNAL(pressed()), this, SLOT(pushed()));
}


void
ButtonImpl::setLabel(const std::string& label)
{
    QPushButton::setText(QString::fromStdString(label));
}


void
ButtonImpl::pushed()
{
    Omm::Gui::Log::instance()->gui().debug("button implementation, button pushed, sending notification to widget: " + Poco::NumberFormatter::format(_pWidget) + "...");
    _pWidget->_eventNotificationCenter.postNotification(new Button::PushNotification);
    Omm::Gui::Log::instance()->gui().debug("button implementation, button pushed, notification sent.");
}


}  // namespace Omm
}  // namespace Gui
