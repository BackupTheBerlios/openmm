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

#include "Descriptor.h"
#include "Section.h"
#include "Stream.h"
#include "Dvb/Section.h"
#include "DvbLogger.h"
#include "Dvb/Service.h"


namespace Omm {
namespace Dvb {


Table::Table(Section& firstSection) :
_pFirstSection(firstSection.clone())
{
}


Table::~Table()
{
    for (std::vector<Section*>::iterator it = _sections.begin(); it != _sections.end(); ++it) {
        delete (*it);
        *it = 0;
    }
}


void
Table::read(Stream* pStream)
{
    int attempt = 1;
    LOG(dvb, trace, "table section read attempt: " + Poco::NumberFormatter::format(attempt));
    _pFirstSection->read(pStream);
    attempt++;
    int sectionCount = _pFirstSection->lastSectionNumber() + 1;
    int maxAttempts = sectionCount + 3;
    LOG(dvb, trace, "table section count: " + Poco::NumberFormatter::format(sectionCount));
    for (int i = 0; i < sectionCount; i++) {
        _sections.push_back(0);
    }
    int sectionNumber = _pFirstSection->sectionNumber();
    LOG(dvb, trace, "table section number of first section: " + Poco::NumberFormatter::format(sectionNumber));
    _sections[sectionNumber] = _pFirstSection;
    int sectionsRead = 1;
    while (sectionsRead < sectionCount && attempt < maxAttempts) {
        LOG(dvb, trace, "table section read attempt: " + Poco::NumberFormatter::format(attempt));
        Section* pS = _pFirstSection->clone();
        pS->read(pStream);
        int sNumber = pS->sectionNumber();
        LOG(dvb, trace, "table section number of next section: " + Poco::NumberFormatter::format(sNumber));
        if (!_sections[sNumber]) {
            _sections[sNumber] = pS;
            sectionsRead++;
        }
        attempt++;
    }
    if (sectionsRead == sectionCount) {
        LOG(dvb, trace, "table read all sections");
    }
    else {
        LOG(dvb, trace, "table max read attempt reached");
    }
}


void
Table::parse()
{
    for (std::vector<Section*>::iterator it = _sections.begin(); it != _sections.end(); ++it) {
        (*it)->parse();
    }
}


int
Table::sectionCount()
{
    return _sections.size();
}


Section*
Table::getFirstSection()
{
    return _pFirstSection;
}


Section*
Table::getSection(int index)
{
    return _sections[index];
}


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
Section::read(Stream* pStream)
{
    pStream->read((Poco::UInt8*)_data, 3, _timeout);

    Poco::UInt16 sectionLength = getValue<Poco::UInt16>(12, 12);

//    LOG(dvb, debug, "section length: " + Poco::NumberFormatter::format(sectionLength));
    pStream->read((Poco::UInt8*)(_data) + 3, sectionLength, _timeout);
    _size = sectionLength + 3;
}


Section*
Section::clone()
{
    return new Section(_name, _pid, _tableId, _timeout);
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
    return getValue<Poco::UInt16>(3 * 8, 16);
}


Poco::UInt8
Section::sectionNumber()
{
    return getValue<Poco::UInt8>(6 * 8, 8);
}


Poco::UInt8
Section::lastSectionNumber()
{
    return getValue<Poco::UInt8>(7 * 8, 8);
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


Section*
PatSection::clone()
{
    return new PatSection;
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


Section*
PmtSection::clone()
{
    return new PmtSection(packetId());
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


Section*
SdtSection::clone()
{
    return new SdtSection;
}


void
SdtSection::parse()
{
    unsigned int byteHead = 11;
    unsigned int sdtLoopLength = length() - byteHead - 4;
    unsigned int byteOffset = byteHead;
    unsigned int serviceIndex = 0;
    while (byteOffset < sdtLoopLength) {
        _serviceDescriptors.push_back(std::vector<Descriptor*>());
        Poco::UInt16 serviceId = getValue<Poco::UInt16>(byteOffset * 8, 16);
        _serviceIds.push_back(serviceId);
        _serviceRunningStatus.push_back(getValue<Poco::UInt8>(byteOffset * 8 + 24, 3));
        _serviceScrambled.push_back(getValue<Poco::UInt8>(byteOffset * 8 + 27, 1));

        Poco::UInt16 serviceDescriptorsLength = getValue<Poco::UInt16>(byteOffset * 8 + 28, 12);
        unsigned int serviceByteHead = byteOffset + 5;
        unsigned int serviceByteOffset = 0;
        while (serviceByteOffset < serviceDescriptorsLength) {
            Descriptor* pDescriptor = Descriptor::createDescriptor(getData(serviceByteHead + serviceByteOffset));
            _serviceDescriptors[serviceIndex].push_back(pDescriptor);
            if (pDescriptor) {
                serviceByteOffset += pDescriptor->getDescriptorLength();
            }
            else {
                break;
            }
        }
        byteOffset += serviceDescriptorsLength + 5;
        serviceIndex++;
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


std::string
SdtSection::runningStatus(unsigned int serviceIndex)
{
    return Service::statusToString(_serviceRunningStatus[serviceIndex]);
}


bool
SdtSection::scrambled(unsigned int serviceIndex)
{
    return _serviceScrambled[serviceIndex];
}


unsigned int
SdtSection::serviceDescriptorCount(unsigned int serviceIndex)
{
    return _serviceDescriptors[serviceIndex].size();
}


Descriptor*
SdtSection::serviceDescriptor(unsigned int serviceIndex, int serviceDescriptorIndex)
{
    return _serviceDescriptors[serviceIndex][serviceDescriptorIndex];
}


//std::string
//SdtSection::providerName(unsigned int serviceIndex)
//{
//    return _serviceProviderName[serviceIndex];
//}


//std::string
//SdtSection::serviceName(unsigned int serviceIndex)
//{
//    return _serviceName[serviceIndex];
//}


NitSection::NitSection(Poco::UInt8 tableId) :
Section("NIT", 0x10, tableId, 15)
{
}


Section*
NitSection::clone()
{
    return new NitSection(tableId());
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
