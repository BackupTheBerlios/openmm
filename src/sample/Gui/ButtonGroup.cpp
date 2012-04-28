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
#include <Omm/Gui/Button.h>


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::View* pCompoundView = new Omm::Gui::View;

        int windowHeight = 0;
        int windowWidth = 0;
        int buttonCount = 10;
        for(int i = 0; i < buttonCount; i++) {
            Omm::Gui::Button* pButton = new Omm::Gui::Button(pCompoundView);
            pButton->resize(80, 40);
            pButton->setLabel("Button " + Poco::NumberFormatter::format(i + 1));
            pButton->move(0, pButton->height() * i);
            windowHeight += pButton->height();
            windowWidth = pButton->width();
        }
        resizeMainView(windowWidth, windowHeight);
        return pCompoundView;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

