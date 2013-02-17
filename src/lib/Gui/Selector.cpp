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

#include "Gui/Selector.h"
#include "Gui/GuiLogger.h"
#include "SelectorImpl.h"


namespace Omm {
namespace Gui {

SelectorView::SelectorView(View* pParent) :
View(pParent, false)
{
    setName("selector view");

    _minWidth = 30;
    _minHeight = 10;
    _prefWidth = 40;
    _prefHeight = 30;
    _pImpl = new SelectorViewImpl(this);
}


int
SelectorView::getCurrentIndex()
{
    return static_cast<SelectorViewImpl*>(_pImpl)->getCurrentIndex();
}


void
SelectorView::setCurrentIndex(int index)
{
    static_cast<SelectorViewImpl*>(_pImpl)->setCurrentIndex(index);
}


void
SelectorView::syncViewImpl()
{
    if (!_pModel) {
        return;
    }
    SelectorViewImpl* pImpl = static_cast<SelectorViewImpl*>(_pImpl);
    SelectorModel* pSelectorModel = dynamic_cast<SelectorModel*>(_pModel);
    if (!pSelectorModel) {
        LOG(gui, error, "wrong model for selector view.");
        return;
    }
    pImpl->clear();
    for (int index = 0; index < pSelectorModel->totalItemCount(); ++index) {
        pImpl->addItem(pSelectorModel->getItemLabel(index), 0);
    }
}

} // namespace Gui
} // namespace Omm
