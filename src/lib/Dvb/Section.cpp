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

#include <vector>
#include <Poco/Types.h>

#include "Log.h"
#include "Descriptor.h"
#include "Section.h"
#include "Stream.h"


namespace Omm {
namespace Dvb {


Section::Section(Poco::UInt8 tableId) :
_tableId(tableId),
_sizeMax(4096),
_size(0)
{
    _data = new Poco::UInt8[_sizeMax];
}


Section::Section(const std::string& name, Poco::UInt16 pid, Poco::UInt8 tableId, unsigned int timeout) :
_name(name),
_pid(pid),
_tableId(tableId),
_sizeMax(4096),
_size(0),
_timeout(timeout)
{
    _data = new Poco::UInt8[_sizeMax];
}


Section::~Section()
{
    delete (Poco::UInt8*)_data;
}


void
Section::read(Stream* pStream, int timeout)
{
    pStream->read((Poco::UInt8*)_data, 3, timeout);

    Poco::UInt16 sectionLength = getValue<Poco::UInt16>(12, 12);

//    LOG(dvb, debug, "section length: " + Poco::NumberFormatter::format(sectionLength));
    pStream->read((Poco::UInt8*)(_data) + 3, sectionLength, timeout);
    _size = sectionLength + 3;
}


void
Section::read(Stream* pStream)
{
    pStream->read((Poco::UInt8*)_data, 3, _timeout);

    Poco::UInt16 sectionLength = getValue<Poco::UInt16>(12, 12);

//    LOG(dvb, debug, "section length: " + Poco::NumberFormatter::format(sectionLength));
    pStream->read((Poco::UInt8*)(_data) + 3, sectionLength, _timeout);
    _size = sectionLength + 3;
}


std::string
Section::name()
{
    return _name;
}


Poco::UInt16
Section::packetId()
{
    return _pid;
}


Poco::UInt8
Section::tableId()
{
    return _tableId;
}


Poco::UInt16
Section::tableIdExtension()
{
    return getValue<Poco::UInt16>(24, 16);
}


unsigned int
Section::length()
{
    return _size;
}


unsigned int
Section::timeout()
{
    return _timeout;
}


PatSection::PatSection() :
Section("PAT", 0x00, 0x00, 5),
_serviceCount(0)
{
}


void
PatSection::parse()
{
    _serviceCount = (length() - 8 - 4) / 4;  // section header size = 8, crc = 4, service section size = 4
    unsigned int headerSize = 8 * 8;
    unsigned int serviceSize = 4 * 8;
    for (int i = 0; i < _serviceCount; i++) {
        _serviceIds.push_back(getValue<Poco::UInt16>(headerSize + i * serviceSize, 16));
        _pmtPids.push_back(getValue<Poco::UInt16>(headerSize + i * serviceSize + 19, 13));
    }
}


Poco::UInt16
PatSection::transportStreamId()
{
    return tableIdExtension();
}


unsigned int
PatSection::serviceCount()
{
    return _serviceCount;
}


Poco::UInt16
PatSection::serviceId(unsigned int serviceIndex)
{
    return _serviceIds[serviceIndex];
}


Poco::UInt16
PatSection::pmtPid(unsigned int serviceIndex)
{
    return _pmtPids[serviceIndex];
}


PmtSection::PmtSection(Poco::UInt16 pid) :
Section("PMT", pid, 0x02, 5)
{
}


void
PmtSection::parse()
{
    _pcrPid = getValue<Poco::UInt16>(67, 13);
    Poco::UInt16 programInfoLength = getValue<Poco::UInt16>(84, 12);

    unsigned int headerSize = 96 + programInfoLength * 8;
    unsigned int totalStreamSectionSize = length() * 8 - headerSize - 4 * 8;
    unsigned int offset = 0;
    while (offset < totalStreamSectionSize) {
        _streamTypes.push_back(getValue<Poco::UInt8>(headerSize + offset, 8));
        _streamPids.push_back(getValue<Poco::UInt16>(headerSize + offset + 11, 13));
        Poco::UInt16 esInfoLength = getValue<Poco::UInt16>(headerSize + offset + 28, 12);
        if (!esInfoLength) {
            _esInfoDescriptors.push_back(0);
        }
        offset += 40 + esInfoLength * 8;
    }
}


Poco::UInt16
PmtSection::programNumber()
{
    return tableIdExtension();
}


Poco::UInt16
PmtSection::pcrPid()
{
    return _pcrPid;
}


unsigned int
PmtSection::streamCount()
{
    return _streamPids.size();
}


Poco::UInt8
PmtSection::streamType(unsigned int streamIndex)
{
    return _streamTypes[streamIndex];
}


Poco::UInt16
PmtSection::streamPid(unsigned int streamIndex)
{
    return _streamPids[streamIndex];
}


Descriptor*
PmtSection::esInfoDescriptor(unsigned int streamIndex)
{
    return _esInfoDescriptors[streamIndex];
}


SdtSection::SdtSection() :
Section("SDT", 0x11, 0x42, 5)
{
}


void
SdtSection::parse()
{
    unsigned int sdtHeaderSize = 88;
    unsigned int totalSdtSectionSize = length() * 8 - sdtHeaderSize - 4 * 8;
    unsigned int sdtOffset = 0;
    while (sdtOffset < totalSdtSectionSize) {
        _serviceIds.push_back(getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset, 16));
        _serviceRunningStatus.push_back(getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 24, 3));
        _serviceScrambled.push_back(getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 27, 1));
        Poco::UInt16 sdtInfoLength = getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset + 28, 12);

//    //        LOG(dvb, trace, "sdt descriptor total length: " + Poco::NumberFormatter::format(sdtInfoLength));
//            Poco::UInt8 descId = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 40, 8);
//    //        LOG(dvb, trace, "sdt descriptor tag: " + Poco::NumberFormatter::format(descId));
//            Poco::UInt8 descLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 48, 8);
//    //        LOG(dvb, trace, "sdt descriptor length: " + Poco::NumberFormatter::format(descLength));
//            Poco::UInt8 descType = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 56, 8);
//            LOG(dvb, trace, "sdt descriptor type: " + Poco::NumberFormatter::format(descType));
//            Poco::UInt8 descProvNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 64, 8);
//    //        LOG(dvb, trace, "sdt descriptor provider name length: " + Poco::NumberFormatter::format(descProvNameLength));
//            std::string providerName((char*)(sdtTable.getData()) + (sdtHeaderSize + sdtOffset + 72) / 8, descProvNameLength);
//            LOG(dvb, trace, "sdt provider name: " + providerName);
//            Poco::UInt8 descServiceNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 72 + descProvNameLength * 8, 8);
//    //        LOG(dvb, trace, "sdt descriptor service name length: " + Poco::NumberFormatter::format(descServiceNameLength));
//            std::string serviceName((char*)(sdtTable.getData()) + (sdtHeaderSize + sdtOffset + 80 + descProvNameLength * 8) / 8, descServiceNameLength);
//            LOG(dvb, trace, "sdt service name: " + serviceName);
//
//            serviceName = filter(serviceName);

        sdtOffset += 40 + sdtInfoLength * 8;
    }
}


unsigned int
SdtSection::serviceCount()
{
    return _serviceIds.size();
}


Poco::UInt16
SdtSection::serviceId(unsigned int serviceIndex)
{
    return _serviceIds[serviceIndex];
}


Poco::UInt8
SdtSection::runningStatus(unsigned int serviceIndex)
{
    return _serviceRunningStatus[serviceIndex];
}


bool
SdtSection::scrambled(unsigned int serviceIndex)
{
    return _serviceScrambled[serviceIndex];
}


NitSection::NitSection(Poco::UInt8 tableId) :
Section("NIT", 0x10, tableId, 15)
{
}


void
NitSection::parse()
{
    Poco::UInt16 networkDescriptorsLength = getValue<Poco::UInt16>(68, 12);
    unsigned int head = 80;
    unsigned int byteOffset = 0;
    while (byteOffset < networkDescriptorsLength) {
        Descriptor* pDescriptor = Descriptor::createDescriptor(getData(10 + byteOffset));
        _networkDescriptors.push_back(pDescriptor);
        if (NetworkNameDescriptor* pD = dynamic_cast<NetworkNameDescriptor*>(pDescriptor)) {
            _networkName = pD->getNetworkName();
        }
        // TODO: which other descriptors can be located here?
        if (pDescriptor) {
            byteOffset += pDescriptor->getDescriptorLength();
        }
        else {
            break;
        }
    }
    Poco::UInt16 transportStreamLoopLength = getValue<Poco::UInt16>(head + networkDescriptorsLength * 8 + 4, 12);
    head = head + networkDescriptorsLength * 8 + 16;
    byteOffset = 0;
    unsigned int transportStreamIndex = 0;
    while (byteOffset < transportStreamLoopLength) {
        _transportStreamIds.push_back(getValue<Poco::UInt16>(head + byteOffset * 8, 16));
        _originalNetworkIds.push_back(getValue<Poco::UInt16>(head + byteOffset * 8 + 16, 16));
        _transportStreamDescriptors.push_back(std::vector<Descriptor*>());

        Poco::UInt16 transportDescriptorsLength = getValue<Poco::UInt16>(head + byteOffset * 8 + 36, 12);
        unsigned int transportHead = head + byteOffset * 8 + 48;
        unsigned int transportByteOffset = 0;
        while (transportByteOffset < transportDescriptorsLength) {
            Descriptor* pDescriptor = Descriptor::createDescriptor(getData(transportHead / 8 + transportByteOffset));
            _transportStreamDescriptors[transportStreamIndex].push_back(pDescriptor);
            if (pDescriptor) {
                transportByteOffset += pDescriptor->getDescriptorLength();
            }
            else {
                break;
            }
        }
        byteOffset += transportDescriptorsLength + 6;
        transportStreamIndex++;
    }
}


Poco::UInt16
NitSection::networkId()
{
    return tableIdExtension();
}


std::string
NitSection::networkName()
{
    return _networkName;
}


unsigned int
NitSection::networkDescriptorCount()
{
    return _networkDescriptors.size();
}


Descriptor*
NitSection::networkDescriptor(unsigned int networkDescriptorIndex)
{
    return _networkDescriptors[networkDescriptorIndex];
}


unsigned int
NitSection::transportStreamCount()
{
    return _transportStreamIds.size();
}


Poco::UInt16
NitSection::transportStreamId(unsigned int transportStreamIndex)
{
    return _transportStreamIds[transportStreamIndex];
}


Poco::UInt16
NitSection::originalNetworkId(unsigned int transportStreamIndex)
{
    return _originalNetworkIds[transportStreamIndex];
}


unsigned int
NitSection::transportStreamDescriptorCount(unsigned int transportStreamIndex)
{
    return _transportStreamDescriptors[transportStreamIndex].size();
}


Descriptor*
NitSection::transportStreamDescriptor(unsigned int transportStreamIndex, unsigned int transportStreamDescriptorIndex)
{
    return _transportStreamDescriptors[transportStreamIndex][transportStreamDescriptorIndex];
}


}  // namespace Omm
}  // namespace Dvb
