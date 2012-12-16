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
    // stretch subviews, so that they fit exactly into each row
    int flexHeightSum = 0.0;
    int fixedHeightSum = 0.0;
    int i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
        if ((*it)->stretchFactor() == -1.0) {
            fixedHeightSum += (*it)->height(View::Pref);
        }
        else {
            flexHeightSum += (*it)->height(View::Pref);
        }
    }
    float subviewStretchFactor = 1.0;
    if (_pView->height() >= fixedHeightSum) {
        subviewStretchFactor = (float)(_pView->height() - fixedHeightSum) / flexHeightSum;
    }
    else {
        subviewStretchFactor = 0.0;
    }

    int subviewHeight = 0;

    // layout the subviews by moving and resizing them
    i = 0;
    for (View::SubviewIterator it = _pView->beginSubview(); it != _pView->endSubview(); ++it, ++i) {
//        int width = (*it)->width(View::Pref) * subviewStretchFactor * (1 + (*it)->stretchFactor());
        int height = (*it)->height(View::Pref) * subviewStretchFactor * (*it)->stretchFactor();
        if ((*it)->stretchFactor() == -1.0) {
//            width = (*it)->width(View::Pref) * subviewStretchFactor;
            height = (*it)->height(View::Pref);
        }
        (*it)->move(0, subviewHeight);
        (*it)->resize(_pView->width(), height);
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
