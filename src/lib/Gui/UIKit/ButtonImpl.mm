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

#import <UIKit/UIKit.h>

#include <Poco/NumberFormatter.h>

#include "ButtonImpl.h"
#include "Gui/Button.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


ButtonViewImpl::ButtonViewImpl(View* pView, View* pParent)
//ViewImpl(pView, new QPushButton(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)))
{
    _pView = pView;
    Omm::Gui::Log::instance()->gui().debug("button view impl ctor");
//    UIButton* pNativeView = [[UIButton alloc] initWithFrame:CGRectMake(0.0, 0.0, 30.0, 20.0)];
    UIButton* pNativeView = [[UIButton alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

    _pNativeView = pNativeView;
    Omm::Gui::Log::instance()->gui().debug("button view impl ctor native view: " + Poco::NumberFormatter::format(_pNativeView));

    if (pParent) {
        UIView* pParentView = static_cast<UIView*>(pView->getParent()->getNativeView());
        [pParentView addSubview:pNativeView];
    }
//    connect(_pNativeView, SIGNAL(pressed()), this, SLOT(pushed()));
}


ButtonViewImpl::~ButtonViewImpl()
{
}


void
ButtonViewImpl::setLabel(const std::string& label)
{
    Omm::Gui::Log::instance()->gui().debug("button view impl set label");
    NSString* pLabel = [[NSString alloc] initWithUTF8String:label.c_str()];
    [static_cast<UIButton*>(_pNativeView) setTitle:pLabel forState:UIControlStateNormal];
}


//void
//ButtonViewImpl::pushed()
//{
//    Omm::Gui::Log::instance()->gui().debug("button implementation, calling pushed virtual method");
//    IMPL_NOTIFY_CONTROLLER(Controller, selected);
//    IMPL_NOTIFY_CONTROLLER(ButtonController, pushed);
//}


}  // namespace Omm
}  // namespace Gui
