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

#ifndef logan_INCLUDED
#define logan_INCLUDED

#include <fstream>
#include <QtGui>
#include "ui_logwidget.h"


class LoganLogger : public QWidget
{
    Q_OBJECT
  
public:
    LoganLogger(QFileSystemWatcher* pMonitor, QWidget* parent = 0);
    void init();
  
private slots:
    void fileChanged(const QString& path);
    void filterChanged();
    void channelChanged(const QString& chan);
  
private:
    enum LogLevel {LEVEL_NONE, TRACE, DEBUG, INFO, NOTICE, WARN, ERROR, CRITICAL, FATAL};

    const static QString LOG_LEVEL_NONE;
    const static QString LOG_LEVEL_TRACE;
    const static QString LOG_LEVEL_DEBUG;
    const static QString LOG_LEVEL_INFO;
    const static QString LOG_LEVEL_NOTICE;
    const static QString LOG_LEVEL_WARN;
    const static QString LOG_LEVEL_ERROR;
    const static QString LOG_LEVEL_CRITICAL;
    const static QString LOG_LEVEL_FATAL;

    const static QString CHAN_NONE;
    const static QString CHAN_ALL;
    const static QString CHAN_UPNP_GENERAL;
    const static QString CHAN_UPNP_SSDP;
    const static QString CHAN_UPNP_HTTP;
    const static QString CHAN_UPNP_DESC;
    const static QString CHAN_UPNP_CONTROL;
    const static QString CHAN_UPNP_EVENT;
    const static QString CHAN_UPNP_AV;

    void getDebugLevelPosition(const QString& line);
    void getIsLogEntry(const QString& line);
    void getLogLevel(const QString& line);
    void getChannel(const QString& line);
    QChar debugLevel(const QString& line);
//    void filterDebugLevel(const QChar& level);
    QString prettyPrint(const QString& xml);
    void colorMessageLine();
    void colorLogLine(const QString& line);
    void clear();
    void reread();
    void setLines(const QString& lines);
    void appendLine(const QString& line);

    QVBoxLayout*            _pLayout;
    QWidget*                _pMainWidget;
    QFile                   _file;
    Ui::LogWidget           _logWidget;
    QFileSystemWatcher*     _pMonitor;
    QString                 _filter;
    bool                    _isLogEntry;
    int                     _debugLevelPosition;
    LogLevel                _logLevel;
    QString                 _channel;
    QString                 _message;
    QString                 _xmlMessage;
    bool                    _channelMatches;
    bool                    _isXml;
};


class MdiArea : public QWidget
{
    Q_OBJECT
public:
    MdiArea(QWidget* parent = 0);
    ~MdiArea();

    void addSubWindow(QWidget* pSubWindow);

private:
    QVBoxLayout*    _pLayout;
};


class LoganMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoganMainWindow();
    ~LoganMainWindow();

    void addLogWindow(LoganLogger* pLogWindow);
    void tileSubWindows();

private:
    MdiArea*       _pMdiArea;
};


#endif