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

#include "Gui/ListItemView.h"


namespace Omm {
namespace Gui {


ListItemView::ListItemView() :
_row(0)
{

}


int
ListItemView::getRow()
{
    return _row;
}


void
ListItemView::setRow(int row)
{
    _row = row;
}


ListItemView::RowSelectNotification::RowSelectNotification(int row) :
_row(row)
{
}


void
ListItemView::select()
{
//    Widget::select();
//    _eventNotificationCenter.postNotification(new RowSelectNotification(_row));
}


} // namespace Gui
} // namespace Omm
