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
#include "Log.h"


namespace Omm {
namespace Gui {



SliderModel::SliderModel() :
_enabled(true)
{

}


const int
SliderModel::getValue() const
{
    return _value;
}


void
SliderModel::setValue(int value)
{
//    LOG(gui, debug, "button model set label");
    _value = value;

    syncViews();
}


bool
SliderModel::getEnabled()
{
    return _enabled;
}


void
SliderModel::setEnabled(bool enabled)
{
    _enabled = enabled;
}


SliderView::SliderView(View* pParent) :
View(pParent, false),
_passiveMode(false),
_activeMode(false)
{
//    LOG(gui, debug, "slider view ctor.");
    _minWidth = 50;
    _minHeight = 10;
    _prefWidth = 150;
    _prefHeight = 15;

    _pImpl = new SliderViewImpl(this);
}


void
SliderView::syncViewImpl()
{
    if (_activeMode) {
        _activeMode = false;
        return;
    }
//    LOG(gui, debug, "slider view sync view: " + getName());
    SliderModel* pSliderModel = static_cast<SliderModel*>(_pModel);
    SliderViewImpl* pImpl = static_cast<SliderViewImpl*>(_pImpl);
    _passiveMode = true;
    pImpl->setValue(pSliderModel->getValue());
    pImpl->setEnabled(pSliderModel->getEnabled());
}


void
SliderView::valueChangedView(int value)
{
    if (_passiveMode) {
        _passiveMode = false;
        return;
    }
    _activeMode = true;
//    LOG(gui, debug, "slider view, calling value changed virtual method");
    NOTIFY_CONTROLLER(SliderController, valueChanged, value);
}



} // namespace Gui
} // namespace Omm
