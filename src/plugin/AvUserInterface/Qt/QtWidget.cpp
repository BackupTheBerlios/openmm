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
#include <Omm/UpnpAvLogger.h>
#include "QtWidget.h"


QtWidget::QtWidget(QWidget* pParent) :
QWidget(pParent)
{
}


void
QtWidget::showWidget()
{
    QWidget::show();
}


void
QtWidget::hideWidget()
{
    QWidget::hide();
}


//void
//QtWidget::mousePressEvent(QMouseEvent* pMouseEvent)
//{
//    Omm::Av::Log::instance()->upnpav().debug("QtWidget mouse pressed in widget");
//    select();
//    QWidget::mousePressEvent(pMouseEvent);
//}


QtListWidget::QtListWidget(QWidget* pParent) :
QWidget(pParent)
{

}


void
QtListWidget::showWidget()
{
    QWidget::show();
}


void
QtListWidget::hideWidget()
{
    QWidget::hide();
}


void
QtListWidget::mousePressEvent(QMouseEvent* pMouseEvent)
{
    Omm::Av::Log::instance()->upnpav().debug("QtListWidget mouse pressed in widget with row: " + Poco::NumberFormatter::format(getRow()));
    select();
    QWidget::mousePressEvent(pMouseEvent);
}


QtSimpleListWidget::QtSimpleListWidget(QWidget* pParent) :
QtListWidget(pParent)
{
    _pLayout = new QHBoxLayout(this);
    _pNameLabel = new QLabel;
//    _pNameLabel->setBackgroundRole(QPalette::Shadow);
    _pLayout->addWidget(_pNameLabel);
    _pLayout->setSpacing(0);
    _pLayout->setMargin(0);
    _pLayout->setContentsMargins(0, 0, 0, 0);
//    QWidget::setBackgroundColor(QColor::fromRgb(255, 255, 255, 255));
//    QWidget::setBackgroundRole(QPalette::Highlight);
//    QWidget::setBackgroundRole(QPalette::NoRole);
//    QWidget::setBackgroundRole(QPalette::Light);
//    QWidget::setBackgroundRole(QPalette::Shadow);
//    QWidget::setBackgroundRole(QPalette(QColor("white")));
    QWidget::setPalette( QPalette( Qt::white ) );
    QWidget::setAutoFillBackground( true );
}


QtSimpleListWidget::~QtSimpleListWidget()
{
    delete _pNameLabel;
}


void
QtSimpleListWidget::setLabel(const std::string& text)
{
    _pNameLabel->setText(QString::fromStdString(text));
}