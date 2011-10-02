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

#include "Gui/Label.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/LabelImpl.h"
#endif
#ifdef __GUI_UIKIT_PLATFORM__
#include "UIKit/LabelImpl.h"
#endif


namespace Omm {
namespace Gui {


//const std::string&
//LabelModel::getLabel() const
//{
//    return _label;
//}


std::string
LabelModel::getLabel()
{
    return _label;
}


void
LabelModel::setLabel(const std::string& label)
{
//    Omm::Gui::Log::instance()->gui().debug("Label model set label");
    _label = label;

//    UPDATE_VIEWS(LabelView, setLabel, label);
    syncViews();
}


LabelView::LabelView(View* pParent) :
View(pParent, false)
{
//    Omm::Gui::Log::instance()->gui().debug("Label view ctor.");
    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 20;
    _pImpl = new LabelViewImpl(this);
}


void
LabelView::setAlignment(Alignment alignment)
{
    static_cast<LabelViewImpl*>(_pImpl)->setAlignment(alignment);
}


void
LabelView::syncView(Model* pModel)
{
//    Omm::Gui::Log::instance()->gui().debug("Label view sync view: " + getName());
    LabelModel* pLabelModel = static_cast<LabelModel*>(pModel);
    LabelViewImpl* pImpl = static_cast<LabelViewImpl*>(_pImpl);
    pImpl->setLabel(pLabelModel->getLabel());
}


} // namespace Gui
} // namespace Omm
