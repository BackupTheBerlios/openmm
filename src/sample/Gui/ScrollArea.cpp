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
#include <Omm/Gui/ScrollArea.h>
#include <Omm/Gui/Label.h>


class ScrollArea : public Omm::Gui::ScrollArea
{
public:
    ScrollArea()
    {
        resizeScrollArea(10000, 10000);
        label.setLabel("offset x: 0, y: 0");
        label.resize(200, 40);
        addSubview(&label);
    }


private:
    void scrolled(int xOffset, int yOffset)
    {
        label.setLabel("offset x: " + Poco::NumberFormatter::format(xOffset) + ", y: " + Poco::NumberFormatter::format(yOffset));
        label.move(xOffset, yOffset);
    }

    Omm::Gui::Label label;
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        return new ScrollArea;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}


