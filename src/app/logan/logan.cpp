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

#include <qt4/QtCore/qfile.h>

#include <iostream>
#include <qt4/QtGui/qboxlayout.h>
#include "logan.h"


LoganLogger::LoganLogger(QFileSystemWatcher* pMonitor, QWidget* parent) :
QWidget(parent),
_pMonitor(pMonitor),
_logLevel(NO_LEVEL),
_channel(NO_CHANNEL),
_filter("")
//_channelFilter("")
{
    _logWidget.setupUi(this);
    _logWidget.logViewer->setReadOnly(true);
    _logWidget.logViewer->setAcceptRichText(false);
    _logWidget.logViewer->setLineWrapMode(QTextEdit::NoWrap);
}


void
LoganLogger::init()
{
    std::clog << "init log window ..." << std::endl;

    _file.setFileName(_pMonitor->files()[0]);
    _file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString lines;
    while (!_file.atEnd()) {
        lines += _file.readLine();
    }
    setLines(lines);

    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    connect(_logWidget.filterEdit, SIGNAL(returnPressed()), this, SLOT(filterChanged()));

    std::clog << "init log window finished." << std::endl;
}


void
LoganLogger::getIsLogEntry(const QString& line)
{
    _isLogEntry = line.length() > 30 && line[2] == ':' && line[5] == ':' && line[8] == '.';
}


void
LoganLogger::getLogLevel(const QString& line)
{
    if (_isLogEntry) {
        QChar level = debugLevel(line);
        if (level == 'E') {
            _logLevel = ERROR;
        }
        else if (level == 'D') {
            _logLevel = DEBUG;
        }
        else if (level == 'I') {
            _logLevel = INFO;
        }
        else {
            _logLevel = TRACE;
        }
    }
    else {
        _logLevel = NO_LEVEL;
    }
}


void
LoganLogger::getChannel(const QString& line)
{
    if (_isLogEntry) {
        int channelBegin = _debugLevelPosition + 2;
        int channelEnd = line.indexOf(' ', channelBegin);
        QStringRef channel = line.midRef(channelBegin, channelEnd - channelBegin);
        if (channel == "UPNP") {
            _channel = UPNP_GENERAL;
        }
        else if (channel == "UPNP.SSDP") {
            _channel = UPNP_SSDP;
        }
        else {
            _channel = NO_CHANNEL;
        }
    }
    else {
        // do nothing, leave same channel as line before.
    }
}


void
LoganLogger::getDebugLevelPosition(const QString& line)
{
    if (_isLogEntry) {
        _debugLevelPosition = line.indexOf(']') + 2;
    }
}


QChar
LoganLogger::debugLevel(const QString& line)
{
    return line[_debugLevelPosition];
}


void
LoganLogger::colorLine(const QString& line)
{
    QColor color = Qt::gray;

    switch (_logLevel) {
        case ERROR:
            color = Qt::red;
            break;
        case DEBUG:
            color = Qt::black;
            break;
        case INFO:
            color = Qt::green;
            break;
        case NO_LEVEL:
            color = Qt::blue;
            break;
    }
    _logWidget.logViewer->setTextColor(color);
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
    clear();
    _file.reset();
    while (!_file.atEnd()) {
        appendLine(_file.readLine());
    }
    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
}


void
LoganLogger::setLines(const QString& lines)
{
    _logWidget.logViewer->setText(lines);
}


void
LoganLogger::appendLine(const QString& line)
{
    // analyze current line
    getIsLogEntry(line);
    getDebugLevelPosition(line);
    getLogLevel(line);
    getChannel(line);

    // display line
    colorLine(line);
    if (_channel == UPNP_SSDP) {
        _logWidget.logViewer->insertPlainText(line);
        _logWidget.logViewer->moveCursor(QTextCursor::End);
    }
//    if (_filter.length() == 0 || line.indexOf(_filter) != -1) {
//        _logWidget.logViewer->insertPlainText(line);
//        _logWidget.logViewer->moveCursor(QTextCursor::End);
//    }
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
    _pLayout->setSpacing(0);
    _pLayout->setMargin(0);
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

    LoganLogger* pLog3 = new LoganLogger(&monitor);
    pLog3->init();
    mainWindow.addLogWindow(pLog3);
    pLog3->show();

    LoganLogger* pLog4 = new LoganLogger(&monitor);
    pLog4->init();
    mainWindow.addLogWindow(pLog4);
    pLog4->show();

    mainWindow.tileSubWindows();

    return app.exec();
}
