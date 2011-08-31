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
#include <Omm/Gui/HorizontalLayout.h>
#include <Omm/Gui/Button.h>


int main(int argc, char** argv)
{
    Omm::Gui::EventLoop loop(argc, argv);
    Omm::Gui::MainWindow mainWindow;
    Omm::Gui::View compoundView;
    Omm::Gui::HorizontalLayout layout;

    int buttonCount = 5;
    for(int i = 0; i < buttonCount; i++) {
        Omm::Gui::Button* pButton = new Omm::Gui::Button(&compoundView);
        pButton->setLabel("Button " + Poco::NumberFormatter::format(i + 1));
    }

    compoundView.resize(600, 100);
    compoundView.setLayout(&layout);
    mainWindow.resize(600, 100);
    mainWindow.setMainView(&compoundView);
    mainWindow.show();

    loop.run();
}
