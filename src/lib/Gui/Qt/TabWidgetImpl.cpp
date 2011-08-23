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

#include "TabWidgetImpl.h"
#include "Gui/TabWidget.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


TabWidgetImpl::TabWidgetImpl(Widget* pWidget, Widget* pParent) :
QTabWidget(static_cast<QWidget*>(pParent? pParent->getNativeWidget() :0)),
WidgetImpl(pWidget, this)
{
    Omm::Gui::Log::instance()->gui().debug("tab widget implementation ctor");
}


TabWidgetImpl::~TabWidgetImpl()
{
}


void
TabWidgetImpl::addWidget(Widget* pWidget, const std::string& tabName)
{
    Omm::Gui::Log::instance()->gui().debug("tab widget implementation add widget");
    QTabWidget::addTab(static_cast<QWidget*>(pWidget->getNativeWidget()), tabName.c_str());
}


}  // namespace Omm
}  // namespace Gui
