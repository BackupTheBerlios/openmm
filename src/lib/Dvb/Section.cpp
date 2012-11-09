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

#include <Poco/Checksum.h>

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


void
Section::stuff()
{
    ::memset(_data, 0xff, _sizeMax);
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


void
Section::setTableId(Poco::UInt8 tid)
{
    setValue<Poco::UInt8>(0, 8, tid);
    _tableId = tid;
}


void
Section::setSyntaxIndicator(bool syntaxIndicator)
{
    setValue<Poco::UInt8>(8, 1, syntaxIndicator ? 1 : 0);
}


void
Section::setFixed()
{
    setValue<Poco::UInt8>(9, 1, 0);
}


void
Section::setLength(Poco::UInt16 sectionLength)
{
    setValue<Poco::UInt16>(12, 12, sectionLength);
    _size = sectionLength + 3;
}


void
Section::setTableIdExtension(Poco::UInt16 tidExt)
{
    setValue<Poco::UInt16>(3 * 8, 16, tidExt);
}


void
Section::setVersionNumber(Poco::UInt8 version)
{
    setValue<Poco::UInt8>(42, 5, version);
}


void
Section::setCurrentNextIndicator(bool currentNext)
{
    setValue<Poco::UInt8>(47, 1, currentNext ? 1 : 0);
}


void
Section::setSectionNumber(Poco::UInt8 section)
{
    setValue<Poco::UInt8>(48, 8, section);
}


void
Section::setLastSectionNumber(Poco::UInt8 lastSection)
{
    setValue<Poco::UInt8>(56, 8, lastSection);
}


void
Section::setCrc()
{
//    Poco::Checksum crc;
////    Poco::Checksum crc(Poco::Checksum::TYPE_CRC32);
////    Poco::Checksum crc(Poco::Checksum::TYPE_ADLER32);
//    crc.update((const char*)getData(), _size - 4);
////    getData()(_size - 4, crc.checksum());
//    setBytes<Poco::UInt32>(_size - 4, Poco::ByteOrder::flipBytes(crc.checksum()));

    char* data = (char*) getData();
    unsigned int crc32 = 0xffffffff;
    for (int i = 0; i < (_size - 4); ++i) {
		unsigned char byte = data[i];
		crc32 = (crc32 << 8) ^ crc32Table[(crc32 >> 24) ^ byte];
	}

	data[_size - 4] = crc32 >> 24;
	data[_size - 3] = crc32 >> 16;
	data[_size - 2] = crc32 >> 8;
	data[_size - 1] = crc32;
}


unsigned int
Section::size()
{
    return _size;
}


unsigned int
Section::timeout()
{
    return _timeout;
}


const unsigned int Section::crc32Table[] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
	0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
	0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
	0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
	0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
	0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
	0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
	0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
	0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
	0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
	0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
	0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
	0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
	0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
	0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
	0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
	0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
	0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
	0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
	0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
	0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
	0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
	0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
	0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
	0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
	0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
	0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
	0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
	0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
	0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
	0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
	0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
	0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
	0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
	0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
	0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
	0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
	0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
	0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
	0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
	0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
	0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
	0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
	0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
	0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
	0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
	0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
	0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
	0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
	0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
	0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
	0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
	0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
	0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
	0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
	0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
	0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
	0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
	0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
	0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
	0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
	0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
	0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
	0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};


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
    _serviceCount = (size() - 8 - 4) / 4;  // section header size = 8, crc = 4, service section size = 4
    unsigned int headerSize = 8 * 8;
    unsigned int serviceSize = 4 * 8;
    for (int i = 0; i < _serviceCount; i++) {
        _serviceIds.push_back(getValue<Poco::UInt16>(headerSize + i * serviceSize, 16));
        _pmtPids.push_back(getValue<Poco::UInt16>(headerSize + i * serviceSize + 19, 13));
    }
}


PatSection*
PatSection::create()
{
    PatSection* pPat = new PatSection;

    pPat->stuff();
    pPat->setTableId(0x00);
    pPat->setSyntaxIndicator(true);
    pPat->setFixed();
    pPat->setVersionNumber(0x00);
    pPat->setCurrentNextIndicator(true);
    pPat->setSectionNumber(0);
    pPat->setLastSectionNumber(0);

    return pPat;
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


void
PatSection::addService(Poco::UInt16 serviceId, Poco::UInt16 pmtPid, unsigned int index)
{
    setValue<Poco::UInt16>(64 + index * 4 * 8, 16, serviceId);
    setValue<Poco::UInt16>(83 + index * 4 * 8, 13, pmtPid);
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
    unsigned int totalStreamSectionSize = size() * 8 - headerSize - 4 * 8;
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
    unsigned int sdtLoopLength = size() - byteHead - 4;
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
