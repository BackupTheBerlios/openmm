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

#include "QtStdApplication.h"
#include "QtMainWindow.h"
#include "QtVisual.h"
#include "QtEventFilter.h"
#include "QtStdController.h"

Q_DECLARE_METATYPE(std::string);


QtStdApplication::QtStdApplication()
{
}


QtStdApplication::~QtStdApplication()
{
}


void
QtStdApplication::addToolBar(QToolBar* pToolBar)
{
    _pMainWindow->addToolBar(Qt::BottomToolBarArea, pToolBar);
}


void
QtStdApplication::initApplication(int argc, char** argv)
{
    LOGNS(Omm, upnp, debug, "init qt std application ...");

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
QtStdApplication::createController()
{
    return new QtStdController(this);
}


void
QtStdApplication::addController()
{
    _pMainWidget->addWidget(static_cast<QtStdController*>(_pController));
}


void
QtStdApplication::removeController()
{
    _pMainWidget->removeWidget(static_cast<QtStdController*>(_pController));
}


void
QtStdApplication::setWindowTitle(const std::string& title)
{
    _pMainWindow->setWindowTitle(QString::fromStdString(title));
}


void
QtStdApplication::eventLoop()
{
    _pApp->exec();
}


void
QtStdApplication::quit()
{
    emit doQuit();
}

//void
//QtApplication::showMainWindow()
//{
//    _pMainWindow->show();
//}


/*

 *
 * Better use something like Poco::Util::ServerApplication::waitForTerminationRequest()
 * unix style:
void ServerApplication::waitForTerminationRequest()
{
	sigset_t sset;
	sigemptyset(&sset);
	if (!std::getenv("POCO_ENABLE_DEBUGGER"))
	{
		sigaddset(&sset, SIGINT);
	}
	sigaddset(&sset, SIGQUIT);
	sigaddset(&sset, SIGTERM);
	sigprocmask(SIG_BLOCK, &sset, NULL);
	int sig;
	sigwait(&sset, &sig);
}
 *
 * windows style:
 void ServerApplication::waitForTerminationRequest()
{
	SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
	std::string evName("POCOTRM");
	NumberFormatter::appendHex(evName, Process::id(), 8);
	NamedEvent ev(evName);
	ev.wait();
	_terminated.set();
}

 */


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::UpnpApplication)
POCO_EXPORT_CLASS(QtStdApplication)
POCO_END_MANIFEST
#endif
