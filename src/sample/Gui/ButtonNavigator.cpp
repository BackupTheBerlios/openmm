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

#include <Omm/Gui/EventLoop.h>
#include <Omm/Gui/MainWindow.h>
#include <Omm/Gui/Button.h>
#include <Omm/Gui/Navigator.h>


class NavButton : public Omm::Gui::Button
{
public:
    NavButton(Omm::Gui::Navigator* pNavigator) :
    _pNavigator(pNavigator)
    {
        setLabel("Button " + Poco::NumberFormatter::format(_buttonCount++));
    }

private:
    void pushed()
    {
        NavButton* pNavButton = new NavButton(_pNavigator);
        _pNavigator->push(pNavButton, pNavButton->getLabel());
    }

    static int              _buttonCount;
    Omm::Gui::Navigator*    _pNavigator;
};

int NavButton::_buttonCount = 0;


int main(int argc, char** argv)
{
    Omm::Gui::EventLoop loop(argc, argv);
    Omm::Gui::MainWindow mainWindow;
    Omm::Gui::Navigator navigator;

    NavButton* pButton = new NavButton(&navigator);
    navigator.push(pButton, pButton->getLabel());

    mainWindow.resize(800, 480);
    mainWindow.setMainView(&navigator);
    mainWindow.show();

    loop.run();
}
