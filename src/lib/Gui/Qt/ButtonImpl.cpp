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

#include "ButtonImpl.h"
#include "Gui/Button.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ButtonImpl::ButtonImpl(View* pView, View* pParent) :
QPushButton(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
ViewImpl(pView, this)
{
    Omm::Gui::Log::instance()->gui().debug("button implementation ctor");
    connect(this, SIGNAL(pressed()), this, SLOT(pushed()));
}


ButtonImpl::~ButtonImpl()
{
}


void
ButtonImpl::setLabel(const std::string& label)
{
    QPushButton::setText(QString::fromStdString(label));
}


void
ButtonImpl::pushed()
{
    Omm::Gui::Log::instance()->gui().debug("button implementation, calling pushed virtual method");
//    static_cast<ButtonController*>(_pView->getController())->pushed();
    NOTIFY_CONTROLLERS(ButtonController, pushed);
}


}  // namespace Omm
}  // namespace Gui
