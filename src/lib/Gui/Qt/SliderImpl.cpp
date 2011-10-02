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

#include "SliderImpl.h"
#include "Gui/Slider.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


SliderViewImpl::SliderViewImpl(View* pView)
{
    QSlider* pNativeView = new QSlider;

//    Omm::Gui::Log::instance()->gui().debug("slider view impl ctor");
    pNativeView->setOrientation(Qt::Horizontal);
    pNativeView->setTracking(true);
    pNativeView->setSingleStep(5);
    pNativeView->setPageStep(25);
    pNativeView->setValue(0);
    connect(pNativeView, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));

    initViewImpl(pView, pNativeView);
}


SliderViewImpl::~SliderViewImpl()
{
}


void
SliderViewImpl::setValue(int value)
{
    QSlider* pNativeView = static_cast<QSlider*>(_pNativeView);

    pNativeView->setValue(value);
}


void
SliderViewImpl::valueChangedSlot(int value)
{
    Omm::Gui::Log::instance()->gui().debug("slider implementation, calling value changed virtual method");
    IMPL_NOTIFY_CONTROLLER(SliderController, valueChanged, value);
}


}  // namespace Omm
}  // namespace Gui
