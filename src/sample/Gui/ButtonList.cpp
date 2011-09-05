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

#include <vector>
#include <string>
#include <iostream>

#include <Poco/NumberFormatter.h>

#include <Omm/Gui/EventLoop.h>
#include <Omm/Gui/MainWindow.h>
#include <Omm/Gui/List.h>
#include <Omm/Gui/ListModel.h>
#include <Omm/Gui/Button.h>


class ButtonItem : public Omm::Gui::ButtonControllerView
{
private:
    virtual void pushed()
    {
        UPDATE_MODEL(Omm::Gui::ButtonModel, setLabel, "works!");
    }
};


class ButtonListModel : public Omm::Gui::ListModel
{
public:
    ButtonListModel(int itemCount)
    {
        for (int i = 0; i < itemCount; i++) {
            Omm::Gui::ButtonModel* pItemModel = new Omm::Gui::ButtonModel;
            pItemModel->setLabel("Button Model " + Poco::NumberFormatter::format(i));
            _itemModels.push_back(pItemModel);
        }
    }

private:
    virtual int totalItemCount()
    {
        return _itemModels.size();
    }

    virtual Omm::Gui::Model* getItemModel(int row)
    {
        return _itemModels[row];
    }

    virtual Omm::Gui::View* createItemView()
    {
        return new ButtonItem;
    }
    
    std::vector<Omm::Gui::ButtonModel*>    _itemModels;
};


class ButtonListController : public Omm::Gui::ListController
{
    virtual void selectedItem(int row)
    {
        std::cout << "selected list item in row " << row << std::endl;
    }
};


int main(int argc, char** argv)
{
    Omm::Gui::EventLoop loop(argc, argv);
    Omm::Gui::MainWindow mainWindow;

    Omm::Gui::ListView list;
//    ButtonListModel listModel(10000);
    ButtonListModel listModel(1);
    list.setModel(&listModel);
    ButtonListController listController;
    list.attachController(&listController);

    mainWindow.setMainView(&list);
    mainWindow.resize(800, 480);
    mainWindow.show();
    loop.run();
}

