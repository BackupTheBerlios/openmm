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

#include "jammgen.h"

StubWriter::StubWriter(DeviceRoot* pDeviceRoot, const std::string& outputPath) :
m_pDeviceRoot(pDeviceRoot),
m_outputPath(outputPath)
{
}


void
StubWriter::write()
{
    deviceRoot(*m_pDeviceRoot);
    for (DeviceRoot::DeviceIterator d = m_pDeviceRoot->beginDevice(); d != m_pDeviceRoot->endDevice(); ++d) {
        Device& rd = *d;
        device(rd);
        for (Device::ServiceIterator s = rd.beginService(); s != rd.endService(); ++s) {
            Service& rs = *s;
            service(rs);
            for (Service::StateVarIterator sv = rs.beginStateVar(); sv != rs.endStateVar(); ++sv) {
                StateVar& rsv = *sv;
                stateVar(rsv);
            }
            for (Service::ActionIterator a = rs.beginAction(); a != rs.endAction(); ++a) {
                Action& ra = *a;
                action(ra);
                for (Action::ArgumentIterator arg = ra.beginArgument(); arg != ra.endArgument(); ++arg) {
                    Argument& rarg = *arg;
                    argument(rarg);
                }
            }
        }
    }
}


void
DeviceH::deviceRoot(const DeviceRoot& deviceRoot)
{
}


void
DeviceH::device(const Device& device)
{
}


void
DeviceH::service(const Service& service)
{
}


void
DeviceH::stateVar(const StateVar& stateVar)
{
}


void
DeviceH::action(const Action& action)
{
}


void
DeviceH::argument(const Argument& argument)
{
}


void
DeviceImplH::deviceRoot(const DeviceRoot& deviceRoot)
{
}


void
DeviceImplH::device(const Device& device)
{
}


void
DeviceImplH::service(const Service& service)
{
}


void
DeviceImplH::stateVar(const StateVar& stateVar)
{
}


void
DeviceImplH::action(const Action& action)
{
}


void
DeviceImplH::argument(const Argument& argument)
{
}


/*
void
DeviceCpp::write()
{
}


void
DeviceCpp::deviceRoot(const DeviceRoot& deviceRoot)
{
}


void
DeviceCpp::device(const Device& device)
{
}


void
DeviceCpp::service(const Service& service)
{
}


void
DeviceCpp::action(const Action& action)
{
}
*/