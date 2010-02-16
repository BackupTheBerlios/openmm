/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
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


class UpnpBrowseResult
{
public:
    UpnpBrowseResult() 
    : m_result(vector<UpnpObject*>()), m_numberReturned(0), m_totalMatches(0), m_updateId(0) {}
    
    vector<UpnpObject*> m_result;
    unsigned int        m_numberReturned;
    unsigned int        m_totalMatches;
    unsigned int        m_updateId;
};


class UpnpServer : public UpnpDevice
{
public:
    UpnpServer();
    UpnpBrowseResult browseChildren(UpnpObject* object, int index = 0, int count = 0, string filter = "*", string sort = "");
    static const int m_sliceSize = 10;
    
// private:
    PLT_MediaBrowser* m_pltBrowser;
};


class UpnpObject
{
public:
    UpnpObject();

    bool isContainer();
    string getTitle();
    // fetchChildren() returns the number of children
    int fetchChildren();

    UpnpServer*         m_server;
    string              m_objectId;
    UpnpObject*         m_parent;
    vector<UpnpObject*> m_children;
    bool                m_fetchedAllChildren;
    unsigned int        m_childCount;
    
// private:
    PLT_MediaObject*    m_pltObject;
};


#endif //UPNPSYNCMEDIABROWSER_H