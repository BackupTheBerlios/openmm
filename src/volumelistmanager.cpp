/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
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
#include "volumelistmanager.h"
#include "directory.h"
#include "title.h"
#include "mrl.h"
#include "debug.h"

#include <vector>


VolumeListManager::VolumeListManager(string path)
 : ListManager(),
    m_path(path)
{
}


VolumeListManager::~VolumeListManager()
{
}


void
VolumeListManager::fill(List *list, Title::TitleT type)
{
    TRACE("VolumeListManager::fill() reading path: %s", m_path.c_str());
    Directory d(m_path);
    vector<string> entries = d.entryList("*");
    for (vector<string>::iterator i = entries.begin(); i != entries.end(); ++i) {
        TRACE("VolumeListManager::fill() adding: %s", (*i).c_str());
        Title * t = new Title(*i, Title::FileT);
        t->setMrl(new Mrl("file://", m_path + "/" + (*i)));
        list->addTitleEntry(t);
    }
}
