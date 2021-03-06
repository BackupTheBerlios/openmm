/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#include <iostream>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/Slider.h>


class SliderController : public Omm::Gui::SliderController
{
private:
    void valueChanged(int value)
    {
        std::cout << "slider value: " << value << std::endl;
    }
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::Slider* pSlider = new Omm::Gui::Slider;
        pSlider->attachController(new SliderController);
        return pSlider;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

