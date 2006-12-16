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
#include "tvprogrambrowser.h"
#include "titlepair.h"
#include "debug.h"

//#include <qdatetime.h>


TvProgramBrowser::TvProgramBrowser(ListComposer *list)
 : ListBrowser("Program Guide", "Id;Left.Name;Start;End;Right.Name", list)
{
    m_filter = new TvProgramFilter();
    ((ListComposer*)m_list)->setFilter(m_filter);
    // TODO: handle events independent from Toolkit.
    //m_listView->installEventFilter(this);
}


TvProgramBrowser::~TvProgramBrowser()
{
}


void
TvProgramBrowser::enterPage()
{
    TRACE("TvProgramBrowser::enterPage()");
    m_listBrowserWidget->enterPage();
    if (m_list->count()) {
        selectEntry(0);
    }
}


bool
TvProgramBrowser::eventHandler(QEvent *e)
{
    //TRACE("TvProgramBrowser::eventFilter()");
    if ( e->type() == QEvent::KeyPress ) {
        QKeyEvent *k = (QKeyEvent *)e;
        time_t at;
        switch (k->key()) {
            case Qt::Key_Left:
                TRACE("TvProgramBrowser::eventFilter(), Key_Left, Title: %s",
                        ((TitlePair*)getCurrent())->getRight()->getText("Name").c_str());
                at = ((TvProgram*)((TitlePair*)getCurrent())->getRight())->getStart() - 1;
                if (at < time(0)) {
                    return true;
                }
                m_filter->setTime(at);
                clear();
                ((ListComposer*)m_list)->pushFiltered();
                return true;
            case Qt::Key_Right:
                TRACE("TvProgramBrowser::eventFilter(), Key_Right, Title: %s",
                        ((TitlePair*)getCurrent())->getRight()->getText("Name").c_str());
                at = ((TvProgram*)((TitlePair*)getCurrent())->getRight())->getEnd();
                m_filter->setTime(at);
                clear();
                ((ListComposer*)m_list)->pushFiltered();
                return true;
            default:
                return false;
        }
    } else {
        // standard event processing
        return false;
    }
}
