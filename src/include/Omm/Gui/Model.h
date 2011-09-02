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

#ifndef Model_INCLUDED
#define Model_INCLUDED

#include <vector>
#include "Controller.h"


namespace Omm {
namespace Gui {

class View;

#define UPDATE_VIEWS(CLASS, METHOD, ...) for (ViewIterator it = beginView(); it != endView(); ++it) \
{ CLASS* pCLASS = dynamic_cast<CLASS*>(*it); if (pCLASS) { pCLASS->METHOD(__VA_ARGS__); } }



class Model
{
public:
    friend class View;

    void attachView(View* pView);
    void detachView(View* pView);

protected:
    typedef std::vector<View*>::iterator ViewIterator;
    ViewIterator beginView();
    ViewIterator endView();

    virtual void syncViews();

    std::vector<View*>     _views;
};


template <class C, class M>
class ControllerModel : public C, public M
{
public:
    ControllerModel()
    {
        C::attachModel(this);
    }

    virtual ~ControllerModel() {}
};


}  // namespace Omm
}  // namespace Gui

#endif
