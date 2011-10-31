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
#include "SliderImpl.h"


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
View(pParent, false)
{
//    Omm::Gui::Log::instance()->gui().debug("slider view ctor.");
    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 15;

    _pImpl = new SliderViewImpl(this);
}


void
SliderView::syncViewImpl()
{
//    Omm::Gui::Log::instance()->gui().debug("slider view sync view: " + getName());
    SliderModel* pSliderModel = static_cast<SliderModel*>(_pModel);
    SliderViewImpl* pImpl = static_cast<SliderViewImpl*>(_pImpl);
    pImpl->setValue(pSliderModel->getValue());
}


} // namespace Gui
} // namespace Omm
