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
#include "QtMainWindow.h"


QtMainWindow::QtMainWindow(QWidget* pCentralWidget)
{
    setCentralWidget(pCentralWidget);
    resize(800, 480);
}


//void
//QtMainWindow::keyPressEvent(QKeyEvent* event)
//{
//    Omm::Av::Log::instance()->upnpav().debug("key pressed: " + event->text().toStdString() + ", key no: " + Poco::NumberFormatter::format(event->key()));
//}

