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

    // assign subviews to rows
    int currentRow = 0;
    std::vector<int> childRow;

    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it) {
        minChildWidth += (*it)->width(View::Min);
        minChildHeight = std::max((*it)->height(View::Min), minChildHeight);
        prefChildWidth += (*it)->width(View::Pref);
        prefChildHeight = std::max((*it)->height(View::Pref), prefChildHeight);
        maxChildWidth += (*it)->width(View::Max);
        maxChildHeight = std::max((*it)->height(View::Max), maxChildHeight);

//        if (minChildWidth > _pView->width()) {
//            currentRow++;
//            minChildWidth = (*it)->width(View::Min);
//        }
        if (prefChildWidth > _pView->width()) {
            currentRow++;
            prefChildWidth = (*it)->width(View::Pref);
        }
        childRow.push_back(currentRow);
    }

    // stretch subviews, so that they fit exactly into each row
    float childWidthSum = 0.0;
    std::vector<float> stretchFactor;
    int i = 0;
    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it, ++i) {
        if (i > 0 && (childRow[i] != childRow[i-1])) {
            stretchFactor.push_back(_pView->width() / childWidthSum);
            // "newline"
            childWidthSum = 0.0;
        }
        childWidthSum += (*it)->width(View::Pref) * (1 + (*it)->stretchFactor());
    }
    stretchFactor.push_back(_pView->width() / childWidthSum);

    int childWidth = 0;
//    int childHeight = _pView->height() / (currentRow + 1);
    int childHeight = prefChildHeight;

    // resize super view
    _pView->resizeNoLayout(_pView->width(), (currentRow + 1) * childHeight);

    // layout the subviews by moving and resizing them
    i = 0;
    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it, ++i) {
        int width = (*it)->width(View::Pref) * (1 + (*it)->stretchFactor()) * stretchFactor[childRow[i]];
        if (i > 0 && (childRow[i] != childRow[i-1])) {
            // "newline"
            childWidth = 0;
        }
        (*it)->move(childWidth, childRow[i] * childHeight);
        (*it)->resize(width, childHeight);
        childWidth += width;
    }
}


void
HorizontalLayout::layoutViewEquiDistant()
{
    int childWidth = _pView->width() / _pView->childCount();
    int childHeight = _pView->height();

    for (View::ChildIterator it = _pView->beginChild(); it != _pView->endChild(); ++it) {
        (*it)->resize(childWidth, childHeight);
        (*it)->move((it - _pView->beginChild()) * (*it)->width(), 0);
    }

    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view finished.");
}


} // namespace Gui
} // namespace Omm
