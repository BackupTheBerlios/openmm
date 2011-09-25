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

#include <vector>

#include "Gui/HorizontalLayout.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"


namespace Omm {
namespace Gui {


void
HorizontalLayout::layoutView()
{
//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view ...");
    
    int minChildWidth = 0;
    int minChildHeight = 0;
    int prefChildWidth = 0;
    int prefChildHeight = 0;
    int maxChildWidth = 0;
    int maxChildHeight = 0;

    int currentRow = 0;
    std::vector<int> childRow;

    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it) {
        minChildWidth += (*it)->width(View::Min);
        minChildHeight += (*it)->height(View::Min);
        prefChildWidth += (*it)->width(View::Pref);
        prefChildHeight += (*it)->height(View::Pref);
        maxChildWidth += (*it)->width(View::Max);
        maxChildHeight += (*it)->height(View::Max);

        if (minChildWidth > _pView->width()) {
            currentRow++;
            minChildWidth = (*it)->width(View::Min);
        }
        childRow.push_back(currentRow);
    }

    int childWidth = 0;
    int childHeight = _pView->height() / (currentRow + 1);

    int i = 0;
    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it, ++i) {
        int width = (*it)->width(View::Min);
        
        if (i > 0 && (childRow[i] != childRow[i-1])) {
            // "newline"
            childWidth = 0;
        }

        (*it)->move(childWidth, childRow[i] * childHeight);
        (*it)->resize(width, childHeight);
        childWidth += width;
    }

//    int childWidth = _pView->width() / _pView->childCount();
//    int childHeight = _pView->height();

//    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it) {
//        (*it)->resize(childWidth, childHeight);
//        (*it)->move((it - _pView->beginChild()) * (*it)->width(), 0);
//    }

//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view finished.");
}


} // namespace Gui
} // namespace Omm
