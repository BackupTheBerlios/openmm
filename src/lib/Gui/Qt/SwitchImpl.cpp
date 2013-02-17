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
#include <qt4/QtGui/qcombobox.h>

#include "Gui/Image.h"
#include "SwitchImpl.h"
#include "QtSwitchImpl.h"
#include "Gui/Switch.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


SwitchViewImpl::SwitchViewImpl(View* pView)
{
    QCheckBox* pNativeView = new QCheckBox;
    SwitchSignalProxy* pSignalProxy = new SwitchSignalProxy(this);

    initViewImpl(pView, pNativeView, pSignalProxy);
}


bool
SwitchViewImpl::getStateOn()
{
    Qt::CheckState res = static_cast<QCheckBox*>(_pNativeView)->checkState();
    switch(res) {
        case Qt::Checked:
            return true;
        case Qt::Unchecked:
            return false;
    }
}


void
SwitchViewImpl::setState(bool on)
{
    static_cast<QCheckBox*>(_pNativeView)->setCheckState(on ? Qt::Checked : Qt::Unchecked);
}


void
SwitchSignalProxy::init()
{
    SignalProxy::init();
    connect(_pViewImpl->getNativeView(), SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
}


void
SwitchSignalProxy::stateChanged(int state)
{
    bool switchOn = false;
    switch(state) {
        case Qt::Checked:
            switchOn = true;
            break;
        case Qt::Unchecked:
            switchOn = false;
            break;
    }
    LOG(gui, debug, "switch switched: " + std::string(switchOn ? "on" : "off"));
    PROXY_NOTIFY_CONTROLLER(SwitchController, switched, switchOn);
}


}  // namespace Omm
}  // namespace Gui
