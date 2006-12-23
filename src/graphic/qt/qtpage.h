/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef QTPAGE_H
#define QTPAGE_H

#include "pagewidget.h"

#include <qwidget.h>

/**
A full page on the screen, which can be a menu, mediaplayer, list, ...
Basically, it can be shown, or not.

	@author Jörg Bakker <joerg@hakker.de>
*/
class QtPage : public QObject, public PageWidget
{
    Q_OBJECT

public:
    QtPage();
    ~QtPage();

    virtual int globalPositionX() { return m_frame->mapToGlobal(QPoint(0,0)).x(); }
    virtual int globalPositionY() { return m_frame->mapToGlobal(QPoint(0,0)).y(); }
    virtual int width() { return m_frame->width(); }
    virtual int height() { return m_frame->height(); }
    virtual unsigned long windowId() { return m_frame->winId(); }
    virtual void* frame() { return (void*) m_frame; }
    virtual void enterPage();

protected:
    QWidget *m_frame;
};

#endif
