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

#include "QtMediaRenderer.h"


QtMediaRenderer::QtMediaRenderer()
{
//    _pMediaRendererWidget = new QtMediaRendererWidget(this);
}


QtMediaRenderer::~QtMediaRenderer()
{
}


void
QtMediaRenderer::setDeviceWidget(QtMediaRendererWidget* pWidget)
{
    _pMediaRendererWidget = pWidget;
//    _pMediaRendererWidget->setText("bla");
}


QtMediaRendererWidget*
QtMediaRenderer::getDeviceWidget()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer get device Widget");
    
    return _pMediaRendererWidget;
}


void
QtMediaRenderer::initController()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer init controller");
    
//    _pMediaRendererWidget->setText(getFriendlyName().c_str());
}


//QtMediaRendererWidget::QtMediaRendererWidget(QtMediaRenderer* pMediaRenderer) :
//_pMediaRenderer(pMediaRenderer)
//{
//}


void
QtMediaRendererWidget::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);
    setText(_pMediaRenderer->getFriendlyName().c_str());
}
