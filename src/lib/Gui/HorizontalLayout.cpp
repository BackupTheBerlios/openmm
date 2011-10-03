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
//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, lay out view ...");
    
    int minSubviewWidth = 0;
    int minSubviewHeight = 0;
    int prefSubviewWidth = 0;
    int prefSubviewHeight = 0;
    int maxSubviewWidth = 0;
    int maxSubviewHeight = 0;

    // assign subviews to rows
    int currentRow = 0;
    std::vector<int> subviewRow;

    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it) {
        minSubviewWidth += (*it)->width(View::Min);
        minSubviewHeight = std::max((*it)->height(View::Min), minSubviewHeight);
        prefSubviewWidth += (*it)->width(View::Pref);
        prefSubviewHeight = std::max((*it)->height(View::Pref), prefSubviewHeight);
        maxSubviewWidth += (*it)->width(View::Max);
        maxSubviewHeight = std::max((*it)->height(View::Max), maxSubviewHeight);

        if (prefSubviewWidth > _pView->width()) {
            currentRow++;
            prefSubviewWidth = (*it)->width(View::Pref);
        }
        subviewRow.push_back(currentRow);
    }

    // stretch subviews, so that they fit exactly into each row
    float subviewWidthSum = 0.0;
    std::vector<float> stretchFactor;
    int i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
        if (i > 0 && (subviewRow[i] != subviewRow[i-1])) {
            stretchFactor.push_back(_pView->width() / subviewWidthSum);
            // "newline"
            subviewWidthSum = 0.0;
        }
        subviewWidthSum += (*it)->width(View::Pref) * (1 + (*it)->stretchFactor());
    }
    stretchFactor.push_back(_pView->width() / subviewWidthSum);

    int subviewWidth = 0;
    int subviewHeight = prefSubviewHeight;

    // resize super view
    _pView->resizeNoLayout(_pView->width(), (currentRow + 1) * subviewHeight);

    // layout the subviews by moving and resizing them
    i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
        int width = (*it)->width(View::Pref) * (1 + (*it)->stretchFactor()) * stretchFactor[subviewRow[i]];
        if ((*it)->stretchFactor() == -1.0) {
            width = (*it)->width(View::Pref);
        }
        if (i > 0 && (subviewRow[i] != subviewRow[i-1])) {
            // "newline"
            subviewWidth = 0;
        }
        (*it)->move(subviewWidth, subviewRow[i] * subviewHeight);
        (*it)->resize(width, subviewHeight);
        subviewWidth += width;
    }
//    Omm::Gui::Log::instance()->gui().debug("horizontal layout, lay out view finished.");
}


void
HorizontalLayout::layoutViewEquiDistant()
{
    int subviewWidth = _pView->width() / _pView->subviewCount();
    int subviewHeight = _pView->height();

    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it) {
        (*it)->resize(subviewWidth, subviewHeight);
        (*it)->move((it - _pView->beginSubview()) * (*it)->width(), 0);
    }

    Omm::Gui::Log::instance()->gui().debug("horizontal layout, laying out view finished.");
}


} // namespace Gui
} // namespace Omm
