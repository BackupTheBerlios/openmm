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

#include "QtApplication.h"
#include "QtController.h"


QtApplication::QtApplication()
{
}


QtApplication::~QtApplication()
{
}


void
QtApplication::addToolBar(QToolBar* pToolBar)
{
    _pMainWindow->addToolBar(Qt::BottomToolBarArea, pToolBar);
}


void
QtApplication::initApplication(int argc, char** argv)
{
    LOGNS(Omm, upnp, debug, "init qt application ...");

    _pApp = new QApplication(_argc, 0);

    _pMainWindow = new QMainWindow;
    _pController = new QtController(this);
    _pMainWindow->setCentralWidget(_pController);
    _pMainWindow->resize(800, 480);
    _pMainWindow->setWindowTitle("QtAvController");

//    connect(_pVisual, SIGNAL(showMenu(bool)), this, SLOT(showMenu(bool)));
//    connect(this, SIGNAL(volSliderMoved(int)), _pControlPanel, SIGNAL(volSliderMoved(int)));

    _pMainWindow->show();

    _pController->init();
    _pController->setState(Omm::Controller::Public);

    LOGNS(Omm, upnp, debug, "finished init qt application.");
}


void
QtApplication::setWindowTitle(const std::string& title)
{
    _pMainWindow->setWindowTitle(QString::fromStdString(title));
}


int
QtApplication::eventLoop()
{
    int ret = _pApp->exec();
    _pController->setState(Omm::Controller::Stopped);
    return ret;
}


void
QtApplication::quit()
{
    emit doQuit();
}


