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

#include "Gui/Splitter.h"
#include "Gui/GuiLogger.h"
#include "SplitterImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


SplitterView::SplitterView(View* pParent, Orientation orientation) :
View(pParent, false)
{
//    LOG(gui, debug, "Splitter view ctor.");
    setName("splitter view");

    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 20;
    _pImpl = new SplitterViewImpl(this, orientation);
}


void
SplitterView::setOrientation(Orientation orientation)
{
    static_cast<SplitterViewImpl*>(_pImpl)->setOrientation(orientation);
}


} // namespace Gui
} // namespace Omm
