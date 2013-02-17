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

#include "Gui/Switch.h"
#include "Gui/GuiLogger.h"
#include "SwitchImpl.h"


namespace Omm {
namespace Gui {

SwitchView::SwitchView(View* pParent) :
View(pParent, false)
{
    setName("switch view");

    _minWidth = 30;
    _minHeight = 10;
    _prefWidth = 20;
    _prefHeight = 20;
    _pImpl = new SwitchViewImpl(this);
}


bool
SwitchView::getStateOn()
{
    return static_cast<SwitchViewImpl*>(_pImpl)->getStateOn();
}


void
SwitchView::setState(bool on)
{
    static_cast<SwitchViewImpl*>(_pImpl)->setState(on);
}

} // namespace Gui
} // namespace Omm
