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

#include "UpnpAvServer.h"

using namespace Jamm;
using namespace Jamm::Av;

UpnpAvServer::UpnpAvServer(ContentDirectoryImplementation* pContentDirectoryImpl, ConnectionManagerImplementation* pConnectionManagerImpl, AVTransportImplementation* pAVTransportImpl) :
MediaServer(pContentDirectoryImpl, pConnectionManagerImpl, pAVTransportImpl),
m_pContentDirectoryImpl(pContentDirectoryImpl),
m_pConnectionManagerImpl(pConnectionManagerImpl),
m_pAVTransportImpl(pAVTransportImpl)
{
}


void
UpnpAvServer::setRoot(MediaObject* pRoot)
{
    m_pRoot = pRoot;
    m_pContentDirectoryImpl->m_pRoot = m_pRoot;
    m_pRoot->setObjectId("0");
    m_pRoot->setParentId("-1");
    // in Controller: set title to something like: <device friendly name> on <host name>
    // friendly name of device should contain <host name> or <ip>
}


MediaObject*
UpnpAvServer::getRoot()
{
    return m_pRoot;
}
