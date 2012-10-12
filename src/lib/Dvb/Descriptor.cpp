/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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

#include "Log.h"
#include "DvbLogger.h"
#include "Descriptor.h"
#include "Service.h"
#include "Transponder.h"


namespace Omm {
namespace Dvb {


//Descriptor::Descriptor(void* data) :
//_data(data)
//{
//
//}


Descriptor*
Descriptor::createDescriptor(void* data)
{
    Descriptor* pRes = 0;
    LOG(dvb, debug, "descriptor tag: 0x" + Poco::NumberFormatter::formatHex(*(Poco::UInt8*)(data)));
    switch (*(Poco::UInt8*)(data)) {
        case 0x40:
            LOG(dvb, debug, "create network name descriptor");
            pRes = new NetworkNameDescriptor;
            break;
        case 0x41:
            LOG(dvb, debug, "create service list descriptor");
            pRes = new ServiceListDescriptor;
            break;
        case 0x43:
            LOG(dvb, debug, "create satellite delivery system descriptor");
            pRes = new SatelliteDeliverySystemDescriptor;
            break;
        case 0x48:
            LOG(dvb, debug, "create service descriptor");
            pRes = new ServiceDescriptor;
            break;
        case 0x5A:
            LOG(dvb, debug, "create terrestrial delivery system descriptor");
            pRes = new TerrestrialDeliverySystemDescriptor;
            break;
        case 0x62:
            LOG(dvb, debug, "create frequency list descriptor");
            pRes = new FrequencyListDescriptor;
            break;
        case 0x6D:
            LOG(dvb, debug, "create cell frequency link descriptor");
            pRes = new CellFrequencyLinkDescriptor;
            break;
        default:
            LOG(dvb, warning, "descriptor type unkown");
    }
    if (pRes) {
        pRes->_data = data;
    }
    return pRes;
}


//Descriptor::id
Poco::UInt8
Descriptor::getId()
{
    return *(Poco::UInt8*)(_data);
//    return *(id*)(_data);
}


Poco::UInt8
Descriptor::getDescriptorLength()
{
    return *((Poco::UInt8*)(_data) + 1) + 2;
}


Poco::UInt8
Descriptor::getContentLength()
{
    return *((Poco::UInt8*)(_data) + 1);
}


void*
Descriptor::content()
{
    return (Poco::UInt8*)(_data) + 2;
}


std::string
NetworkNameDescriptor::getNetworkName()
{
    return std::string((char*)content(), getContentLength());
}


Poco::UInt8
ServiceDescriptor::serviceType()
{
    return *((Poco::UInt8*)content());
}


std::string
ServiceDescriptor::providerName()
{
    Poco::UInt8 providerNameLength = *((Poco::UInt8*)content() + 1);
    return filter(std::string((char*)content() + 2, providerNameLength));
}


std::string
ServiceDescriptor::serviceName()
{
    Poco::UInt8 providerNameLength = *((Poco::UInt8*)content() + 1);
    Poco::UInt8 serviceNameLength = *((Poco::UInt8*)content() + 2 + providerNameLength);
    return filter(std::string((char*)content() + 3 + providerNameLength, serviceNameLength));
}


Poco::UInt8
ServiceListDescriptor::serviceCount()
{
    return getContentLength() / 3;
}


Poco::UInt16
ServiceListDescriptor::serviceId(Poco::UInt8 index)
{
    return getBytes<Poco::UInt16>(2 + index * 3);

}


Poco::UInt8
ServiceListDescriptor::serviceType(Poco::UInt8 index)
{
    return getBytes<Poco::UInt8>(2 + index * 3 + 2);
//    return *((Poco::UInt8*)(content()) + index * 3 + 2);
}


unsigned int
SatelliteDeliverySystemDescriptor::frequency()
{
    return getBcd<unsigned int>(2, 8) * 10;
}


std::string
SatelliteDeliverySystemDescriptor::orbitalPosition()
{
    return getBcdString(6, 4, 1);
//    return getBcd<float>(6, 4) / 10;
}


std::string
SatelliteDeliverySystemDescriptor::polarization()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8 + 1, 2);
    switch(val) {
        case 0x00:
            return SatTransponder::POL_HORIZ;
        case 0x01:
            return SatTransponder::POL_VERT;
        case 0x02:
            return SatTransponder::POL_CIRC_LEFT;
        case 0x03:
            return SatTransponder::POL_CIRC_RIGHT;
        default:
            return "invalid polarization";
    }
}


std::string
SatelliteDeliverySystemDescriptor::modulationSystem()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8 + 5, 1);
    switch(val) {
        case 0x00:
            return SatTransponder::MOD_S;
        case 0x01:
            return SatTransponder::MOD_S2;
        default:
            return "invalid modulation system";
    }
}


std::string
SatelliteDeliverySystemDescriptor::modulationType()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8 + 6, 2);
    switch(val) {
        case 0x00:
            return SatTransponder::MOD_TYPE_AUTO;
        case 0x01:
            return SatTransponder::MOD_TYPE_QPSK;
        case 0x02:
            return SatTransponder::MOD_TYPE_8PSK;
        case 0x03:
            return SatTransponder::MOD_TYPE_16QAM;
        default:
            return "invalid modulation type";
    }
}


Poco::UInt32
SatelliteDeliverySystemDescriptor::symbolRate()
{
    return getBcd<Poco::UInt32>(9, 7) * 100;
}


std::string
SatelliteDeliverySystemDescriptor::fecInner()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(12 * 8 + 4, 4);
    switch(val) {
        case 0x00:
            return SatTransponder::FEC_NOT_DEFINED;
        case 0x01:
            return SatTransponder::FEC_1_2;
        case 0x02:
            return SatTransponder::FEC_2_3;
        case 0x03:
            return SatTransponder::FEC_3_4;
        case 0x04:
            return SatTransponder::FEC_5_6;
        case 0x05:
            return SatTransponder::FEC_7_8;
        case 0x06:
            return SatTransponder::FEC_8_9;
        case 0x07:
            return SatTransponder::FEC_3_5;
        case 0x08:
            return SatTransponder::FEC_4_5;
        case 0x09:
            return SatTransponder::FEC_9_10;
        case 0x0f:
            return SatTransponder::FEC_NO_CODING;
        default:
            return "invalid fec inner";
    }
}


unsigned int
TerrestrialDeliverySystemDescriptor::centreFrequency()
{
    return getBytes<Poco::UInt32>(2) * 10;
}


std::string
TerrestrialDeliverySystemDescriptor::bandwidth()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(6 * 8, 3);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::BANDWIDTH_8_MHZ;
        case 0x01:
            return TerrestrialTransponder::BANDWIDTH_7_MHZ;
        case 0x02:
            return TerrestrialTransponder::BANDWIDTH_6_MHZ;
        case 0x03:
            return TerrestrialTransponder::BANDWIDTH_5_MHZ;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::priority()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(6 * 8 + 3, 1);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::PRIORITY_LOW;
        case 0x01:
            return TerrestrialTransponder::PRIORITY_HIGH;
        default:
            return "";
    }
}


bool
TerrestrialDeliverySystemDescriptor::timeSlicingIndicator()
{
    return getValue<Poco::UInt8>(6 * 8 + 4, 1);

}


bool
TerrestrialDeliverySystemDescriptor::MpeFecIndicator()
{
    return getValue<Poco::UInt8>(6 * 8 + 5, 1);

}


std::string
TerrestrialDeliverySystemDescriptor::constellation()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(7 * 8, 2);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::QPSK;
        case 0x01:
            return TerrestrialTransponder::QAM_16;
        case 0x02:
            return TerrestrialTransponder::QAM_64;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::hierarchyInformation()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(7 * 8 + 2, 3);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::HIERARCHY_NONE;
        case 0x01:
            return TerrestrialTransponder::HIERARCHY_1;
        case 0x02:
            return TerrestrialTransponder::HIERARCHY_2;
        case 0x03:
            return TerrestrialTransponder::HIERARCHY_4;
        case 0x04:
            return TerrestrialTransponder::HIERARCHY_NONE;
        case 0x05:
            return TerrestrialTransponder::HIERARCHY_1;
        case 0x06:
            return TerrestrialTransponder::HIERARCHY_2;
        case 0x07:
            return TerrestrialTransponder::HIERARCHY_4;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::codeRateHpStream()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(7 * 8 + 5, 3);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::FEC_1_2;
        case 0x01:
            return TerrestrialTransponder::FEC_2_3;
        case 0x02:
            return TerrestrialTransponder::FEC_3_4;
        case 0x03:
            return TerrestrialTransponder::FEC_5_6;
        case 0x04:
            return TerrestrialTransponder::FEC_7_8;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::codeRateLpStream()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8, 3);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::FEC_1_2;
        case 0x01:
            return TerrestrialTransponder::FEC_2_3;
        case 0x02:
            return TerrestrialTransponder::FEC_3_4;
        case 0x03:
            return TerrestrialTransponder::FEC_5_6;
        case 0x04:
            return TerrestrialTransponder::FEC_7_8;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::guardInterval()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8 + 3, 2);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::GUARD_INTERVAL_1_32;
        case 0x01:
            return TerrestrialTransponder::GUARD_INTERVAL_1_16;
        case 0x02:
            return TerrestrialTransponder::GUARD_INTERVAL_1_8;
        case 0x03:
            return TerrestrialTransponder::GUARD_INTERVAL_1_4;
        default:
            return "";
    }
}


std::string
TerrestrialDeliverySystemDescriptor::transmissionMode()
{
    Poco::UInt8 val = getValue<Poco::UInt8>(8 * 8 + 5, 2);
    switch (val) {
        case 0x00:
            return TerrestrialTransponder::TRANSMISSION_MODE_2K;
        case 0x01:
            return TerrestrialTransponder::TRANSMISSION_MODE_8K;
        case 0x02:
            return TerrestrialTransponder::TRANSMISSION_MODE_4K;
        default:
            return "";
    }
}


bool
TerrestrialDeliverySystemDescriptor::otherFrequencyFlag()
{
    return getValue<Poco::UInt8>(8 * 8 + 7, 1);

}


std::string
FrequencyListDescriptor::codingType()
{

}


Poco::UInt8
FrequencyListDescriptor::centreFrequencyCount()
{
    return (getContentLength() - 1) / 4;
}


unsigned int
FrequencyListDescriptor::centreFrequency(Poco::UInt8 index)
{
    return getBytes<Poco::UInt32>(3 + index * 4) * 10;
}


}  // namespace Omm
}  // namespace Dvb
