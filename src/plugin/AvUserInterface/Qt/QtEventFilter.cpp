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
#include "QtEventFilter.h"
#include "QtAvInterface.h"
#include "QtBrowserWidget.h"


QtEventFilter::QtEventFilter()
{
}


QtEventFilter::QtEventFilter(QtAvInterface* avInterface) :
_pAvInterface(avInterface),
_pApplication(0)
{
}


bool
QtEventFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        LOGNS(Omm::Av, upnpav, debug, "key pressed: " + keyEvent->text().toStdString() + ", key no: " + Poco::NumberFormatter::format(keyEvent->key()));
//        if (Controler::instance()->getCurrentPage()->hasEventType(m_eventMap.find(k->key())->second)) {
//            Controler::instance()->queueEvent(new Event(m_eventMap.find(k->key())->second));
//            // don't forward the event to the Qt event loop.
//            return true;
//        }
        // menu
        if (keyEvent->text() == "m" || keyEvent->key() == 16777301) {
            _pAvInterface->showMenu(!_pAvInterface->menuVisible());
            return true;
        }
        // fullscreen
        else if (keyEvent->text() == "x") {
            _pAvInterface->setFullscreen(!_pAvInterface->isFullscreen());
            return true;
        }
        // power
        else if (keyEvent->key() == 16777399) {
            return true;
        }
        // vol up
        else if (((keyEvent->modifiers() & Qt::AltModifier) && keyEvent->key() == 16777235) || keyEvent->key() == 16777330) {
            LOGNS(Omm::Av, upnpav, debug, "vol up key");
//            int oldValue = _pAvInterface->_pVolumeSlider->value();
//            _pAvInterface->volumeChanged(++oldValue);
            return true;
        }
        // vol down
        else if (((keyEvent->modifiers() & Qt::AltModifier) && keyEvent->key() == 16777237) || keyEvent->key() == 16777328) {
            LOGNS(Omm::Av, upnpav, debug, "vol down key");
//            int oldValue = _pAvInterface->_pVolumeSlider->value();
//            _pAvInterface->volumeChanged(--oldValue);
            return true;
        }
        // chan up
        else if (((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == 16777235) || keyEvent->key() == 16777238) {
            _pAvInterface->skipForwardButtonPressed();
            return true;
        }
        // chan down
        else if (((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == 16777237) || keyEvent->key() == 16777239) {
            _pAvInterface->skipBackwardButtonPressed();
            return true;
        }
        // back
        else if (keyEvent->key() == 16777219) {
            _pAvInterface->_pBrowserWidget->goBack();
            return true;
        }
        // play
        else if (keyEvent->text() == "p") {
            _pAvInterface->playPressed();
            return true;
        }
        // stop
        else if (keyEvent->text() == "s") {
            _pAvInterface->stopPressed();
            return true;
        }
        // skip fwd
        else if (keyEvent->text() == "f") {
            _pAvInterface->skipForwardButtonPressed();
            return true;
        }
        // skip back
        else if (keyEvent->text() == "b") {
            _pAvInterface->skipBackwardButtonPressed();
            return true;
        }
        // mute
        else if (keyEvent->text() == " " || keyEvent->key() == 16777329) {
            return true;
        }
    }
    // standard event processing in the Qt event loop.
    return false;
}
