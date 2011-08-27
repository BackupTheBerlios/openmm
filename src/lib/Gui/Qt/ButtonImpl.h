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

#ifndef ButtonImpl_INCLUDED
#define ButtonImpl_INCLUDED

#include <QtGui>
#include "WidgetImpl.h"

namespace Omm {
namespace Gui {

class Widget;
class Button;

class ButtonImpl : public QPushButton, public WidgetImpl
{
    Q_OBJECT

    friend class ButtonView;

private slots:
    void pushed();

private:
    ButtonImpl(Widget* pWidget, Widget* pParent = 0);
    virtual ~ButtonImpl();

    void setLabel(const std::string& label);
};


}  // namespace Omm
}  // namespace Gui

#endif

