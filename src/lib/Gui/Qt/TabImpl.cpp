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

#include "TabImpl.h"
#include "Gui/Tab.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


class QtTabWidget : public QTabWidget
{
    friend class TabViewImpl;

    void setHidden(bool hidden)
    {
        tabBar()->setHidden(hidden);
    }
};


TabViewImpl::TabViewImpl(View* pView)
{
    Omm::Gui::Log::instance()->gui().debug("tab widget implementation ctor");

    QtTabWidget* pNativeView = new QtTabWidget;

    initViewImpl(pView, pNativeView);
}


TabViewImpl::~TabViewImpl()
{
}


void
TabViewImpl::addView(View* pView, const std::string& tabName)
{
    Omm::Gui::Log::instance()->gui().debug("tab widget implementation add widget");
    static_cast<QtTabWidget*>(_pNativeView)->addTab(static_cast<QWidget*>(pView->getNativeView()), tabName.c_str());
}


int
TabViewImpl::getCurrentTab()
{
    static_cast<QtTabWidget*>(_pNativeView)->currentIndex();
}


void
TabViewImpl::setTabBarHidden(bool hidden)
{
    static_cast<QtTabWidget*>(_pNativeView)->setHidden(hidden);
}


void
TabViewImpl::setCurrentView(View* pView)
{
    static_cast<QtTabWidget*>(_pNativeView)->setCurrentWidget(static_cast<QWidget*>(pView->getNativeView()));
}


void
TabViewImpl::setCurrentTab(int index)
{
    static_cast<QtTabWidget*>(_pNativeView)->setCurrentIndex(index);
}


}  // namespace Omm
}  // namespace Gui
