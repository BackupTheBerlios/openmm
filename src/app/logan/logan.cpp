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
#include <QtXml>
#include <qt4/QtCore/qstring.h>
#include "logan.h"


const QString LoganLogger::LOG_LEVEL_NONE = "";
const QString LoganLogger::LOG_LEVEL_TRACE = "T";
const QString LoganLogger::LOG_LEVEL_DEBUG = "D";
const QString LoganLogger::LOG_LEVEL_INFO = "I";
const QString LoganLogger::LOG_LEVEL_NOTICE = "N";
const QString LoganLogger::LOG_LEVEL_WARN = "W";
const QString LoganLogger::LOG_LEVEL_ERROR = "E";
const QString LoganLogger::LOG_LEVEL_CRITICAL = "C";
const QString LoganLogger::LOG_LEVEL_FATAL = "F";

const QString LoganLogger::CHAN_NONE = "";
const QString LoganLogger::CHAN_ALL = "*";
const QString LoganLogger::CHAN_UPNP_GENERAL = "UPNP.GENERAL";
const QString LoganLogger::CHAN_UPNP_SSDP = "UPNP.SSDP";
const QString LoganLogger::CHAN_UPNP_HTTP = "UPNP.HTTP";
const QString LoganLogger::CHAN_UPNP_DESC = "UPNP.DESC";
const QString LoganLogger::CHAN_UPNP_CONTROL = "UPNP.CONTROL";
const QString LoganLogger::CHAN_UPNP_EVENT = "UPNP.EVENT";
const QString LoganLogger::CHAN_UPNP_AV = "UPNP.AV";


LoganLogger::LoganLogger(QFileSystemWatcher* pMonitor, QWidget* parent) :
QWidget(parent),
_pMonitor(pMonitor),
_logLevel(LEVEL_NONE),
_channel(CHAN_NONE),
_channelMatches(false),
_filter(""),
_message(""),
_xmlMessage(""),
_isXml(false)
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
//    QString lines;
//    while (!_file.atEnd()) {
//        lines += _file.readLine();
//    }
//    setLines(lines);

    _logWidget.channelSelector->addItem(CHAN_NONE);
    _logWidget.channelSelector->addItem(CHAN_ALL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_GENERAL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_SSDP);
    _logWidget.channelSelector->addItem(CHAN_UPNP_HTTP);
    _logWidget.channelSelector->addItem(CHAN_UPNP_DESC);
    _logWidget.channelSelector->addItem(CHAN_UPNP_CONTROL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_EVENT);
    _logWidget.channelSelector->addItem(CHAN_UPNP_AV);

    connect(_pMonitor, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    connect(_logWidget.filterEdit, SIGNAL(returnPressed()), this, SLOT(filterChanged()));
    connect(_logWidget.channelSelector, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(channelChanged(const QString&)));

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
        _logLevel = LEVEL_NONE;
    }
}


void
LoganLogger::getChannel(const QString& line)
{
    if (_isLogEntry) {
        int channelBegin = _debugLevelPosition + 2;
        int channelEnd = line.indexOf(' ', channelBegin);
        _channel = line.mid(channelBegin, channelEnd - channelBegin);
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


QString
LoganLogger::prettyPrint(const QString& xml)
{
    QDomDocument doc;
    doc.setContent(xml);
    return doc.toString(2);
}


void
LoganLogger::colorMessageLine()
{
    _logWidget.logViewer->setTextColor(Qt::blue);
}


void
LoganLogger::colorLogLine(const QString& line)
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
        case LEVEL_NONE:
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
    QString selectedChannel = _logWidget.channelSelector->currentText();

    getIsLogEntry(line);
    if (_isLogEntry) {
        getDebugLevelPosition(line);
        getLogLevel(line);
        getChannel(line);
        _channelMatches = (_channel == selectedChannel || selectedChannel == CHAN_ALL);
    }
    else if (_channelMatches) {
        if (line.startsWith("<?xml")) {
            _isXml = true;
        }
        if (_isXml) {
            _xmlMessage += line;
        }
        else {
            _message += line;
        }
    }
    if (_isLogEntry && _channelMatches) {
        // NOTE: this relies on the xml message coming always after the non-xml message.
        if (_message.length() || _xmlMessage.length()) {
            colorMessageLine();
            if (_message.length()) {
                _logWidget.logViewer->insertPlainText(_message);
            }
            if (_xmlMessage.length()) {
                _logWidget.logViewer->insertPlainText(prettyPrint(_xmlMessage));
            }
            if (_isXml) {
                _isXml = false;
            }
            _logWidget.logViewer->moveCursor(QTextCursor::End);
            _message = "";
            _xmlMessage = "";
        }
        colorLogLine(line);
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


void
LoganLogger::channelChanged(const QString& chan)
{
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

    mainWindow.tileSubWindows();

    return app.exec();
}
