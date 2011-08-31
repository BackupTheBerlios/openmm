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

    
class ListModel : public Model
{
public:
    virtual int totalItemCount() { return 0; }
    virtual Model* getItemModel(int row) { return 0; }
    virtual View* createItemView() { return 0; }

protected:
    void insertItem(int row);
    void removeItem(int row);
};


class LazyListModel : public ListModel
{
public:
    virtual bool canFetchMore() { return false; }
    virtual void fetchMore(bool forward = true) {}
    virtual int fetch(int rowCount = 10, bool forward = true) { return 0; }
    virtual int lastFetched(bool forward = true) { return (forward ? totalItemCount() : 0); }
};


}  // namespace Omm
}  // namespace Gui

#endif
