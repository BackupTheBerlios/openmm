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

#ifndef Image_INCLUDED
#define Image_INCLUDED

#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class ImageModel : public Model
{
public:
    virtual const std::string& getData() const;
    void setData(const std::string& data);
    void setFile(const std::string& fileName);
    
private:
    std::string _data;
};


class ImageView : public View
{
    friend class ImageModel;
    
public:
    ImageView(View* pParent = 0);

    void setAlignment(Alignment alignment);

//private:
    virtual void syncViewImpl();
};


class Image : public Widget<ImageView, Controller, ImageModel>
{
public:
    Image(View* pParent = 0) : Widget<ImageView, Controller, ImageModel>(pParent) {}
    
    void setData(const std::string& data);
    void setFile(const std::string& fileName);
};


}  // namespace Omm
}  // namespace Gui

#endif
