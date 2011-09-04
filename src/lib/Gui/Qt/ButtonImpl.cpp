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


ButtonViewImpl::ButtonViewImpl(View* pView, View* pParent) :
//QPushButton(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
//ViewImpl(pView, this)
//_pNativeView(new QPushButton(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0))),
ViewImpl(pView, new QPushButton(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)))
{
    Omm::Gui::Log::instance()->gui().debug("button view impl ctor");
//    connect(this, SIGNAL(pressed()), this, SLOT(pushed()));
    connect(_pNativeView, SIGNAL(pressed()), this, SLOT(pushed()));
}


ButtonViewImpl::~ButtonViewImpl()
{
}


void
ButtonViewImpl::setLabel(const std::string& label)
{
//    QPushButton::setText(QString::fromStdString(label));
    static_cast<QPushButton*>(_pNativeView)->setText(QString::fromStdString(label));
}


void
ButtonViewImpl::pushed()
{
    Omm::Gui::Log::instance()->gui().debug("button implementation, calling pushed virtual method");
    IMPL_NOTIFY_CONTROLLER(Controller, selected);
    IMPL_NOTIFY_CONTROLLER(ButtonController, pushed);
}


}  // namespace Omm
}  // namespace Gui
