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

#include <Poco/NumberFormatter.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/VerticalLayout.h>
#include <Omm/Gui/Button.h>


class MasterController: public Omm::Gui::ButtonController
{
private:
    virtual void pushed()
    {
        UPDATE_MODEL(Omm::Gui::ButtonModel, setLabel, "One for all!");
        syncModelViews();
    }
};


class SlaveButton: public Omm::Gui::Button
{
public:
    SlaveButton(Omm::Gui::View* pParent = 0) : Button(pParent) {}

private:
    virtual void pushed()
    {
        setLabel(getName());
    }
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::View* pCompoundView = new Omm::Gui::View;
        Omm::Gui::VerticalLayout* pLayout = new Omm::Gui::VerticalLayout;

        Omm::Gui::Button* pMasterButton = new Omm::Gui::Button(pCompoundView);
        MasterController* pMasterController = new MasterController;
        pMasterButton->attachController(pMasterController);
        pMasterButton->setLabel("Master Button");

        int buttonCount = 5;
        for(int i = 0; i < buttonCount; i++) {
            SlaveButton* pButton = new SlaveButton(pCompoundView);
            std::string name = "Button " + Poco::NumberFormatter::format(i + 1);
            pButton->setName(name);
            pButton->setLabel(name);
            pMasterController->attachModel(pButton);
        }

        pCompoundView->setLayout(pLayout);
        return pCompoundView;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.run(argc, argv);
}

