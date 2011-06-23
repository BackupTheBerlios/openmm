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

#include <qt4/QtCore/qfilesystemwatcher.h>


#include <iostream>
#include <QtXml>
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
const QString LoganLogger::CHAN_UTIL = "UTIL";
const QString LoganLogger::CHAN_NET = "NET";
const QString LoganLogger::CHAN_SYS = "SYS";


LoganFileWatcher::LoganFileWatcher() :
_fileSize(0)
{
}


void
LoganFileWatcher::init(const QString& path)
{
//    QString home = QProcessEnvironment::systemEnvironment().value("HOME", ".");

    _fileInfo.setFile(path);
    _fileInfo.setCaching(false);
    _fileSystemWatcher.addPath(_fileInfo.path());
    connect(&_fileSystemWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(directoryChanged(const QString&)));
    openFile();
}


void
LoganFileWatcher::openFile()
{
    std::clog << "open file " << _fileInfo.absoluteFilePath().toStdString() << std::endl;

    if (_file.isOpen()) {
        std::clog << "file " << _fileInfo.absoluteFilePath().toStdString() << " already open, doing nothing." << std::endl;
        return;
    }

    if (_fileInfo.isFile() && _fileInfo.isReadable()) {
        _file.setFileName(_fileInfo.absoluteFilePath());
        _file.open(QIODevice::ReadOnly | QIODevice::Text);
        _fileSystemWatcher.addPath(_fileInfo.absoluteFilePath());
        connect(&_fileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    }
    else {
        std::clog << "file " << _fileInfo.absoluteFilePath().toStdString() << " not present, waiting ..." << std::endl;
    }
}


void
LoganFileWatcher::closeFile()
{
    std::clog << "close file " << _fileInfo.absoluteFilePath().toStdString() << std::endl;
    
    disconnect(&_fileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    _fileSystemWatcher.removePath(_fileInfo.absoluteFilePath());
    _file.close();
    emit fileClosed();
}


void
LoganFileWatcher::fileChanged(const QString& path)
{
    if (!(_fileInfo.isFile() && _fileInfo.isReadable())) {
        closeFile();
    }
    else {
        if (_fileInfo.size() < _fileSize) {
            _file.reset();
            emit fileClosed();
        }
        _fileSize = _fileInfo.size();
        while(!_file.atEnd()) {
            emit newLine(_file.readLine());
        }
    }
}


void
LoganFileWatcher::directoryChanged(const QString& path)
{
    std::clog << "directory " << path.toStdString() << " changed." << std::endl;

    openFile();
}


QString
LoganFileWatcher::fileName()
{
    return _fileInfo.absoluteFilePath();
}


LoganLogger::LoganLogger(LoganFileWatcher* pMonitor, QWidget* parent) :
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


LoganLogger::~LoganLogger()
{

}


void
LoganLogger::init()
{
    _logWidget.channelSelector->addItem(CHAN_NONE);
    _logWidget.channelSelector->addItem(CHAN_ALL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_GENERAL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_SSDP);
    _logWidget.channelSelector->addItem(CHAN_UPNP_HTTP);
    _logWidget.channelSelector->addItem(CHAN_UPNP_DESC);
    _logWidget.channelSelector->addItem(CHAN_UPNP_CONTROL);
    _logWidget.channelSelector->addItem(CHAN_UPNP_EVENT);
    _logWidget.channelSelector->addItem(CHAN_UPNP_AV);
    _logWidget.channelSelector->addItem(CHAN_UTIL);
    _logWidget.channelSelector->addItem(CHAN_NET);
    _logWidget.channelSelector->addItem(CHAN_SYS);

    connect(_pMonitor, SIGNAL(newLine(const QString&)), this, SLOT(newLine(const QString&)));
    connect(_pMonitor, SIGNAL(fileClosed()), this, SLOT(clear()));

    connect(_logWidget.channelSelector, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(channelChanged(const QString&)));
    connect(_logWidget.searchEdit, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(_logWidget.searchBackButton, SIGNAL(pressed()), this, SLOT(searchBackwards()));
    connect(_logWidget.searchForwardButton, SIGNAL(pressed()), this, SLOT(searchForwards()));

    _logWidget.searchBackButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    _logWidget.searchForwardButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
}


void
LoganLogger::readConfig(int windowNumber)
{
    LoganSettings::instance()->beginGroup("WindowNumber" + QString(windowNumber));
    QString selectedChannel = LoganSettings::instance()->value("Channel", "").toString();
    int index = _logWidget.channelSelector->findText(selectedChannel);
    _logWidget.channelSelector->setCurrentIndex(index);
    LoganSettings::instance()->endGroup();
}


void
LoganLogger::saveConfig(int windowNumber)
{
    LoganSettings::instance()->beginGroup("WindowNumber" + QString(windowNumber));
    LoganSettings::instance()->setValue("Channel", _logWidget.channelSelector->currentText());
    LoganSettings::instance()->endGroup();
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
    disconnect(_pMonitor, SIGNAL(newLine(const QString&)), this, SLOT(newLine(const QString&)));
    clear();
    QFile file;
    file.setFileName(_pMonitor->fileName());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    while (!file.atEnd()) {
        appendLine(file.readLine());
    }
    connect(_pMonitor, SIGNAL(newLine(const QString&)), this, SLOT(newLine(const QString&)));
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
        QScrollBar* scrollBar = _logWidget.logViewer->verticalScrollBar();
        bool scrolledToBottom = (scrollBar->value() >= scrollBar->maximum() - 50) || !scrollBar->isVisible();
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
            _message = "";
            _xmlMessage = "";
        }
        colorLogLine(line);
        _logWidget.logViewer->insertPlainText(line);
        if (scrolledToBottom) {
            _logWidget.logViewer->moveCursor(QTextCursor::End);
        }
    }

//    if (_filter.length() == 0 || line.indexOf(_filter) != -1) {
//        _logWidget.logViewer->insertPlainText(line);
//        _logWidget.logViewer->moveCursor(QTextCursor::End);
//    }
}


void
LoganLogger::newLine(const QString& line)
{
    appendLine(line);
}


void
LoganLogger::search(bool backward)
{
    std::clog << "find: " << _logWidget.searchEdit->text().toStdString() << std::endl;

    QTextDocument::FindFlag findFlag;
    if (backward) {
        findFlag = QTextDocument::FindBackward;
    }

    if (_logWidget.logViewer->find(_logWidget.searchEdit->text(), findFlag)) {
        std::clog << "found." << std::endl;
    }
    else {
        std::clog << "not found." << std::endl;
    }
}


void
LoganLogger::searchBackwards()
{
    search(true);
}


void
LoganLogger::searchForwards()
{
    search(false);
}


void
LoganLogger::channelChanged(const QString& chan)
{
    reread();
}


LoganMdiArea::LoganMdiArea(QWidget* parent) :
QWidget(parent)
{
    _pLayout = new QVBoxLayout(this);
    _pLayout->setSpacing(0);
    _pLayout->setMargin(0);
}


LoganMdiArea::~LoganMdiArea()
{
    delete _pLayout;
}


void
LoganMdiArea::addSubWindow(QWidget* pSubWindow)
{
    pSubWindow->setParent(this);
    _pLayout->addWidget(pSubWindow);
}


LoganMainWindow::LoganMainWindow()
{
    _pMdiArea = new LoganMdiArea;
    setCentralWidget(_pMdiArea);
    QSize windowSize = LoganSettings::instance()->value("mainWindow/size", QSize(800, 900)).toSize();
    resize(windowSize);
}


LoganMainWindow::~LoganMainWindow()
{
    LoganSettings::instance()->setValue("mainWindow/size", size());
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


LoganSettings* LoganSettings::_pInstance = 0;

LoganSettings::LoganSettings() :
QSettings("open-multimedia.org", "logan")
{

}


void
LoganSettings::release()
{
    if (_pInstance) {
        _pInstance->sync();
        delete _pInstance;
        _pInstance = 0;
    }
}


LoganSettings*
LoganSettings::instance()
{
    if (!_pInstance) {
        _pInstance = new LoganSettings;
    }
    return _pInstance;
}


int
main(int argc, char** argv)
{
    QApplication app(argc, argv);

    LoganFileWatcher monitor;
    monitor.init(argv[1]);

    LoganMainWindow* pMainWindow = new LoganMainWindow;
    pMainWindow->show();

    LoganLogger* pLog1 = new LoganLogger(&monitor);
    pLog1->init();
    pLog1->readConfig(1);
    pMainWindow->addLogWindow(pLog1);
    pLog1->show();

    LoganLogger* pLog2 = new LoganLogger(&monitor);
    pLog2->init();
    pLog2->readConfig(2);
    pMainWindow->addLogWindow(pLog2);
    pLog2->show();

    pMainWindow->tileSubWindows();

    bool ret = app.exec();

    pLog2->saveConfig(2);
    pLog1->saveConfig(1);

    delete pLog2;
    delete pLog1;
    delete pMainWindow;

    LoganSettings::release();
    
    return ret;
}
