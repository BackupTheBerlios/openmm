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

#include "ListItemImpl.h"
#include "Gui/ListItem.h"
#include "Gui/GuiLogger.h"


namespace Omm {
namespace Gui {


ListItemImpl::ListItemImpl(View* pView, View* pParent) :
QWidget(static_cast<QWidget*>(pParent ? pParent->getNativeView() : 0)),
ViewImpl(pView, this)
{
    _pLayout = new QHBoxLayout(this);
    _pNameLabel = new QLabel;
    _pLayout->addWidget(_pNameLabel);
    _pLayout->setSpacing(0);
    _pLayout->setMargin(0);
    _pLayout->setContentsMargins(0, 0, 0, 0);
    QWidget::setPalette(QPalette(Qt::white));
    QWidget::setAutoFillBackground(true);
}


ListItemImpl::~ListItemImpl()
{
    delete _pNameLabel;
}


void
ListItemImpl::setLabel(const std::string& text)
{
    _pNameLabel->setText(QString::fromStdString(text));
}


void
ListItemImpl::mousePressEvent(QMouseEvent* pMouseEvent)
{
    selected();
    QWidget::mousePressEvent(pMouseEvent);
}


}  // namespace Omm
}  // namespace Gui
