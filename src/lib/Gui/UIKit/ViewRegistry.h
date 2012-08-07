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

#ifndef ViewRegistry_INCLUDED
#define ViewRegistry_INCLUDED

#import <UIKit/UIKit.h>

#include <map>
#include "Gui/View.h"


namespace Omm {
namespace Gui {


class ViewRegistry
{
public:
    static ViewRegistry* instance();

    void registerView(View* pView, UIView* pNative);
    View* getViewForNative(UIView* pNative);

private:
    ViewRegistry();

    static ViewRegistry*        _pInstance;
    std::map<UIView*, View*>    _views;
};


}  // namespace Omm
}  // namespace Gui

#endif

