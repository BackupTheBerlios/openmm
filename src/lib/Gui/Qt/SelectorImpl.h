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

#ifndef SelectorImpl_INCLUDED
#define SelectorImpl_INCLUDED

#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;

class SelectorViewImpl : public ViewImpl
{
    friend class SelectorView;

    SelectorViewImpl(View* pView);

    int getCurrentIndex();
    void setCurrentIndex(int index);
    void clear();
    void addItem(const std::string& label, Image* pImage);
};


}  // namespace Omm
}  // namespace Gui

#endif

