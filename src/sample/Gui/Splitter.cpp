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
#include <Omm/Gui/Splitter.h>


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::Splitter* pSplitter = new Omm::Gui::Splitter;
        pSplitter->setOrientation(Omm::Gui::View::Vertical);

        const int subviewCount = 3;
        for (int i = 0; i < subviewCount; i++) {
            Omm::Gui::Button* pButton = new Omm::Gui::Button;
            std::string label = "B" + Poco::NumberFormatter::format(i);
            pButton->setName(label);
            pButton->setLabel(label);
            pSplitter->insertView(pButton, i);
//            if (i == 1) {
//                pButton->hide();
//            }
        }
//        std::vector<float> sizes;
//        sizes.push_back(0.1);
//        sizes.push_back(0.3);
//        sizes.push_back(0.6);
//        pSplitter->setSizes(sizes);
        pSplitter->setSize(0, 0.5);

        return pSplitter;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

