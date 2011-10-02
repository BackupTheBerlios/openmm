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

#include "Gui/Color.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ColorImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/ColorImpl.h"
#endif


namespace Omm {
namespace Gui {


Color::Color(const std::string& colorName)
{
//    Omm::Gui::Log::instance()->gui().debug("color ctor.");
    _pImpl = new ColorImpl(colorName);
}


void*
Color::getNativeColor() const
{
    return _pImpl->getNativeColor();
}

} // namespace Gui
} // namespace Omm
