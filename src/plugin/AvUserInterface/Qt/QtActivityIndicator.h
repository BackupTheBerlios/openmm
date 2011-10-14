/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#ifndef QtActivityIndicator_INCLUDED
#define	QtActivityIndicator_INCLUDED

#include <QWidget>
#include <QTimer>
#include <QMutex>
// Bug in old cmake find file for qt: though package Svg is selected, include path is missing
#include <qt4/QtSvg/QSvgRenderer>
//#include <QSvgRenderer>


class QtActivityIndicator : public QWidget
{
    Q_OBJECT

public:
    QtActivityIndicator(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~QtActivityIndicator();

public slots:
    void startActivity();
    void stopActivity();

private slots:
    void stopIndicator();

private:
    void paintEvent(QPaintEvent *event);
    void setActivityInProgress(bool set);
    bool activityInProgress();
    void setIndicatorOn(bool set);
    bool indicatorOn();

    const int           _indicateDuration;
    QSvgRenderer*       _symbolRenderer;
    bool                _activityInProgress;
    QMutex              _activityInProgressLock;
    bool                _indicatorOn;
    QMutex              _indicatorOnLock;
    QTimer              _offTimer;
};

#endif

