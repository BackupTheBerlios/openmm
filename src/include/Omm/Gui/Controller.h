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

#ifndef Controller_INCLUDED
#define Controller_INCLUDED

#include <vector>

namespace Omm {
namespace Gui {

class View;
class Model;


#define NOTIFY_MODELS(CLASS, METHOD, ...) for (ModelIterator it = beginModel(); it != endModel(); ++it) { static_cast<CLASS*>(*it)->METHOD(__VA_ARGS__); }

class Controller
{
public:
    friend class Model;

    void attachModel(Model* pModel);

protected:
    typedef std::vector<Model*>::iterator ModelIterator;
    ModelIterator beginModel();
    ModelIterator endModel();

    std::vector<Model*>     _models;
};


}  // namespace Omm
}  // namespace Gui

#endif
