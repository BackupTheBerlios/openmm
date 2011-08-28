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

#include <Poco/NumberFormatter.h>

#include "Gui/ListModel.h"
#include "Gui/GuiLogger.h"
#include "Gui/List.h"
#include "Gui/ListItem.h"

namespace Omm {
namespace Gui {


ListModel::ListModel() :
_pViewFactory(0)
{
}


void
ListModel::insertItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Log::instance()->gui().debug("list model insert row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        // update all attached views
        UPDATE(ListView, insertItem, row);
    }
    else {
        Log::instance()->gui().error("list model tries to insert item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
ListModel::removeItem(int row)
{
    if (0 <= row && row < totalItemCount()) {
        Log::instance()->gui().debug("list model remove row: " + Poco::NumberFormatter::format(row) + ", row count: " + Poco::NumberFormatter::format(totalItemCount()));
        UPDATE(ListView, removeItem, row);
    }
    else {
        Log::instance()->gui().error("list model tries to remove item in row number not less than total row count or less than zero (ignoring)");
    }
}


void
ListModel::setItemViewFactory(ListItemViewFactory* pViewFactory)
{
    _pViewFactory = pViewFactory;
}


ListItemView*
ListModel::createView()
{
    if (_pViewFactory) {
        return _pViewFactory->createItemView();
    }
    return 0;
}


} // namespace Gui
} // namespace Omm
