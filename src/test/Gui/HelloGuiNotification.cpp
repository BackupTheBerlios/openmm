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

#include <Omm/Gui/EventLoop.h>
#include <Omm/Gui/MainWindow.h>
#include <Omm/Gui/Button.h>


class HelloGui : public Omm::Gui::MainWindow
{
public:
    HelloGui()
    {
        _pButton = new Omm::Gui::Button(this);
        _pButton->setLabel("Hello GUI");
        _pButton->connect(Poco::Observer<HelloGui, Omm::Gui::Button::PushNotification>(*this, &HelloGui::onPush));
        setMainView(_pButton);
    }

    void onPush(Omm::Gui::Button::PushNotification* pNotification)
    {
        _pButton->setLabel("works!");
    }
    
private:
    Omm::Gui::Button* _pButton;
};


int main(int argc, char** argv)
{
    Omm::Gui::EventLoop loop(argc, argv);
    HelloGui mainWindow;

    mainWindow.show();

    loop.run();
}

