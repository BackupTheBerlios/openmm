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

#include <Poco/NumberFormatter.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/Label.h>
#include <Omm/Gui/VerticalLayout.h>
#include <Omm/Gui/Selector.h>


class SelectorController : public Omm::Gui::SelectorController
{
private:
    void selected(int row)
    {
        std::cout << "selected row: " << row << std::endl;
    }
};


class SelectorModel : public Omm::Gui::SelectorModel
{
public:
    SelectorModel(int itemCount)
    {
        for (int i = 0; i < itemCount; i++) {
            _items.push_back("item " + Poco::NumberFormatter::format(i));
        }
    }

    virtual int totalItemCount()
    {
        return _items.size();
    }

    virtual std::string getItemLabel(int row)
    {
        return _items[row];
    }

private:
    std::vector<std::string>    _items;
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        Omm::Gui::View* pView = new Omm::Gui::View;

        SelectorModel* pSelectorModel = new SelectorModel(5);
        Omm::Gui::SelectorView* pSelector = new Omm::Gui::SelectorView(pView);
        pSelector->attachController(new SelectorController);
        pSelector->setModel(pSelectorModel);

        SelectorModel* pSelectorModel2 = new SelectorModel(2);
        Omm::Gui::SelectorView* pSelector2 = new Omm::Gui::SelectorView(pView);
        pSelector2->attachController(new SelectorController);
        pSelector2->setModel(pSelectorModel2);

        pView->setLayout(new Omm::Gui::VerticalLayout);

        return pView;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

