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
//    LOG(gui, debug, "color ctor.");
    if (colorName == "white") {
        _pImpl = new ColorImpl(255, 255, 255, 255);
    }
    else if (colorName == "red") {
        _pImpl = new ColorImpl(255, 0, 0, 255);
    }
    else if (colorName == "green") {
        _pImpl = new ColorImpl(0, 255, 0, 255);
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
    else if (colorName == "grey") {
        _pImpl = new ColorImpl(120, 120, 120, 255);
    }
}


Color::Color(int red, int green, int blue, int alpha)
{
    _pImpl = new ColorImpl(red, green, blue, alpha);
}


void*
Color::getNativeColor() const
{
    return _pImpl->getNativeColor();
}

} // namespace Gui
} // namespace Omm
