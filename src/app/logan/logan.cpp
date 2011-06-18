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


LoganLogger::LoganLogger(QFileSystemWatcher* pMonitor) :
_pMonitor(pMonitor),
_filter("")
//_channelFilter("")
{
    _logWidget.setupUi(this);
    _logWidget.logViewer->setReadOnly(true);
    _logWidget.logViewer->setLineWrapMode(QTextEdit::NoWrap);
}


void
LoganLogger::init()
{
    _file.setFileName(_pMonitor->files()[0]);
    _file.open(QIODevice::ReadOnly | QIODevice::Text);
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    connect(_logWidget.filterEdit, SIGNAL(editingFinished()), this, SLOT(filterChanged()));
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
LoganLogger::colorLine(const QString& line)
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
}


void
LoganLogger::clear()
{
    _logWidget.logViewer->clear();
}


void
LoganLogger::reread()
{
    disconnect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    _file.reset();
    clear();
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
}


void
LoganLogger::appendLine(const QString& line)
{
    colorLine(line);
    if (_filter.length() == 0 || line.indexOf(_filter) != -1) {
        _logWidget.logViewer->insertPlainText(line);
        _logWidget.logViewer->moveCursor(QTextCursor::End);
    }
}


void
LoganLogger::fileChanged(const QString& path)
{
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
}


void
LoganLogger::filterChanged()
{
    _filter = _logWidget.filterEdit->text();
    reread();
}


int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QFileSystemWatcher monitor;
  monitor.addPath(argv[1]);
  
  LoganLogger* pLogWidget = new LoganLogger(&monitor);
  pLogWidget->init();

  QMainWindow* pMainWindow;
  pMainWindow = new LoganMainWindow(pLogWidget);
  pMainWindow->show();
    
  return app.exec();
}
