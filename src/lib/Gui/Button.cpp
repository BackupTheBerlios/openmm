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

#include "Gui/Button.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ButtonImpl.h"
#endif


namespace Omm {
namespace Gui {


Button::Button(Widget* pParent) :
Widget(new ButtonImpl(this, pParent), pParent)
{
    Omm::Gui::Log::instance()->gui().debug("button ctor.");
}


Button::~Button()
{
}


void
Button::setLabel(const std::string& label)
{
    Omm::Gui::Log::instance()->gui().debug("button set label");
    static_cast<ButtonImpl*>(_pImpl)->setLabel(label);
}


Button::PushNotification::PushNotification()
{
}


} // namespace Gui
} // namespace Omm
