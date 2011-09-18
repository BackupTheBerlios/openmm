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

#include <Poco/NumberFormatter.h>

#include <Omm/Gui/Application.h>
#include <Omm/Gui/List.h>
#include <Omm/Gui/ListModel.h>
#include <Omm/Gui/ListItem.h>


class StringListModel : public Omm::Gui::ListModel
{
public:
    StringListModel(int itemCount);

    virtual int totalItemCount();
    virtual Omm::Gui::Model* getItemModel(int row);
    virtual Omm::Gui::View* createItemView();
    
private:
    std::vector<Omm::Gui::ListItemModel*>    _itemModels;
    int                                      _viewCount;
};


StringListModel::StringListModel(int itemCount) :
_viewCount(0)
{
    for (int i = 0; i < itemCount; i++) {
        Omm::Gui::ListItemModel* pItemModel = new Omm::Gui::ListItemModel;
        pItemModel->setLabel("list item " + Poco::NumberFormatter::format(i));
        _itemModels.push_back(pItemModel);
    }
}


int
StringListModel::totalItemCount()
{
    return _itemModels.size();
}


Omm::Gui::View*
StringListModel::createItemView()
{
    Omm::Gui::ListItemView* pView = new Omm::Gui::ListItemView;
    pView->setName("string list view item " + Poco::NumberFormatter::format(_viewCount++));
    return pView;
}


Omm::Gui::Model*
StringListModel::getItemModel(int row)
{
    return _itemModels[row];
}


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        StringListModel* pListModel = new StringListModel(10000);
        Omm::Gui::ListView* pList = new Omm::Gui::ListView;
        pList->setModel(pListModel);
        resize(800, 480);
        return pList;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.run(argc, argv);
}

