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

#include "Gui/Model.h"
#include "Gui/GuiLogger.h"
#include "Gui/View.h"
#include "ModelImpl.h"
#include "UIKit/ModelImpl.h"


namespace Omm {
namespace Gui {


Model::Model()
{
    _pImpl = new ModelImpl(this);
}


Model::Model(const Model& model) :
_views(model._views)
{
}


void*
Model::getNativeModel()
{
    return _pImpl->getNativeModel();
}


void
Model::attachView(View* pView)
{
//    LOG(gui, debug, "model attach view ...");
    _views.push_back(pView);
//    LOG(gui, debug, "model attach view finished.");
}


void
Model::detachView(View* pView)
{
//    LOG(gui, debug, "model detach view ...");
    ViewIterator pos = std::find(beginView(), endView(), pView);
    if (pos != _views.end()) {
        _views.erase(pos);
    }
//    LOG(gui, debug, "model detach view finished.");
}


Model::ViewIterator
Model::beginView()
{
    return _views.begin();
}


Model::ViewIterator
Model::endView()
{
    return _views.end();
}


void
Model::syncViews()
{
//    LOG(gui, debug, "model sync views");
    for (ViewIterator it = beginView(); it != endView(); ++it) {
       (*it)->syncView();
    }
}


} // namespace Gui
} // namespace Omm
