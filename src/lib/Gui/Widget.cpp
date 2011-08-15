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

#include "Gui/Widget.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/QtWidget.h"
#endif


namespace Omm {
namespace Gui {


Widget::Widget()
{
    _pImpl = new WidgetImpl;
}


Widget::~Widget()
{
    delete _pImpl;
}


void
Widget::showWidget()
{
    _pImpl->showWidget();
}


void
Widget::hideWidget()
{
    _pImpl->hideWidget();
}


Widget::SelectNotification::SelectNotification()
{
}


void
Widget::registerEventNotificationHandler(const Poco::AbstractObserver& observer)
{
    _eventNotificationCenter.addObserver(observer);
}


void
Widget::select()
{
    _eventNotificationCenter.postNotification(new SelectNotification);
}


ListWidget::ListWidget() :
_row(0)
{

}


int
ListWidget::getRow()
{
    return _row;
}


void
ListWidget::setRow(int row)
{
    _row = row;
}


ListWidget::RowSelectNotification::RowSelectNotification(int row) :
_row(row)
{
}


void
ListWidget::select()
{
    Widget::select();
    _eventNotificationCenter.postNotification(new RowSelectNotification(_row));
}


} // namespace Gui
} // namespace Omm
