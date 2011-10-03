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
#include <Omm/Gui/LazyList.h>
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


class ButtonListModel : public Omm::Gui::LazyListModel
{
public:
    ButtonListModel(int itemCount);

    virtual int totalItemCount();
    virtual Omm::Gui::Model* getItemModel(int row);
    virtual Omm::Gui::View* createItemView();

    virtual int fetch(int rowCount = 10, bool forward = true);
    virtual int lastFetched(bool forward = true);
    
private:
    std::vector<Omm::Gui::ButtonModel*>    _itemModels;
    int                                    _viewCount;
    int                                    _rowsFetched;
};


ButtonListModel::ButtonListModel(int itemCount) :
_viewCount(0),
_rowsFetched(0)
{
    for (int i = 0; i < itemCount; i++) {
        Omm::Gui::ButtonModel* pItemModel = new Omm::Gui::ButtonModel;
        pItemModel->setLabel("button model " + Poco::NumberFormatter::format(i));
        _itemModels.push_back(pItemModel);
    }
}


int
ButtonListModel::totalItemCount()
{
    return _itemModels.size();
}


Omm::Gui::Model*
ButtonListModel::getItemModel(int row)
{
    return _itemModels[row];
}


Omm::Gui::View*
ButtonListModel::createItemView()
{
    ButtonItem* pButtonItem = new ButtonItem;
    pButtonItem->setName("button view " + Poco::NumberFormatter::format(_viewCount++));
    return pButtonItem;
}


int
ButtonListModel::fetch(int rowCount, bool forward)
{
    if (!forward) {
        return 0;
    }
    if (_rowsFetched < totalItemCount()) {
        int diff = std::min(rowCount, totalItemCount() - _rowsFetched);
        _rowsFetched += diff;
        return diff;
    }
    else {
        return 0;
    }
}


int
ButtonListModel::lastFetched(bool forward)
{
    return _rowsFetched;
}


class Application : public Omm::Gui::Application
{
    virtual Omm::Gui::View* createMainView()
    {
        ButtonListModel* pListModel = new ButtonListModel(10000);
        Omm::Gui::LazyListView* pList = new Omm::Gui::LazyListView;
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

