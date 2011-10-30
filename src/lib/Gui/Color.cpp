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
#include "ColorImpl.h"


namespace Omm {
namespace Gui {


Color::Color(const std::string& colorName)
{
//    Omm::Gui::Log::instance()->gui().debug("color ctor.");
    if (colorName == "white") {
        _pImpl = new ColorImpl(255, 255, 255, 255);
    }
    else if (colorName == "blue") {
        _pImpl = new ColorImpl(0, 0, 255, 255);
    }
    else if (colorName == "lightBlue") {
        _pImpl = new ColorImpl(128, 204, 255, 255);
    }
    else if (colorName == "black") {
        _pImpl = new ColorImpl(0, 0, 0, 255);
    }
}


void*
Color::getNativeColor() const
{
    return _pImpl->getNativeColor();
}

} // namespace Gui
} // namespace Omm
