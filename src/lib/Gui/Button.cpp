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
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/ButtonImpl.h"
#endif


namespace Omm {
namespace Gui {


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
//    Omm::Gui::Log::instance()->gui().debug("button model set label");
    _label = label;

//    UPDATE_VIEWS(ButtonView, setLabel, label);
    syncViews();
}


ButtonView::ButtonView(View* pParent) :
View(pParent, false)
{
//    Omm::Gui::Log::instance()->gui().debug("button view ctor.");
    _minWidth = 30;
    _minHeight = 10;
    _prefWidth = 50;
    _prefHeight = 20;
    _pImpl = new ButtonViewImpl(this);
}


void
ButtonView::setImage(Image* pImage)
{
    static_cast<ButtonViewImpl*>(_pImpl)->setImage(pImage);
}


void
ButtonView::syncView(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("button view sync view: " + getName());
    ButtonModel* pButtonModel = static_cast<ButtonModel*>(pModel);
    static_cast<ButtonViewImpl*>(_pImpl)->setLabel(pButtonModel->getLabel());
}


} // namespace Gui
} // namespace Omm
