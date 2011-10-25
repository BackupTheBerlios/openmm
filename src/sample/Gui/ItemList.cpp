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

#include "ImageData.h"

class ItemListModel : public Omm::Gui::ListModel
{
public:
    ItemListModel(int itemCount);

    virtual int totalItemCount();
    virtual Omm::Gui::Model* getItemModel(int row);
    virtual Omm::Gui::View* createItemView();
    
private:
    std::vector<Omm::Gui::ListItemModel*>    _itemModels;
    int                                      _viewCount;
};


ItemListModel::ItemListModel(int itemCount) :
_viewCount(0)
{
    for (int i = 0; i < itemCount; i++) {
        Omm::Gui::ListItemModel* pItemModel = new Omm::Gui::ListItemModel;
        
        Omm::Gui::ImageModel* pImageModel = new Omm::Gui::ImageModel;
        pImageModel->setData(std::string(ImageData, ImageSize));
        pItemModel->setImageModel(pImageModel);
        
        Omm::Gui::LabelModel* pLabelModel = new Omm::Gui::LabelModel;
        pLabelModel->setLabel("list item " + Poco::NumberFormatter::format(i));
        pItemModel->setLabelModel(pLabelModel);
        
        _itemModels.push_back(pItemModel);
    }
}


int
ItemListModel::totalItemCount()
{
    return _itemModels.size();
}


Omm::Gui::View*
ItemListModel::createItemView()
{
    Omm::Gui::ListItemView* pView = new Omm::Gui::ListItemView;
    pView->setName("item list view item " + Poco::NumberFormatter::format(_viewCount++));
    return pView;
}


Omm::Gui::Model*
ItemListModel::getItemModel(int row)
{
    return _itemModels[row];
}


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        ItemListModel* pListModel = new ItemListModel(10000);
        Omm::Gui::ListView* pList = new Omm::Gui::ListView;
        pList->setModel(pListModel);
        resizeMainView(800, 480);
        return pList;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.run(argc, argv);
}

