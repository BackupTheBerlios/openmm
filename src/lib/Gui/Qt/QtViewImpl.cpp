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

#include "ViewImpl.h"
#include "QtViewImpl.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


    
SignalProxy::SignalProxy(ViewImpl* pViewImpl):
 _pViewImpl(pViewImpl)
{
}


void
SignalProxy::init()
{
    connect(this, SIGNAL(showViewSignal()), _pViewImpl->_pNativeView, SLOT(show()));
    connect(this, SIGNAL(hideViewSignal()), _pViewImpl->_pNativeView, SLOT(hide()));
    connect(this, SIGNAL(syncViewSignal()), this, SLOT(syncViewSlot()));    
}


void
SignalProxy::showView()
{
    emit showViewSignal();
}


void
SignalProxy::hideView()
{
    emit hideViewSignal();
}


void
SignalProxy::syncView()
{
    emit syncViewSignal();
}


void
SignalProxy::syncViewSlot()
{
    _pViewImpl->_pView->syncViewImpl();
}
        
        
}  // namespace Omm
}  // namespace Gui
