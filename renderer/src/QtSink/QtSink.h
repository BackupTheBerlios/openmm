/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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
#ifndef QtSink_INCLUDED
#define QtSink_INCLUDED

#include <QtGui>

#include <Omm/AvStream.h>

class VideoWidget : public QWidget
{
    Q_OBJECT
    friend class QtSinkPlugin;
    
public:
        

private:
    void paintEvent(QPaintEvent *event);
    
    QImage                  _image;
};


class QtSinkPlugin : public QObject, public Omm::Av::Sink
{
    Q_OBJECT
        
public:
    QtSinkPlugin();
    virtual ~QtSinkPlugin();
    virtual void open();
    virtual void close();
    virtual void writeFrame(Omm::Av::Frame *pFrame);
    virtual int eventLoop();
    
signals:
    void doUpdate();
    
private:
    void showWindow();
    
    QApplication            _app;
    VideoWidget             _widget;
};

#endif
