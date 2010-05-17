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
#include <Poco/ClassLibrary.h>

#include "QtSink.h"


QtSinkPlugin::QtSinkPlugin() :
_app(0, 0),
_widget()
{
}


QtSinkPlugin::~QtSinkPlugin()
{
}


void
QtSinkPlugin::open()
{
    connect(this, SIGNAL(doUpdate()), &_widget, SLOT(update()));
    
    _widget.setWindowTitle("OMM Renderer");
    _widget.resize(720, 576);
    showWindow();
}


void
QtSinkPlugin::close()
{
    std::clog << "QtSinkPlugin::close()" << std::endl;
    
}


void
QtSinkPlugin::writeFrame(Omm::Av::Frame* pFrame)
{
    int width = pFrame->getStream()->width();
    int height = pFrame->getStream()->height();
    
    std::clog << "allocate QImage width: " << width << " height: " << height << std::endl;
    Omm::Av::Frame* pConvertedFrame = pFrame->convert(PIX_FMT_RGB24);
    _widget._image = QImage((uchar*)pConvertedFrame->planeData(0), width, height, QImage::Format_RGB888);
    _widget.repaint();
}


void
QtSinkPlugin::showWindow()
{
    std::clog << "showWindow()" << std::endl;
    
    _widget.show();
}


int
QtSinkPlugin::eventLoop()
{
    std::clog << "event loop ..." << std::endl;
    
    return _app.exec();
}


void
VideoWidget::paintEvent(QPaintEvent *event)
{
    std::clog << "paintEvent()" << std::endl;
    
    if (_image.isNull()) return;
    
    std::clog << "set up QPainter" << std::endl;
    // TODO: is this QPainter needed? VideoWidget is a QPainter ...
    QPainter painter(this);
    std::clog << "draw image to QPainter" << std::endl;
    painter.drawImage(QPoint(0,0), _image /*,  Qt::ColorOnly | Qt::OrderedDither*/);
}


POCO_BEGIN_MANIFEST(Omm::Av::Sink)
POCO_EXPORT_CLASS(QtSinkPlugin)
POCO_END_MANIFEST
