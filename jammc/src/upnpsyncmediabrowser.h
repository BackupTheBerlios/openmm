/***************************************************************************
 *   Copyright (C) 2009 by Jörg Bakker   				   *
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
#ifndef UPNPSYNCMEDIABROWSER_H
#define UPNPSYNCMEDIABROWSER_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

#include <QObject>
#include <platinum/PltSyncMediaBrowser.h>

class UpnpObject;

class UpnpDevice
{
public:
    UpnpDevice();
    string getFriendlyName();

// protected:
    PLT_DeviceDataReference m_pltDevice;
};


class UpnpServer : public UpnpDevice
{
public:
    UpnpServer();
    // TODO: return reference or pointer to vector to avoid calling vector's copy constructor
    vector<UpnpObject*> browseChildren(UpnpObject* object, int index = 0, int count = 0, string filter = "*", string sort = "");
    
// private:
    PLT_MediaBrowser* m_pltBrowser;
};


class UpnpObject
{
public:
    UpnpObject();

    bool isContainer();
    string getTitle();
    void fetchChildren();

    UpnpServer*         m_server;
    string              m_objectId;
    UpnpObject*         m_parent;
    vector<UpnpObject*> m_children;
    //    int                 m_lastSliceFetched;
    bool                m_fetchedChildren;
    
// private:
    PLT_MediaObject*    m_pltObject;
};


#endif //UPNPSYNCMEDIABROWSER_H