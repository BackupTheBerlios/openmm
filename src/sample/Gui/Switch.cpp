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

#include <Poco/NumberFormatter.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/Label.h>
#include <Omm/Gui/VerticalLayout.h>
#include <Omm/Gui/Switch.h>


class SwitchController : public Omm::Gui::SwitchController
{
private:
    void switched(bool on)
    {
        std::cout << "switched: " << std::string(on ? "on" : "off") << std::endl;
    }
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::SwitchView* pSwitch = new Omm::Gui::SwitchView;
        pSwitch->attachController(new SwitchController);

        return pSwitch;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

