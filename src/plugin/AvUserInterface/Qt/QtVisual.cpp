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

#include "QtVisual.h"

QtVisual::QtVisual(QWidget* pParent) :
QLabel(pParent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAlignment(Qt::AlignCenter);

    connect(this, SIGNAL(signalShowImage(const std::string&)), this, SLOT(slotShowImage(const std::string&)), Qt::QueuedConnection);
}


QtVisual::~QtVisual()
{
}


void
QtVisual::show()
{
    emit showMenu(false);
}


void
QtVisual::hide()
{
    emit showMenu(true);
}


void*
QtVisual::getWindow()
{
#ifdef __LINUX__
    _x11Window = winId();
    return &_x11Window;
#else
    return 0;
#endif
}


Omm::Sys::Visual::VisualType
QtVisual::getType()
{
    // QtVisual is multi-platform, and type of visual is platform dependent.
#ifdef __LINUX__
    return Omm::Sys::Visual::VTX11;
#elif __DARWIN__
    return Omm::Sys::Visual::VTMacOSX;
#elif __WINDOWS__
    return Omm::Sys::Visual::VTWin;
#else
    return Omm::Sys::Visual::VTNone;
#endif
}


void
QtVisual::renderImage(const std::string& imageData)
{
    emit signalShowImage(imageData);
}


void
QtVisual::blank()
{
    clear();
}


void
QtVisual::slotShowImage(const std::string& imageData)
{
    QPixmap pixmap;
    pixmap.loadFromData((uchar*)imageData.c_str(), (uint)imageData.size());
//    _pWidget->setPixmap(pixmap.scaled(getWidth(), getHeight(), Qt::KeepAspectRatio));
    setPixmap(pixmap.scaled(getWidth(), getHeight(), Qt::KeepAspectRatio));
}
