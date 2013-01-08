/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2013                                                       |
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

#include "Gui/Stack.h"
#include "Gui/Layout.h"
#include "Gui/GuiLogger.h"
#include "Log.h"


namespace Omm {
namespace Gui {


class PrivateStackController : public Controller
{
    friend class StackView;

    PrivateStackController(StackView* pView) : _pView(pView) {}

    virtual void removedSubview(View* pView)
    {
        _pView->_views.erase(pView);
    }

    StackView*  _pView;
};


class StackLayout : public Layout
{
public:
    virtual void layoutView();
};


void
StackLayout::layoutView()
{
    LOG(gui, debug, "stack view update layout");

    StackView* pView = static_cast<StackView*>(_pView);
    for (std::set<View*>::iterator it = pView->_views.begin(); it != pView->_views.end(); ++it) {
        (*it)->resize(pView->width(), pView->height());
    }
}


StackView::StackView(View* pParent) :
View(pParent, true),
_pCurrentView(0),
_pLastView(0)
{
    setName("stack");
    setLayout(new StackLayout);
    attachController(new PrivateStackController(this));
}


void
StackView::insertView(View* pView)
{
    pView->setParent(this);
    _views.insert(pView);
    updateLayout();
    setCurrentView(pView);
}


void
StackView::setCurrentView(View* pView)
{
    if (_pCurrentView == pView) {
        return;
    }
    _pLastView = _pCurrentView;
    _pCurrentView = pView;
    syncView();
}


void
StackView::syncViewImpl()
{
    LOG(gui, debug, "stack view sync view impl");

    setEnableRedraw(false);
    if (_pLastView) {
        _pLastView->hide(false);
    }
    _pCurrentView->show(false);
    _pCurrentView->raise(false);
    setEnableRedraw(true);
    NOTIFY_CONTROLLER(StackController, shownView, _pCurrentView);
}


} // namespace Gui
} // namespace Omm
