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
#ifndef LISTPROXY_H
#define LISTPROXY_H

#include "list.h"
#include "listmanager.h"

#include <string>
using namespace std;


/**
	@author Jörg Bakker <joerg@hakker.de>
*/
class ListProxy : public List
{
public:
    ListProxy(ListManager *listManager, Title::TitleT type);
    ~ListProxy();

    virtual void update();

// public slots:
    virtual void addTitle(Title *entry) { m_listManager->addProxyTitle(entry); }
    virtual void delTitle(Title *entry) { m_listManager->delProxyTitle(entry); }

protected:
    virtual void fillList();

private:
    ListManager *m_listManager;
    Title::TitleT m_type;
};

#endif
