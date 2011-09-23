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

#include "Gui/HorizontalLayout.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"


namespace Omm {
namespace Gui {


void
HorizontalLayout::layoutView()
{
//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view ...");

    int childWidth = _pView->width() / _pView->childCount();
    int childHeight = _pView->height();

    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it) {
        (*it)->resize(childWidth, childHeight);
        (*it)->move((it - _pView->beginChild()) * (*it)->width(), 0);
    }

//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view finished.");
}


} // namespace Gui
} // namespace Omm
