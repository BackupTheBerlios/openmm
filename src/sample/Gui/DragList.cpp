/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2012                                                       |
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
#include <Omm/Gui/Label.h>
#include <Omm/Gui/ListItem.h>


class DragListModel : public Omm::Gui::ListModel
{
public:
    DragListModel(int itemCount);

    virtual int totalItemCount();
    virtual Omm::Gui::Model* getItemModel(int row);
    virtual Omm::Gui::View* createItemView();

private:
//    std::vector<Omm::Gui::LabelModel*>    _itemModels;
    std::vector<Omm::Gui::ListItemModel*>    _itemModels;
    int                                   _viewCount;
};


DragListModel::DragListModel(int itemCount) :
_viewCount(0)
{
    for (int i = 0; i < itemCount; i++) {
        Omm::Gui::LabelModel* pItemModel = new Omm::Gui::LabelModel;
        Omm::Gui::ListItemModel* pListItemModel = new Omm::Gui::ListItemModel;
        pItemModel->setLabel("item " + Poco::NumberFormatter::format(i));
        pListItemModel->setLabelModel(pItemModel);
        _itemModels.push_back(pListItemModel);
    }
}


int
DragListModel::totalItemCount()
{
    return _itemModels.size();
}


Omm::Gui::View*
DragListModel::createItemView()
{
//    Omm::Gui::LabelView* pView = new Omm::Gui::LabelView;
    Omm::Gui::ListItemView* pView = new Omm::Gui::ListItemView;
    pView->setName("item view " + Poco::NumberFormatter::format(_viewCount++));
    return pView;
}


Omm::Gui::Model*
DragListModel::getItemModel(int row)
{
    return _itemModels[row];
}


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        DragListModel* pListModel = new DragListModel(20);
        Omm::Gui::ListView* pList = new Omm::Gui::ListView;
        pList->setName("list view");
        pList->setModel(pListModel);
        pList->setDragMode(Omm::Gui::ListView::DragSource | Omm::Gui::ListView::DragTarget);
        resizeMainView(800, 480);
        return pList;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

