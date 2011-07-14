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

#include "QtNavigator.h"


void
QtNavigator::push(QtNavigable* pNavigable, QWidget* pWidget)
{

}


void
QtNavigator::pop(QtNavigable* pNavigable, QWidget* pWidget)
{

}


//void
//QtDeviceGroup::pushBrowser(QtNavigable* pBrowser)
//{
////    new QtCrumbButton(_pDeviceListView, QModelIndex(), _pCrumbButton);
//    // FIXME: this only works for first level.
//    _pCrumbButton = new QtCrumbButton(_pDeviceListView, QModelIndex(), _pCrumbPanel);
//    _pStackedWidget->addWidget(pBrowser);
//    _stack.push(pBrowser);
//}
//
//
//void
//QtDeviceGroup::popBrowser()
//{
//    _pStackedWidget->removeWidget(_stack.top());
//    _stack.pop();
//}