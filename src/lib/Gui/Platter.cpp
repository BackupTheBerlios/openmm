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
#include <map>
#include <vector>

#include "Log.h"
#include "Gui/GuiLogger.h"
#include "Gui/Platter.h"
#include "Gui/Button.h"
#include "Gui/Layout.h"


namespace Omm {
namespace Gui {


class PlatterStackedLayout : public Layout
{
public:
    virtual void layoutView();

};


void
PlatterStackedLayout::layoutView()
{
    PlatterView* pView = static_cast<PlatterView*>(_pView);
    for (PlatterView::SubviewIterator it = pView->beginSubview(); it != pView->endSubview(); ++it) {
        (*it)->resize(pView->width(), pView->height() - (pView->_tabBarHidden ? 0 : pView->_barHeight));
        (*it)->move(0, (pView->_tabBarHidden ? 0 : pView->_barHeight));
    }
}


class HandleController : public ButtonController
{
public:
    HandleController(View* pView) : _pView(pView) {}

    virtual void pushed()
    {
        _pView->raise();
    }

private:
    View*   _pView;

};


PlatterView::PlatterView(View* pParent) :
View(pParent, true),
_barHeight(20),
_handleWidth(50),
_currentView(-1),
_tabBarHidden(false)
{
    LOG(gui, debug, "platter view ctor.");
    setLayout(new PlatterStackedLayout);
}


PlatterView::~PlatterView()
{
}


void
PlatterView::addView(View* pView, const std::string& name, bool show)
{
    if (show) {
        addSubview(pView);
        pView->resize(width(), height() - _barHeight);
        pView->move(0, _barHeight);
        Button* pHandleButton = new Button(this);
        pHandleButton->resize(_handleWidth, _barHeight);
        pHandleButton->move(_handles.size() * _handleWidth, 0);
        pHandleButton->setLabel(name);
        HandleController* pHandleController = new HandleController(pView);
        pHandleButton->attachController(pHandleController);
        _handles[pView] = pHandleButton;
        _views.push_back(pView);
        _currentView = _views.size() - 1;
    }
    else {
        SubviewIterator it = std::find(_views.begin(), _views.end(), pView);
        if (it != _views.end()) {
            _views.erase(it);
            _currentView = 0;
        }
    }
}


int
PlatterView::getTabCount()
{
    return _views.size();
}


int
PlatterView::getCurrentTab()
{
    LOG(gui, debug, "platter get current tab: " + Poco::NumberFormatter::format(_currentView));

    return _currentView;
}


void
PlatterView::setTabBarHidden(bool hidden)
{
    _tabBarHidden = hidden;
    for (SubviewIterator it = beginSubview(); it != endSubview(); ++it) {
        (*it)->resize(width(), height() - (hidden ? 0 : _barHeight));
        (*it)->move(0, (hidden ? 0 : _barHeight));
    }
}


void
PlatterView::setCurrentView(View* pView)
{
    SubviewIterator it = std::find(_views.begin(), _views.end(), pView);
    if (it != _views.end()) {
        pView->raise();
        _currentView = it - _views.begin();
    }
}


void
PlatterView::setCurrentTab(int index)
{
    LOG(gui, debug, "platter set current tab: " + Poco::NumberFormatter::format(index));

    _currentView = index;
    _views[_currentView]->raise();
}


PlatterView::SubviewIterator
PlatterView::beginSubview()
{
    return _views.begin();
}


PlatterView::SubviewIterator
PlatterView::endSubview()
{
    return _views.end();
}

} // namespace Gui
} // namespace Omm
