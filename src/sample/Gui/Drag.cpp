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
#include <Omm/Gui/Label.h>
#include <Omm/Gui/Button.h>
#include <Omm/Gui/VerticalLayout.h>
#include <Omm/Gui/Drag.h>
#include <Omm/Gui/GuiLogger.h>
#include <Omm/Log.h>


class Application : public Omm::Gui::Application
{
    class DragController : public Omm::Gui::Controller
    {
        virtual void dragStarted()
        {
            LOGNS(Omm::Gui, gui, debug, "drag event");
        }

        virtual void selected()
        {
            LOGNS(Omm::Gui, gui, debug, "select event");
        }

    };


    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::Label* pSource = new Omm::Gui::Label;
        pSource->setName("source view");
        pSource->setLabel("Source");
        pSource->setBackgroundColor(Omm::Gui::Color("blue"));
        pSource->setAlignment(Omm::Gui::View::AlignCenter);
        pSource->attachController(new DragController);

        Omm::Gui::Label* pTarget = new Omm::Gui::Label;
        pTarget->setName("target view");
        pTarget->setLabel("Target");
        pTarget->setBackgroundColor(Omm::Gui::Color("white"));
        pTarget->setAlignment(Omm::Gui::View::AlignCenter);
        pTarget->attachController(new DragController);

        Omm::Gui::View* pView = new Omm::Gui::View;
        pView->setName("main view");
        pView->addSubview(pSource);
        pView->addSubview(pTarget);
        pView->setLayout(new Omm::Gui::VerticalLayout);
        pView->attachController(new DragController);

        return pView;
    }
};


int main(int argc, char** argv)
{
    Application app;
    app.resizeMainView(300, 400);
    return app.runEventLoop(argc, argv);
}


