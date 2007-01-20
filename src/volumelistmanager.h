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
#ifndef VOLUMELISTMANAGER_H
#define VOLUMELISTMANAGER_H

#include "listmanager.h"

#include <string>
using namespace std;

/**
Reads the files on a volume and makes Titles for a List out of them.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/
class VolumeListManager : public ListManager
{
public:
    VolumeListManager(string path);
    ~VolumeListManager();

    virtual bool pushUpdates() { return false; }  // Titles can only be pushed if the filesystem changes ...
    virtual void fill(List *list, Title::TitleT type);

    virtual void addProxyTitle(Title *title) {}  // we don't change the filesystem, just read from it.
    virtual void delProxyTitle(Title *title) {}  // we don't change the filesystem, just read from it.

private:
    string m_path;
};

#endif
