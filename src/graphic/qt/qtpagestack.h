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
#ifndef QTPAGESTACK_H
#define QTPAGESTACK_H

#include "pagestackwidget.h"
#include "controler.h"
#include "thread.h"

#include <qapplication.h>
#include <qwidgetstack.h>


/**
@author Jörg Bakker
*/
class QtPageStack : public PageStackWidget, private Thread
{
public:
    QtPageStack();
    ~QtPageStack();

    virtual void addPage(Page *page) { m_pageStack->addWidget((QWidget*) page->frame()); }
    virtual void raisePage(Page *page) { m_pageStack->raiseWidget((QWidget*) page->frame()); }
    virtual void* frame() { return (void*) m_pageStack; }

    void lockGui() { m_qtApp->lock(); }
    void unlockGui() { m_qtApp->unlock(); }

private:
    virtual void run();
    virtual void beforeExit();

    QWidgetStack *m_pageStack;
    QApplication *m_qtApp;
};


class QtEventFilter : public QObject
{
private:
    virtual bool eventFilter(QObject *o, QEvent *e);
};

#endif
