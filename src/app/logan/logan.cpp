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

#include "logan.h"


LoganMainWindow::LoganMainWindow(QWidget* pCentralWidget)
{
    setGeometry(0, 0, 800, 480);
    setCentralWidget(pCentralWidget);
}


LoganLogger::LoganLogger(QFileSystemWatcher* pMonitor)
{
  _logWidget.setupUi(this);
  _logWidget.logViewer->setReadOnly(true);
  _logWidget.logViewer->setLineWrapMode(QTextEdit::NoWrap);
  connect(pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));

//   _pFile = new QFile(pMonitor->files()[0]);
  _file.setFileName(pMonitor->files()[0]);
  _file.open(QIODevice::ReadOnly | QIODevice::Text);
//   _logWidget.logViewer->append(_file.readLine());
}


void
LoganLogger::fileChanged(const QString& path)
{
//   _logWidget.logViewer->append(_file.readLine());
  _logWidget.logViewer->insertPlainText(_file.readLine());
}


int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QFileSystemWatcher monitor;
  monitor.addPath(argv[1]);

  LoganLogger* pLogWidget = new LoganLogger(&monitor);
  QMainWindow* pMainWindow;
  pMainWindow = new LoganMainWindow(pLogWidget);
  pMainWindow->show();
    
  return app.exec();
}
