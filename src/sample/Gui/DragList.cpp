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

    void insertItemModel(Omm::Gui::ListItemModel* pItem, int row);
    void removeItemModel(int row);

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


void
DragListModel::insertItemModel(Omm::Gui::ListItemModel* pItem, int row)
{
    _itemModels.insert(_itemModels.begin() + row, pItem);
}


void
DragListModel::removeItemModel(int row)
{
    _itemModels.erase(_itemModels.begin() + row);
}


class DragListController : public Omm::Gui::ListController
{
public:
    DragListController(DragListModel* pModel) : _pModel(pModel), _draggedStartedInRow(-1) {}

    virtual void draggedItem(int row)
    {
//        _pModel->removeItemModel(row);
        _draggedStartedInRow = row;
    }

    virtual void droppedItem(Omm::Gui::Model* pModel, int row)
    {
        if (_draggedStartedInRow != -1) {
            _pModel->removeItemModel(_draggedStartedInRow);
            _draggedStartedInRow = -1;
        }
        _pModel->insertItemModel(static_cast<Omm::Gui::ListItemModel*>(pModel), row);
    }

    DragListModel*      _pModel;
    int                 _draggedStartedInRow;
};


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        DragListModel* pListModel = new DragListModel(20);
        Omm::Gui::ListView* pList = new Omm::Gui::ListView;
        pList->setName("list view");
        pList->setDragMode(Omm::Gui::ListView::DragSource | Omm::Gui::ListView::DragTarget);
        pList->setModel(pListModel);
        pList->attachController(new DragListController(pListModel));
        resizeMainView(800, 480);
        return pList;
    }
};


int main(int argc, char** argv)
{
    Application app;
    return app.runEventLoop(argc, argv);
}

