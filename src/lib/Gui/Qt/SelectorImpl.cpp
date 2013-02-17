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
#include "SelectorImpl.h"
#include "QtSelectorImpl.h"
#include "Gui/Selector.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


SelectorViewImpl::SelectorViewImpl(View* pView)
{
    QComboBox* pNativeView = new QComboBox;
    SelectorSignalProxy* pSignalProxy = new SelectorSignalProxy(this);

    initViewImpl(pView, pNativeView, pSignalProxy);
}


int
SelectorViewImpl::getCurrentIndex()
{
    return static_cast<QComboBox*>(_pNativeView)->currentIndex();
}


void
SelectorViewImpl::setCurrentIndex(int index)
{
    static_cast<QComboBox*>(_pNativeView)->setCurrentIndex(index);
}


void
SelectorViewImpl::clear()
{
    static_cast<QComboBox*>(_pNativeView)->clear();
}


void
SelectorViewImpl::addItem(const std::string& label, Image* pImage)
{
    static_cast<QComboBox*>(_pNativeView)->addItem(QString().fromStdString(label));
}


void
SelectorSignalProxy::init()
{
    SignalProxy::init();
    connect(_pViewImpl->getNativeView(), SIGNAL(activated(int)), this, SLOT(selected(int)));
}


void
SelectorSignalProxy::selected(int index)
{
    LOG(gui, debug, "selector selected index: " + Poco::NumberFormatter::format(index));
    PROXY_NOTIFY_CONTROLLER(SelectorController, selected, index);
}


}  // namespace Omm
}  // namespace Gui
