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
}


QtMediaRenderer::~QtMediaRenderer()
{
}


void
QtMediaRenderer::setDeviceWidget(QtMediaRendererWidget* pWidget)
{
    _pMediaRendererWidget = pWidget;
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
}


QtMediaRendererWidget::QtMediaRendererWidget()
{
    resize(600, 40);
    _pLayout = new QHBoxLayout(this);
    _pLabel = new QPushButton(this);
    _pBackButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipBackward), "", this);
    _pPlayButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaPlay), "", this);
    _pStopButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaStop), "", this);
    _pForwardButton = new QPushButton(style()->standardIcon(QStyle::SP_MediaSkipForward), "", this);

    _pVolumeSlider = new QSlider(Qt::Horizontal, this);
    _pVolumeSlider->setTracking(true);
    _pVolumeSlider->setSingleStep(5);
    _pSeekSlider = new QSlider(Qt::Horizontal, this);
    _pSeekSlider->setSingleStep(10);
    _pSeekSlider->setPageStep(25);

    _pLayout->addWidget(_pLabel);
    _pLayout->addWidget(_pBackButton);
    _pLayout->addWidget(_pPlayButton);
    _pLayout->addWidget(_pStopButton);
    _pLayout->addWidget(_pForwardButton);
    _pLayout->addWidget(_pVolumeSlider);
    _pLayout->addWidget(_pSeekSlider);

    connect(_pLabel, SIGNAL(pressed()), this, SLOT(rendererSelected()));
}


void
QtMediaRendererWidget::configure()
{
    _pLabel->setText(QString::fromStdString(_pMediaRenderer->getFriendlyName()));
}


void
QtMediaRendererWidget::rendererSelected()
{
    Omm::Av::Log::instance()->upnpav().debug("Qt media renderer widget selected renderer: " + _pMediaRenderer->getFriendlyName());
    emit selectedWidget(_row);
}


//void
//QtMediaRendererWidget::setRendererName(const std::string& name)
//{
//    _pLabel->setText(QString::fromStdString(name));
//}


void
QtMediaRendererWidget::setRow(int row)
{
    _row = row;
}
