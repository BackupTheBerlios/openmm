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

#include "ColorImpl.h"
#include "Gui/Color.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {


ColorImpl::ColorImpl(const std::string& colorName)
{
    Omm::Gui::Log::instance()->gui().debug("Color impl ctor");

    if (colorName == "white") {
        _pQtColor = new QColor(255, 255, 255, 255);
    }
    else if (colorName == "blue") {
        _pQtColor = new QColor(0, 0, 255, 255);
    }
    else if (colorName == "lightBlue") {
        _pQtColor = new QColor(128, 204, 255, 255);
    }
}


QColor*
ColorImpl::getNativeColor() const
{
    return _pQtColor;
}


}  // namespace Omm
}  // namespace Gui
