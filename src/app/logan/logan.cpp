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

#include <iostream>

#include <qt4/QtCore/qfile.h>

#include "logan.h"


LoganMainWindow::LoganMainWindow(QWidget* pCentralWidget)
{
    setCentralWidget(pCentralWidget);
    resize(800, 480);
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
    init();
}


void
LoganLogger::init()
{
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
}


bool
LoganLogger::isLogEntry(const QString& line)
{
    return line.length() > 30 && line[2] == ':' && line[5] == ':' && line[8] == '.';
}


QChar
LoganLogger::debugLevel(const QString& line)
{
    return line[line.indexOf(']') + 2];
}


void
LoganLogger::appendLine(const QString& line)
{
    if (isLogEntry(line)) {
        QChar level = debugLevel(line);
        if (level == 'E') {
            _logWidget.logViewer->setTextColor(Qt::red);
        }
        else if (level == 'I') {
            _logWidget.logViewer->setTextColor(Qt::blue);
        }
        else {
            _logWidget.logViewer->setTextColor(Qt::black);
        }
    }
    else {
        _logWidget.logViewer->setTextColor(Qt::black);
    }
    _logWidget.logViewer->append(line.trimmed());
}


void
LoganLogger::fileChanged(const QString& path)
{
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
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
