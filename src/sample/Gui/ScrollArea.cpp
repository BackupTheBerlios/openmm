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
#include <Omm/Gui/ScrollArea.h>


class ScrollAreaController : public Omm::Gui::ScrollAreaController
{
private:
    void scrolled(int value)
    {
        std::cout << "scrolled by value: " << value << std::endl;
    }

    void resized(int width, int height)
    {
        std::cout << "resized width: " << width << ", height: " << height << std::endl;
    }
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::ScrollArea* pScrollArea = new Omm::Gui::ScrollArea;
        pScrollArea->attachController(new ScrollAreaController);
        pScrollArea->resizeScrollArea(pScrollArea->getViewportWidth(), 10000);
        return pScrollArea;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.run(argc, argv);
}


