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

#ifndef Descriptor_INCLUDED
#define Descriptor_INCLUDED


#include "DvbUtil.h"

namespace Omm {
namespace Dvb {

class Service;

class Descriptor : public BitField
{
public:
//    Descriptor(void* data);
    static Descriptor* createDescriptor(void* data);

    virtual void foo() {}
    Poco::UInt8 getId();
    Poco::UInt8 getDescriptorLength();
    Poco::UInt8 getContentLength();

protected:
    void* content();
};


class NetworkNameDescriptor : public Descriptor
{
public:
    std::string getNetworkName();
};


class ServiceListDescriptor : public Descriptor
{
public:
    Poco::UInt8 serviceCount();
    Poco::UInt16 serviceId(Poco::UInt8 index);
    Poco::UInt8 serviceType(Poco::UInt8 index);
};


class SatelliteDeliverySystemDescriptor : public Descriptor
{
public:
    unsigned int frequency();
    /// returns frequency in kHz
    float orbitalPosition();
    std::string polarization();
    std::string modulationSystem();
    std::string modulationType();
    Poco::UInt32 symbolRate();
    std::string fecInner();
};


class TerrestrialDeliverySystemDescriptor : public Descriptor
{
public:
    unsigned int centreFrequency();
    std::string bandwidth();
    std::string priority();
    bool timeSlicingIndicator();
    bool MpeFecIndicator();
    std::string constellation();
    std::string hierarchyInformation();
    std::string codeRateHpStream();
    std::string codeRateLpStream();
    std::string guardInterval();
    std::string transmissionMode();
    bool otherFrequencyFlag();
};


class FrequencyListDescriptor : public Descriptor
{
public:
    std::string codingType();
    Poco::UInt8 centreFrequencyCount();
    unsigned int centreFrequency(Poco::UInt8 index);
};


class CellFrequencyLinkDescriptor : public Descriptor
{
public:
};


}  // namespace Omm
}  // namespace Dvb

#endif
