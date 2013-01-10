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

#ifndef ImageImpl_INCLUDED
#define ImageImpl_INCLUDED

#include "ViewImpl.h"
#include "ModelImpl.h"


namespace Omm {
namespace Gui {

class View;


class ImageModelImpl : public ModelImpl
{
public:
    ImageModelImpl(Model* pModel);

    void setData(const std::string& data);
};


class ImageViewImpl : public ViewImpl
{
    friend class ImageView;
    friend class ButtonViewImpl;

private:
    ImageViewImpl(View* pView);
    ~ImageViewImpl();

    void syncViewImpl();
    void setAlignment(View::Alignment alignment);
    void scaleBestFit(int width, int height);
};


}  // namespace Omm
}  // namespace Gui

#endif

