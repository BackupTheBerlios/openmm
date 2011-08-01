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
    _pMediaRendererWidget = new QtMediaRendererWidget(this);
}


QtMediaRenderer::~QtMediaRenderer()
{
}


QWidget*
QtMediaRenderer::getWidget()
{
    return _pMediaRendererWidget;
}


void
QtMediaRenderer::initController()
{
    _pMediaRendererWidget->_pLabel->setText(getFriendlyName().c_str());
}


QtMediaRendererWidget::QtMediaRendererWidget(QtMediaRenderer* pMediaRenderer) :
_pMediaRenderer(pMediaRenderer)
{
    _pLabel = new QLabel(this);
}
