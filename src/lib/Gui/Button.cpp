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

#include "Gui/Button.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/ButtonImpl.h"
#endif


namespace Omm {
namespace Gui {


const std::string&
ButtonModel::getLabel() const
{
    return _label;
}


void
ButtonModel::setLabel(const std::string& label)
{
    Omm::Gui::Log::instance()->gui().debug("button model set label");
    _label = label;
    static_cast<Button*>(_pWidget)->syncView();
}


Button::Button(Widget* pParent) :
Widget(new ButtonImpl(this, pParent), pParent)
{
    Omm::Gui::Log::instance()->gui().debug("button ctor.");
}


Button::Button(ButtonModel* pModel, Widget* pParent) :
Widget(new ButtonImpl(this, pParent), pParent)
{
    Omm::Gui::Log::instance()->gui().debug("button ctor with model.");
    if (!pModel) {
        setModel(this);
    }
    else {
        setModel(pModel);
    }
}


Button::~Button()
{
}


void
Button::pushed()
{
    Omm::Gui::Log::instance()->gui().debug("button pushed.");
    if (_pModel) {
        static_cast<ButtonModel*>(_pModel)->pushed();
    }
    syncView();
}


void
Button::syncView()
{
    Omm::Gui::Log::instance()->gui().debug("button sync view");
    ButtonModel* pModel = static_cast<ButtonModel*>(_pModel);
    static_cast<ButtonImpl*>(_pImpl)->setLabel(pModel->getLabel());
}


} // namespace Gui
} // namespace Omm
