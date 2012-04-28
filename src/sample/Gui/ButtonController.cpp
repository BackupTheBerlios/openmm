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

#include <Omm/Gui/Application.h>
#include <Omm/Gui/Button.h>


class HelloButtonController : public Omm::Gui::ButtonController
{
private:
    void pushed()
    {
        UPDATE_MODEL(Omm::Gui::ButtonModel, setLabel, "works!");
        syncModelViews();
    }
};


class HelloApplication : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        HelloButtonController* pButtonController = new HelloButtonController;
        Omm::Gui::Button* pButton = new Omm::Gui::Button;
        pButton->attachController(pButtonController);
        pButtonController->attachModel(pButton);
        pButton->setLabel("Hello GUI");
        return pButton;
    }
};


int main(int argc, char** argv)
{
    HelloApplication app;
    return app.runEventLoop(argc, argv);
}

