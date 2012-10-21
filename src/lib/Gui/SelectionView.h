/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2012                                                       |
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

#ifndef SelectionView_INCLUDED
#define SelectionView_INCLUDED

#include "Gui/View.h"


namespace Omm {
namespace Gui {



class SelectionView : public View
{
public:
    SelectionView();

    void setChildView(View* pParent);

    virtual void raise();
    virtual void show(bool async = true);
    virtual void hide(bool async = true);
    virtual void resize(int width, int height);
    virtual void move(int x, int y);

private:
    View*       _pBottom;
    int         _height;
};


}  // namespace Omm
}  // namespace Gui

#endif
