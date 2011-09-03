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

#include "Gui/Slider.h"
#include "Gui/GuiLogger.h"

#ifdef __GUI_QT_PLATFORM__
#include "Qt/SliderImpl.h"
#endif


namespace Omm {
namespace Gui {


const int
SliderModel::getValue() const
{
    return _value;
}


void
SliderModel::setValue(int value)
{
//    Omm::Gui::Log::instance()->gui().debug("button model set label");
    _value = value;

//    UPDATE_VIEWS(SliderView, setLabel, label);
    syncViews();
}


SliderView::SliderView(View* pParent) :
View(new SliderViewImpl(this, pParent), pParent)
{
    Omm::Gui::Log::instance()->gui().debug("slider view ctor.");
}


void
SliderView::syncView(Model* pModel)
{
    Omm::Gui::Log::instance()->gui().debug("slider view sync view: " + getName());
    SliderModel* pSliderModel = static_cast<SliderModel*>(pModel);
    SliderViewImpl* pImpl = static_cast<SliderViewImpl*>(_pImpl);
    pImpl->setValue(pSliderModel->getValue());
}


} // namespace Gui
} // namespace Omm
