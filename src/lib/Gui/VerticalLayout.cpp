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

#include "Gui/VerticalLayout.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"
#include "Log.h"


namespace Omm {
namespace Gui {


void
VerticalLayout::layoutView()
{
    LOG(gui, debug, "vertical layout, lay out view ...");

    int minSubviewWidth = 0;
    int minSubviewHeight = 0;
    int prefSubviewWidth = 0;
    int prefSubviewHeight = 0;
    int maxSubviewWidth = 0;
    int maxSubviewHeight = 0;

    // assign subviews to rows
    int currentCol = 0;
    std::vector<int> subviewCol;

    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it) {
        minSubviewWidth += (*it)->width(View::Min);
        minSubviewHeight = std::max((*it)->height(View::Min), minSubviewHeight);
        prefSubviewWidth += (*it)->width(View::Pref);
        prefSubviewHeight = std::max((*it)->height(View::Pref), prefSubviewHeight);
        maxSubviewWidth += (*it)->width(View::Max);
        maxSubviewHeight = std::max((*it)->height(View::Max), maxSubviewHeight);

        if (prefSubviewHeight > _pView->height()) {
            currentCol++;
            prefSubviewHeight = (*it)->height(View::Pref);
        }
        subviewCol.push_back(currentCol);
    }

    // stretch subviews, so that they fit exactly into each col
    float subviewHeightSum = 0.0;
    std::vector<float> stretchFactor;
    int i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
        if (i > 0 && (subviewCol[i] != subviewCol[i-1])) {
            stretchFactor.push_back(_pView->height() / subviewHeightSum);
            // "newline"
            subviewHeightSum = 0.0;
        }
        subviewHeightSum += (*it)->height(View::Pref) * (1 + (*it)->stretchFactor());
    }
    stretchFactor.push_back(_pView->height() / subviewHeightSum);

    int subviewWidth = prefSubviewWidth;
    int subviewHeight = 0;

    // resize super view
    _pView->resizeNoLayout((currentCol + 1) * subviewWidth, _pView->height());

    // layout the subviews by moving and resizing them
    i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
        int height = (*it)->height(View::Pref) * (1 + (*it)->stretchFactor()) * stretchFactor[subviewCol[i]];
        if ((*it)->stretchFactor() == -1.0) {
            height = (*it)->height(View::Pref);
        }
        if (i > 0 && (subviewCol[i] != subviewCol[i-1])) {
            // "newline"
            subviewHeight = 0;
        }
        (*it)->move(subviewCol[i] * subviewWidth, subviewHeight);
        (*it)->resize(subviewWidth, height);
        subviewHeight += height;
    }
//    LOG(gui, debug, "vertical layout, lay out view finished.");
}


void
VerticalLayout::layoutViewEquiDistant()
{
    int subviewWidth = _pView->width();
    int subviewHeight = _pView->height() / _pView->subviewCount();

    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it) {
        (*it)->resize(subviewWidth, subviewHeight);
        (*it)->move(0, (it - _pView->beginSubview()) * (*it)->height());
    }

    LOG(gui, debug, "vertical layout, laying out view finished.");
}


} // namespace Gui
} // namespace Omm
