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

#include <algorithm>

#include "Gui/Controller.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"
#include "Gui/Model.h"


namespace Omm {
namespace Gui {


void
Controller::attachModel(Model* pModel)
{
//    LOG(gui, debug, "controller attach model ...");
    _models.push_back(pModel);
//    LOG(gui, debug, "controller attach model finished.");
}


void
Controller::detachModel(Model* pModel)
{
//    LOG(gui, debug, "controller detach model ...");
    ModelIterator pos = std::find(beginModel(), beginModel(), pModel);
    if (pos != _models.end()) {
        _models.erase(pos);
    }
//    LOG(gui, debug, "controller detach model finished.");
}


void
Controller::syncModelViews()
{
    for (ModelIterator it = beginModel(); it != endModel(); ++it) {
        for (Model::ViewIterator viewIt = (*it)->beginView(); viewIt != (*it)->endView(); ++viewIt) {
            (*viewIt)->syncView();
        }
    }
}


Controller::ModelIterator
Controller::beginModel()
{
    return _models.begin();
}


Controller::ModelIterator
Controller::endModel()
{
    return _models.end();
}


} // namespace Gui
} // namespace Omm
