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

#ifndef UIDrag_INCLUDED
#define UIDrag_INCLUDED

#import <UIKit/UIKit.h>

#include <map>
#include "Gui/View.h"


namespace Omm {
namespace Gui {


class UIDrag
{
public:
    static UIDrag* instance();

    Drag* getDrag();
    void setDrag(Drag* pDrag);

    View* getMainView();
    void setMainView(View* pView);

    View* getDropView();
    void setDropView(View* pView);

    View* getPointerView();
    void setPointerView(View* pView);

private:
    UIDrag();

    static UIDrag*      _pInstance;
    Drag*               _pDrag;
    View*               _pMainView;
    View*               _pDropView;
    View*               _pPointerView;
};


}  // namespace Omm
}  // namespace Gui

#endif

