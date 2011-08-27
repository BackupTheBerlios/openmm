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

#ifndef Navigable_INCLUDED
#define Navigable_INCLUDED


namespace Omm {
namespace Gui {

class Navigator;


class Navigable
{
    friend class Navigator;

public:
    Navigable();

    virtual std::string getBrowserTitle() { return ""; }
    virtual View* getView() { return 0; }
    /// If getView() returns not null but a valid view, the view
    /// is pushed on QtNavigator::_pStackedWidget.
    virtual void show() {}
    /// Additionally, show() can be implemented if for example no view
    /// is pushed but some other action is necessary to show the correct view.
    Navigator* getNavigator() const;

private:
    Navigator*    _pNavigator;
};


}  // namespace Omm
}  // namespace Gui

#endif
