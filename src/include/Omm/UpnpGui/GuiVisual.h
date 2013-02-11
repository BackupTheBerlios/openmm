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

#ifndef GuiVisual_INCLUDED
#define GuiVisual_INCLUDED

#include "../Sys/Visual.h"

#include "../Gui/View.h"
#include "../Gui/Image.h"


namespace Omm {


class GuiVisual : public Gui::Image, public Sys::Visual
{
public:
    GuiVisual(Gui::View* pParent = 0);
    virtual ~GuiVisual();

    virtual void show();
    virtual void hide();

    virtual void* getWindow();
    virtual WindowHandle getWindowId();
    virtual VisualType getType();
    virtual void renderImage(const std::string& imageData);
    virtual void blank();
};


}  // namespace Omm


#endif
