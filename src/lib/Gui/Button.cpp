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
#include "ButtonImpl.h"


namespace Omm {
namespace Gui {


ButtonModel::ButtonModel() :
_enabled(true),
_pImage(0)
{
}


//const std::string&
//ButtonModel::getLabel() const
//{
//    return _label;
//}


std::string
ButtonModel::getLabel()
{
    return _label;
}


void
ButtonModel::setLabel(const std::string& label)
{
//    LOG(gui, debug, "button model set label");
    _label = label;
}


bool
ButtonModel::getEnabled()
{
    return _enabled;
}


void
ButtonModel::setEnabled(bool enabled)
{
    _enabled = enabled;
}


Image*
ButtonModel::getImage()
{
    return _pImage;
}


void
ButtonModel::setImage(Image* pImage)
{
    _pImage = pImage;
}


ButtonView::ButtonView(View* pParent) :
View(pParent, false)
{
//    LOG(gui, debug, "button view ctor.");
    setName("button view");

    _minWidth = 30;
    _minHeight = 10;
    _prefWidth = 40;
    _prefHeight = 30;
    _pImpl = new ButtonViewImpl(this);
}


void
ButtonView::syncViewImpl()
{
//    LOG(gui, debug, "button view sync view: " + getName() + " ...");
    if (!_pModel) {
        return;
    }
    ButtonViewImpl* pImpl = static_cast<ButtonViewImpl*>(_pImpl);
    ButtonModel* pButtonModel = static_cast<ButtonModel*>(_pModel);
    pImpl->setLabel(pButtonModel->getLabel());
    pImpl->setEnabled(pButtonModel->getEnabled());
    if (pButtonModel->getImage()) {
        pImpl->setImage(pButtonModel->getImage());
    }
}


void
Button::setLabel(const std::string& label)
{
    ButtonModel::setLabel(label);
    syncView();
}


void
Button::setEnabled(bool enabled)
{
    ButtonModel::setEnabled(enabled);
    syncView();
}


void
Button::setImage(Image* pImage)
{
    ButtonModel::setImage(pImage);
    syncView();
}


} // namespace Gui
} // namespace Omm
