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

#include "Util.h"
#include "Gui/GuiLogger.h"
#include "MediaImages.h"

#include "UpnpGui/ActivityIndicator.h"

namespace Omm {


ActivityIndicator::ActivityIndicator(Gui::View* pParent) :
Gui::ImageView(pParent),
_indicateDuration(250),
_activityInProgress(false),
_indicatorOn(false),
_timerActive(false),
_pOffTimer(0),
_stopIndicatorCallback(*this, &ActivityIndicator::stopIndicator)
{
    _pActivityOffModel = new Gui::ImageModel;
    _pActivityOffModel->setData(MediaImages::instance()->getResource("activity-off.png"));
    _pActivityOnModel = new Gui::ImageModel;
    _pActivityOnModel->setData(MediaImages::instance()->getResource("activity-on.png"));
    resize(20, 20);
    setModel(_pActivityOffModel);
    setBackgroundColor(Gui::Color("white"));
}


ActivityIndicator::~ActivityIndicator()
{
}


void
ActivityIndicator::startActivity()
{
    LOGNS(Gui, gui, debug, "activity indicator start activity");

    setActivityInProgress(true);
    if (!indicatorOn()) {
        setIndicatorOn(true);
        setModel(_pActivityOnModel);
    }
    else {
//        LOGNS(Gui, gui, debug, "indicator already on, do nothing");
    }
}


void
ActivityIndicator::stopActivity()
{
    LOGNS(Gui, gui, debug, "activity indicator stop activity");

    setActivityInProgress(false);
    if (indicatorOn() && !timerActive()) {
//        LOGNS(Gui, gui, debug, "turn off indicator after short delay ...");
        setTimerActive(true);
        if (_pOffTimer) {
            delete _pOffTimer;
        }
        _pOffTimer = new Poco::Timer;
        _pOffTimer->setStartInterval(_indicateDuration);
        _pOffTimer->start(_stopIndicatorCallback);
    }
    else {
//        LOGNS(Gui, gui, debug, "indicator already off or timer running, do nothing");
    }
}


void
ActivityIndicator::stopIndicator(Poco::Timer& timer)
{
//    LOGNS(Gui, gui, debug, "activity indicator stop timer callback ...");

    if (!activityInProgress() && indicatorOn()) {
        setModel(_pActivityOffModel);
        setIndicatorOn(false);
//        LOGNS(Gui, gui, debug, "indicator turned off, no activity in progress anymore");
    }
    else {
//        LOGNS(Gui, gui, debug, "turn off indicator ignored, activity still in progress or indicator already off");
    }
    setTimerActive(false);
//    LOGNS(Gui, gui, debug, "activity indicator stop timer callback finished.");
}


void
ActivityIndicator::setActivityInProgress(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    if (set) {
//        LOGNS(Gui, gui, debug, "indicator flag \"activity in progress\" set to true");
    }
    else {
//        LOGNS(Gui, gui, debug, "indicator flag \"activity in progress\" set to false");
    }
    _activityInProgress = set;
}


bool
ActivityIndicator::activityInProgress()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_activityInProgressLock);
    return _activityInProgress;
}


void
ActivityIndicator::setIndicatorOn(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    if (set) {
//        LOGNS(Gui, gui, debug, "flag \"indicator on\" set to true");
    }
    else {
//        LOGNS(Gui, gui, debug, "flag \"indicator on\" set to false");
    }
    _indicatorOn = set;
}


bool
ActivityIndicator::indicatorOn()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_indicatorOnLock);
    return _indicatorOn;
}


bool
ActivityIndicator::timerActive()
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    return _timerActive;
}


void
ActivityIndicator::setTimerActive(bool set)
{
    Poco::ScopedLock<Poco::FastMutex> locker(_timerActiveLock);
    _timerActive = set;
}


} // namespace Omm
