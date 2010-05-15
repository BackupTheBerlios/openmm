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


SinkPlugin::SinkPlugin() :
_app(0, 0),
_widget()
{
}


SinkPlugin::~SinkPlugin()
{
}


void
SinkPlugin::open()
{
    connect(this, SIGNAL(doUpdate()), &_widget, SLOT(update()));
    
    _widget.setWindowTitle("OMM Renderer");
    _widget.resize(720, 576);
    showWindow();
}


void
SinkPlugin::close()
{
    std::clog << "SinkPlugin::close()" << std::endl;
    
}


void
SinkPlugin::writeFrame(Omm::Av::Frame* pFrame)
{
    int width = pFrame->getStream()->width();
    int height = pFrame->getStream()->height();
        
    std::clog << "Qt video sink::writeFrame() width: " << width << " height: " << height << std::endl;
    
//     // Scale up to aspect ratio
// //     float wratio = pFrame->width()/(float)pFrame->aspect_ratio.width;
// //     float hratio = pFrame->height()/(float)pFrame->aspect_ratio.height;
//     float wratio = width/(float)4;
//     float hratio = height/(float)3;
//     float ratio = qMax(wratio, hratio);
// //     QSize frameSize(pFrame->aspect_ratio.width * ratio,
// //                      pFrame->aspect_ratio.height * ratio);
//     QSize frameSize(4 * ratio,
//                     3 * ratio);
//     // resize to widget
//     // TODO: scale with sws_scale() in method convertRGB() instead
//     frameSize.scale(_widget.size(), Qt::KeepAspectRatio);
    
    std::clog << "allocate QImage width: " << width << " height: " << height << std::endl;
//     QImage decodedImage((uchar*)pFrame->data(), width, height, QImage::Format_RGB32);
//     QImage decodedImage((uchar*)pFrame->data(), width, 150, QImage::Format_RGB32);
//     std::clog << "scale QImage to width: " << frameSize.width() << " height: " << frameSize.height() << std::endl;
//     _image = decodedImage.scaled(frameSize, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    _widget._image = QImage((uchar*)pFrame->data(), width, height, QImage::Format_RGB32);
//     _widget.update();
//     emit doUpdate();
    _widget.repaint();
}


void
SinkPlugin::showWindow()
{
    std::clog << "showWindow()" << std::endl;
    
    _widget.show();
}


int
SinkPlugin::eventLoop()
{
    std::clog << "event loop ..." << std::endl;
    
    return _app.exec();
}


// void SinkPlugin::pause()
// {
// }
// 

// void SinkPlugin::resume()
// {
// }


// static int resume(snd_pcm_t *pcm)
// {
// }


// int SinkPlugin::latency()
// {
// }


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
    
    
//     d->mutex.lock();
//     QPainter p(this);
//     p.drawImage(QPoint(0,0), d->image /*,  Qt::ColorOnly | Qt::OrderedDither*/);
//     
//     QFont font("Arial", 16);
//     p.setPen(Qt::white);
//     p.setFont(font);
//     p.drawText(QPoint(20,20), QString("Delay: ")+QString::number(latency())+"ms");
//     
//     // ### does drawImage block 'till the image is on the X11 server?
//     // if not we need to add X11 latency for network transparency
//     if (d->frameTime != 0) {
//         int delay = getTime() - d->frameTime;
//         d->latency.addLatency(delay);
//         d->frameTime = 0;
//     }
//     d->mutex.unlock();
}


POCO_BEGIN_MANIFEST(Omm::Av::Sink)
POCO_EXPORT_CLASS(SinkPlugin)
POCO_END_MANIFEST
