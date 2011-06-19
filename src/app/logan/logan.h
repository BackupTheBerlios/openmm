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


//class LoganLogger : public QMdiSubWindow
class LoganLogger : public QWidget
{
    Q_OBJECT
  
public:
    LoganLogger(QFileSystemWatcher* pMonitor, QWidget* parent = 0);
    void init();
  
private slots:
    void fileChanged(const QString& path);
    void filterChanged();
  
private:
    bool isLogEntry(const QString& line);
    QChar debugLevel(const QString& line);
//    void filterDebugLevel(const QChar& level);
//    void filterChannel(const QChar& level);
    void colorLine(const QString& line);
    void clear();
    void reread();
    void setLines(const QString& lines);
    void appendLine(const QString& line);

    QVBoxLayout*            _pLayout;
    QWidget*                _pMainWidget;
    QFile                   _file;
//    std::ifstream           _file;
    Ui::LogWidget           _logWidget;
    QFileSystemWatcher*     _pMonitor;
    QString                 _filter;
//    QString                 _channelFilter;
//    QChar                   _levelFilter;
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
//    QMdiArea*       _pMdiArea;
    MdiArea*       _pMdiArea;
};


#endif