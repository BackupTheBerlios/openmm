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


class HelloButtonModel : public Omm::Gui::ButtonControllerModel
{
private:
    void pushed()
    {
        UPDATE_MODEL(Omm::Gui::ButtonModel, setLabel, "works!");
    }
};


class HelloApplication : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        HelloButtonModel* pButtonModel = new HelloButtonModel;
        Omm::Gui::ButtonView* pButtonView = new Omm::Gui::ButtonView;
        pButtonView->setControllerModel(pButtonModel);
        pButtonModel->setLabel("Hello GUI");
        return pButtonView;
    }
};


int main(int argc, char** argv)
{
    HelloApplication app;
    return app.run(argc, argv);
}

