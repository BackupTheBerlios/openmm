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

#include "ButtonImpl.h"
#include "QtButtonImpl.h"
#include "Gui/Button.h"
#include "Gui/GuiLogger.h"
#include "ImageImpl.h"

namespace Omm {
namespace Gui {


ButtonViewImpl::ButtonViewImpl(View* pView)
{
//    LOG(gui, debug, "button view impl ctor");
    QPushButton* pNativeView = new QPushButton;
    ButtonSignalProxy* pSignalProxy = new ButtonSignalProxy(this);

    initViewImpl(pView, pNativeView, pSignalProxy);
}


ButtonViewImpl::~ButtonViewImpl()
{
}


void
ButtonViewImpl::setLabel(const std::string& label)
{
    static_cast<QPushButton*>(_pNativeView)->setText(QString::fromStdString(label));
}


void
ButtonViewImpl::setEnabled(bool enabled)
{
   static_cast<QPushButton*>(_pNativeView)->setEnabled(enabled);
}


void
ButtonViewImpl::setImage(Image* pImage)
{
    ImageViewImpl* pImageImpl = static_cast<ImageViewImpl*>(static_cast<Image*>(pImage)->getViewImpl());
    static_cast<QPushButton*>(_pNativeView)->setIcon(QIcon(*pImageImpl->_pImage));
}


void
ButtonSignalProxy::init()
{
    SignalProxy::init();
    connect(_pViewImpl->getNativeView(), SIGNAL(pressed()), this, SLOT(pushed()));
}


void
ButtonSignalProxy::pushed()
{
    LOG(gui, debug, "button view impl, calling pushed virtual method");
    PROXY_NOTIFY_CONTROLLER(Controller, selected);
    PROXY_NOTIFY_CONTROLLER(ButtonController, pushed);
}


}  // namespace Omm
}  // namespace Gui
