/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
|  Jörg Bakker (jb'at'open-multimedia.org)                                  |
|                                                                           |
|  This file is part of OMM.                                                |
|                                                                           |
|  OMM is free software: you can redistribute it and/or modify              |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  OMM is distributed in the hope that it will be useful,                   |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/
#ifndef UpnpAvCtlServer_INCLUDED
#define UpnpAvCtlServer_INCLUDED

#include <Poco/Timer.h>
#include <Poco/DOM/Node.h>
#include <Poco/Notification.h>

#include "Upnp.h"
#include "UpnpInternal.h"
#include "UpnpAvCtlObject.h"


namespace Omm {
namespace Av {

class CtlMediaServerCode;
class CtlMediaObject;


class CtlMediaServer : public Device
{
public:
    virtual void addCtlDeviceCode();

    CtlMediaObject* getRootObject() const;

protected:
    void browseRootObject();
    void selectMediaObject(CtlMediaObject* pObject);

private:    
    CtlMediaServerCode*   _pCtlMediaServerCode;
    CtlMediaObject*       _pRoot;
};


class MediaServerGroupDelegate : public DeviceGroupDelegate
{
public:
    virtual std::string getDeviceType();
    virtual std::string shortName();
};


} // namespace Av
} // namespace Omm


#endif
