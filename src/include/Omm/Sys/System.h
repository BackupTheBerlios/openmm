/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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
#ifndef SysDevice_INCLUDED
#define SysDevice_INCLUDED

#include <string>
#include <vector>

namespace Omm {
namespace Sys {

class SysImpl;
class Device;


class System
{
public:
    // TODO: complete system device types
    static const std::string DeviceTypeOther;
    static const std::string DeviceTypeDvb;
    static const std::string DeviceTypeDisk;

    static System* instance();

    void getDevicesForType(std::vector<Device*>& devices, const std::string& deviceType);

private:
    System();

    static System*     _pInstance;
    SysImpl*           _pImpl;
};


class Device
{
public:
    Device(const std::string& id, const std::string& type = System::DeviceTypeOther, const std::string& node = "");

    std::string getId();
    std::string getType();
    std::string getNode();

private:
    std::string     _id;
    std::string     _type;
    std::string     _node;
};

}  // namespace Sys
}  // namespace Omm

#endif
