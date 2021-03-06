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
#include "LabelImpl.h"
#include "Log.h"


namespace Omm {
namespace Gui {


LabelModel::LabelModel(const std::string& label) :
_label(label)
{
}


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
//    LOG(gui, debug, "Label model set label");
    _label = label;
}


LabelView::LabelView(View* pParent) :
View(pParent, false)
{
//    LOG(gui, debug, "Label view ctor.");
    setName("label view");

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
LabelView::syncViewImpl()
{
//    LOG(gui, debug, "Label view sync view: " + getName());
//    LOG(gui, debug, "Label view sync view with model: " + Poco::NumberFormatter::format(_pModel));
    LabelModel* pLabelModel = static_cast<LabelModel*>(_pModel);
    LabelViewImpl* pImpl = static_cast<LabelViewImpl*>(_pImpl);
    if (pLabelModel) {
        pImpl->setLabel(pLabelModel->getLabel());
    }
}


void
Label::setLabel(const std::string& label)
{
//    LOG(gui, debug, "Label model set label");
    LabelModel::setLabel(label);
    syncView();
}


} // namespace Gui
} // namespace Omm
