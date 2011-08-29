/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2011                                                       |
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

#ifndef ListModel_INCLUDED
#define ListModel_INCLUDED

#include "Model.h"


namespace Omm {
namespace Gui {

    
class ListView;
class ListItemView;
class ListItemViewFactory;


class ListModel : public Model
{
    friend class ListView;

public:
    ListModel();

    // item related
    virtual int totalItemCount() { return 0; }
    virtual View* createItemView() { return 0; }
    virtual Model* getItemModel(int row) { return 0; }

    void insertItem(int row);
    /// Ask the view to show an item at row. No data is created (cached), as
    /// the whole data of the model is already present in the underlying implementation.
    /// The model is only an abstraction layer on top of the data.
    void removeItem(int row);
    /// See insertItem().
    /// View should call selectItem() when the item in row is selected.

    // lazy model related
    virtual bool canFetchMore() { return false; }
    virtual void fetchMore(bool forward = true) {}
    virtual int fetch(int rowCount = 10, bool forward = true) { return 0; }
    virtual int lastFetched(bool forward = true) { return (forward ? totalItemCount() : 0); }

    // widget related
//    void setItemViewFactory(ListItemViewFactory* pViewFactory);
//    virtual void attachView(int row, ListItemView* pView) {}
//    virtual void detachView(int row) {}

//private:
//    ListView*                     _pView;
//    ListItemViewFactory*          _pViewFactory;
};


class ListController : public Controller
{
protected:
    virtual void selectItem(int row) {}
};


}  // namespace Omm
}  // namespace Gui

#endif
