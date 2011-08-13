/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <QPainter>

#include "QtActivityIndicator.h"

QtActivityIndicator::QtActivityIndicator(QWidget* parent, Qt::WindowFlags flags) :
QWidget(parent, flags),
_indicateDuration(250),
_activityInProgress(false),
_indicatorOn(false)
{
    _symbolRenderer = new QSvgRenderer(this);
    _symbolRenderer->load(QString(":/images/circle_grey.svg"));
    update();
    // NOTE: if Qt::WA_OpaquePaintEvent is set, repaint() is faster,
    // but root window shines through transparent parts of the image
//     setAttribute(Qt::WA_OpaquePaintEvent, true);
    _offTimer.setInterval(_indicateDuration);
    _offTimer.setSingleShot(true);
    connect(&_offTimer, SIGNAL(timeout()), this, SLOT(stopIndicator()));

    setMinimumSize(20, 20);
    QSizePolicy sizePolicy;
    sizePolicy.setHeightForWidth(true);
    setSizePolicy(sizePolicy);
}


QtActivityIndicator::~QtActivityIndicator()
{
    delete _symbolRenderer;
}


void
QtActivityIndicator::startActivity()
{
    setActivityInProgress(true);
    if (!indicatorOn()) {
        setIndicatorOn(true);
        _symbolRenderer->load(QString(":/images/circle_purple.svg"));
//         update();
        repaint();
//        Omm::Av::Log::instance()->upnpav().trace("INDICATOR TURNED ON");
        // QWidget::update() is called asynchronously and gets therefore executed later sometimes.
        // Qt docs of slot Widget::update():
        // This function does not cause an immediate repaint; instead it schedules a paint event for processing when Qt returns
        // to the main event loop. This permits Qt to optimize for more speed and less flicker than a call to repaint() does.
        // we can set Qt::WA_OpaquePaintEvent as widget attribute
        // so we use QWidget::repaint() instead:
    }
    else {
//        Omm::Av::Log::instance()->upnpav().trace("indicator already on, do nothing");
    }
}


void
QtActivityIndicator::stopActivity()
{
    // NOTE: starting this timer only works when started from a Qt event loop thread.
    // with actions triggered by gui elements, this is ok. User QThread::exec() ?
    setActivityInProgress(false);
    if (indicatorOn() && !_offTimer.isActive()) {
//        Omm::Av::Log::instance()->upnpav().trace("turn off indicator after short delay ...");
        _offTimer.start();
    }
    else {
//        Omm::Av::Log::instance()->upnpav().trace("indicator already off or timer running, do nothing");
    }
}


void
QtActivityIndicator::stopIndicator()
{
    if (!activityInProgress() && indicatorOn()) {
        _symbolRenderer->load(QString(":/images/circle_grey.svg"));
//         update();
        repaint();
        setIndicatorOn(false);
//        Omm::Av::Log::instance()->upnpav().trace("INDICATOR TURNED OFF, no activity in progress anymore");
    }
    else {
//        Omm::Av::Log::instance()->upnpav().trace("turn off indicator ignored, activity still in progress or indicator already off");
    }
}


void
QtActivityIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    _symbolRenderer->render(&painter);
}


void
QtActivityIndicator::setActivityInProgress(bool set)
{
    QMutexLocker locker(&_activityInProgressLock);
    if (set) {
//        Omm::Av::Log::instance()->upnpav().trace("flag \"activity in progress\" set to true");
    }
    else {
//        Omm::Av::Log::instance()->upnpav().trace("flag \"activity in progress\" set to false");
    }
    _activityInProgress = set;
}


bool
QtActivityIndicator::activityInProgress()
{
    QMutexLocker locker(&_activityInProgressLock);
    return _activityInProgress;
}


void
QtActivityIndicator::setIndicatorOn(bool set)
{
    QMutexLocker locker(&_indicatorOnLock);
    if (set) {
//        Omm::Av::Log::instance()->upnpav().trace("flag \"indicator on\" set to true");
    }
    else {
//        Omm::Av::Log::instance()->upnpav().trace("flag \"indicator on\" set to false");
    }
    _indicatorOn = set;
}


bool
QtActivityIndicator::indicatorOn()
{
    QMutexLocker locker(&_indicatorOnLock);
    return _indicatorOn;
}
