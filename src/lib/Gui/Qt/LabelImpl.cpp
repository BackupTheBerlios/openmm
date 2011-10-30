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

#include "LabelImpl.h"
#include "Gui/Label.h"
#include "Gui/GuiLogger.h"

namespace Omm {
namespace Gui {


LabelViewImpl::LabelViewImpl(View* pView)
{
//    Omm::Gui::Log::instance()->gui().debug("label view impl ctor");
    QLabel* pNativeView = new QLabel;

    initViewImpl(pView, pNativeView);
}


LabelViewImpl::~LabelViewImpl()
{
}


void
LabelViewImpl::setLabel(const std::string& label)
{
    static_cast<QLabel*>(_pNativeView)->setText(QTextCodec::codecForName("UTF-8")->toUnicode(label.c_str()));
}


void
LabelViewImpl::setAlignment(View::Alignment alignment)
{
    switch(alignment) {
        case View::AlignLeft:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignLeft);
            break;
        case View::AlignCenter:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignCenter);
            break;
        case View::AlignRight:
            static_cast<QLabel*>(_pNativeView)->setAlignment(Qt::AlignRight);
            break;
    }
}

}  // namespace Omm
}  // namespace Gui
