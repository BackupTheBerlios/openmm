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

#include <Poco/ClassLibrary.h>

#include "QtApplication.h"
#include "QtMainWindow.h"
#include "QtVisual.h"
#include "QtEventFilter.h"
#include "QtController.h"

Q_DECLARE_METATYPE(std::string);


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

//    setupUnixSignalHandlers();
    qRegisterMetaType<std::string>();

    _fullscreenStyleSheet =
            "*         { font-size: 32pt } \
             QTreeView { background-color: black;\
                         selection-color: white; \
                         selection-background-color: darkblue; } \
             QTreeView { color: white }";

    // TODO: command line arguments can be passed after Poco::Application has processed them.
    _pApp = new QApplication(_argc, 0);
//    _pEventFilter = new QtEventFilter(this);
//    _pApp->installEventFilter(_pEventFilter);
//    connect(this, SIGNAL(doQuit()), _pApp, SLOT(quit()));
//    installSignalHandler();

    _pMainWidget = new QStackedWidget;
    _pMainWindow = new QtMainWindow(_pMainWidget);


//    _pVisual = new QtVisual(_pMainWidget);
//
//    _pBrowserWidget = new QtBrowserWidget(_pMainWindow, this);
//
//    _pPlayerRack = new QtPlayerRack(this);
//
//    _pMainWidget->addWidget(_pVisual);
//    _pMainWidget->addWidget(_pBrowserWidget);
//    _pMainWidget->setCurrentWidget(_pBrowserWidget);
//    _menuVisible = true;
//
//    _pMainWindow->addDockWidget(Qt::RightDockWidgetArea, _pPlayerRack);
//    _pMainWindow->setWindowTitle("OMM");
//
//    _pControlPanel = new QtControlPanel(this, _pMainWindow);
//    _pMainWindow->addToolBar(Qt::BottomToolBarArea, _pControlPanel);
//
//    connect(_pVisual, SIGNAL(showMenu(bool)), this, SLOT(showMenu(bool)));
//    connect(this, SIGNAL(volSliderMoved(int)), _pControlPanel, SIGNAL(volSliderMoved(int)));

    _pMainWindow->show();

    LOGNS(Omm, upnp, debug, "finished init qt application.");
}


Omm::Controller*
QtApplication::createController()
{
    return new QtController(this);
}


void
QtApplication::addController()
{
    _pMainWidget->addWidget(static_cast<QtController*>(_pController));
}


void
QtApplication::removeController()
{
    _pMainWidget->removeWidget(static_cast<QtController*>(_pController));
}


void
QtApplication::setWindowTitle(const std::string& title)
{
    _pMainWindow->setWindowTitle(QString::fromStdString(title));
}


void
QtApplication::eventLoop()
{
    _pApp->exec();
}


void
QtApplication::quit()
{
    emit doQuit();
}

//void
//QtApplication::showMainWindow()
//{
//    _pMainWindow->show();
//}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::UpnpApplication)
POCO_EXPORT_CLASS(QtApplication)
POCO_END_MANIFEST
#endif
