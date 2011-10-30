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

#ifndef Slider_INCLUDED
#define Slider_INCLUDED

#include "View.h"
#include "Model.h"
#include "Controller.h"


namespace Omm {
namespace Gui {


class SliderController : public Controller
{
    friend class SliderViewImpl;
    friend class SliderSignalProxy;
    
protected:
    virtual void valueChanged(int value) {}
};


class SliderModel : public Model
{
public:
    virtual const int getValue() const;
    void setValue(int value);
    
private:
    int _value;
};


class SliderView : public View
{
    friend class SliderModel;
    
public:
    SliderView(View* pParent = 0);
    
private:
    virtual void syncView(Model* pModel);
};


class SliderControllerView : public ControllerView<SliderController, SliderView>
{
public:
    SliderControllerView(View* pParent = 0) : ControllerView<SliderController, SliderView>(pParent) {}
};


class SliderControllerModel : public ControllerModel<SliderController, SliderModel>
{
public:
    SliderControllerModel() : ControllerModel<SliderController, SliderModel>() {}
};


class Slider : public Widget<SliderView, SliderController, SliderModel>
{
public:
    Slider(View* pParent = 0) : Widget<SliderView, SliderController, SliderModel>(pParent) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
