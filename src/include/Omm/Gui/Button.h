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

#ifndef Button_INCLUDED
#define Button_INCLUDED

#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "Image.h"


namespace Omm {
namespace Gui {


class ButtonController : public Controller
{
    friend class ButtonViewImpl;
    friend class ButtonSignalProxy;

protected:
    virtual void pushed() {}
};


class ButtonModel : public Model
{
public:
    ButtonModel();

//    virtual const std::string& getLabel() const;
    virtual std::string getLabel();
    void setLabel(const std::string& label);

    virtual bool getEnabled();
    void setEnabled(bool enabled = true);

    virtual Image* getImage();
    void setImage(Image* pImage);

private:
    bool        _enabled;
    std::string _label;
    Image*      _pImage;
};


class ButtonView : public View
{
    friend class ButtonModel;

public:
    ButtonView(View* pParent = 0);

//private:
    virtual void syncViewImpl();
};


class ButtonControllerView : public ControllerView<ButtonController, ButtonView>
{
public:
    ButtonControllerView(View* pParent = 0) : ControllerView<ButtonController, ButtonView>(pParent) {}
};


class ButtonControllerModel : public ControllerModel<ButtonController, ButtonModel>
{
public:
    ButtonControllerModel() : ControllerModel<ButtonController, ButtonModel>() {}
};


class Button : public Widget<ButtonView, ButtonController, ButtonModel>
{
public:
    Button(View* pParent = 0) : Widget<ButtonView, ButtonController, ButtonModel>(pParent) {}

    void setLabel(const std::string& label);
    void setEnabled(bool enabled = true);
    void setImage(Image* pImage);
};


}  // namespace Omm
}  // namespace Gui

#endif
