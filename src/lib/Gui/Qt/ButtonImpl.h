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

#ifndef ButtonImpl_INCLUDED
#define ButtonImpl_INCLUDED

#include "ViewImpl.h"

namespace Omm {
namespace Gui {

class View;
class Image;

class ButtonViewImpl : public ViewImpl
{
    friend class ButtonView;

private:
    ButtonViewImpl(View* pView);
    ~ButtonViewImpl();

    void setLabel(const std::string& label);
    void setEnabled(bool enabled);
    void setImage(Image* pImage);
};


}  // namespace Omm
}  // namespace Gui

#endif

