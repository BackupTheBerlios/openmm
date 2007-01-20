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
#ifndef VDR_H
#define VDR_H

#include "list.h"
#include "listmanager.h"


/**
Backend to manage channels, recs, and timers using the SVDRP protocol of the vdr server.

	@author Jörg Bakker <joerg@hakker.de>
*/

// TODO: make this a singleton?
// TODO: register a List for each ListManager, or one backend for all Lists?

class Vdr : public ListManager
{
public:
    Vdr();
    ~Vdr();

    virtual bool pushUpdates() { return false; }  // there's no way to push updates with SVDRP.
    virtual void fill(List *list, Title::TitleT type);

    virtual void addProxyTitle(Title *title);
    virtual void delProxyTitle(Title *title);
};

#endif
