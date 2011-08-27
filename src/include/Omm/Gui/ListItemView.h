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

#ifndef ListItemView_INCLUDED
#define ListItemView_INCLUDED

#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>

#include "View.h"


namespace Omm {
namespace Gui {


class ListItemView : public View
{
public:
    ListItemView();

    int getRow();
    void setRow(int row);

    class RowSelectNotification : public Poco::Notification
    {
    public:
        RowSelectNotification(int row);

        int _row;
    };

protected:
    virtual void select();

private:
    int _row;
};


class ListItemViewFactory
{
public:
    virtual ListItemView* createItemView() { return 0; }
};


}  // namespace Omm
}  // namespace Gui

#endif
