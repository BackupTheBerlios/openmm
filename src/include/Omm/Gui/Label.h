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

#ifndef Label_INCLUDED
#define Label_INCLUDED

#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class LabelModel : public Model
{
public:
//    virtual const std::string& getLabel() const;
    virtual std::string getLabel();
    void setLabel(const std::string& label);
    
private:
    std::string _label;
};


class LabelView : public View
{
    friend class LabelModel;
    
public:
    LabelView(View* pParent = 0);
    
private:
    virtual void syncView(Model* pModel);
};


class Label : public Widget<LabelView, Controller, LabelModel>
{
public:
    Label(View* pParent = 0) : Widget<LabelView, Controller, LabelModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
