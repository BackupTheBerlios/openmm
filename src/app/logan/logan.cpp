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


LoganLogger::LoganLogger(QFileSystemWatcher* pMonitor, QWidget* parent) :
//QMdiSubWindow(parent),
QWidget(parent),
_pMonitor(pMonitor),
_filter("")
//_channelFilter("")
{
//    _pLayout = new QVBoxLayout;

//    _pMainWidget = new QWidget;
//    setWidget(_pMainWidget);
//    _logWidget.setupUi(_pMainWidget);
    
    _logWidget.setupUi(this);
    _logWidget.logViewer->setReadOnly(true);
    _logWidget.logViewer->setAcceptRichText(false);
    _logWidget.logViewer->setLineWrapMode(QTextEdit::NoWrap);

//    _pLayout->addWidget(&_logWidget);
}


void
LoganLogger::init()
{
    std::clog << "init log window ..." << std::endl;

    _file.setFileName(_pMonitor->files()[0]);
    _file.open(QIODevice::ReadOnly | QIODevice::Text);
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    connect(_logWidget.filterEdit, SIGNAL(editingFinished()), this, SLOT(filterChanged()));

    std::clog << "init log window finished." << std::endl;
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


MdiArea::MdiArea(QWidget* parent) :
QWidget(parent)
{
    _pLayout = new QVBoxLayout(this);
}


MdiArea::~MdiArea()
{
    delete _pLayout;
}


void
MdiArea::addSubWindow(QWidget* pSubWindow)
{
    pSubWindow->setParent(this);
    _pLayout->addWidget(pSubWindow);
}


LoganMainWindow::LoganMainWindow()
{
//    _pMdiArea = new QMdiArea;
    _pMdiArea = new MdiArea;
    setCentralWidget(_pMdiArea);
    resize(800, 900);
}


LoganMainWindow::~LoganMainWindow()
{
    delete _pMdiArea;
}


void
LoganMainWindow::addLogWindow(LoganLogger* pLogWindow)
{
    _pMdiArea->addSubWindow(pLogWindow);
}


void
LoganMainWindow::tileSubWindows()
{
//    _pMdiArea->tileSubWindows();
//    _pMdiArea->cascadeSubWindows();
}


int
main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QFileSystemWatcher monitor;
  monitor.addPath(argv[1]);

  LoganMainWindow mainWindow;
  mainWindow.show();

  LoganLogger* pLog1 = new LoganLogger(&monitor);
  pLog1->init();
  mainWindow.addLogWindow(pLog1);
  pLog1->show();

  LoganLogger* pLog2 = new LoganLogger(&monitor);
  pLog2->init();
  mainWindow.addLogWindow(pLog2);
  pLog2->show();

//  LoganLogger* pLog3 = new LoganLogger(&monitor);
//  pLog3->init();
//  mainWindow.addLogWindow(pLog3);
//  pLog3->show();

  mainWindow.tileSubWindows();

  return app.exec();
}
