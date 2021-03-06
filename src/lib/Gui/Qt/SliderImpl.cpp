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

#include <QtGui>
#include <Poco/NumberFormatter.h>

#include "SliderImpl.h"
#include "QtSliderImpl.h"
#include "Gui/Slider.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


SliderViewImpl::SliderViewImpl(View* pView)
{
    QSlider* pNativeView = new QSlider;
    SliderSignalProxy* pSignalProxy = new SliderSignalProxy(this);

//    LOG(gui, debug, "slider view impl ctor");
    pNativeView->setOrientation(Qt::Horizontal);
    pNativeView->setTracking(true);
    pNativeView->setSingleStep(5);
    pNativeView->setPageStep(25);
    pNativeView->setValue(0);

    initViewImpl(pView, pNativeView, pSignalProxy);
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
SliderViewImpl::setEnabled(bool enabled)
{
    static_cast<QSlider*>(_pNativeView)->setEnabled(enabled);
}


void
SliderSignalProxy::init()
{
//    LOG(gui, debug, "slider view impl, init signal proxy");
    SignalProxy::init();
    connect(_pViewImpl->getNativeView(), SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
}


void
SliderSignalProxy::valueChangedSlot(int value)
{
//    LOG(gui, debug, "slider view impl, calling value changed");
    static_cast<SliderView*>(_pViewImpl->getView())->valueChangedView(value);
}


}  // namespace Omm
}  // namespace Gui
