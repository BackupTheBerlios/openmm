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

#import <UIKit/UIKit.h>

#include <Poco/NumberFormatter.h>

#include "ColorImpl.h"
#include "Gui/Color.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {


ColorImpl::ColorImpl(int red, int green, int blue, int alpha)
{
//    LOG(gui, debug, "Color impl ctor");
    _pUIColor = [UIColor colorWithRed:red/255.0 green:green/255.0 blue:blue/255.0 alpha:alpha/255.0];
}


void*
ColorImpl::getNativeColor() const
{
    return _pUIColor;
}


}  // namespace Omm
}  // namespace Gui
