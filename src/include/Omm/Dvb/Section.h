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

#ifndef Section_INCLUDED
#define Section_INCLUDED

#include "DvbUtil.h"


namespace Omm {
namespace Dvb {

class Descriptor;
class Stream;
class Section;


class Table
{
public:
    Table(Section& firstSection);
    ~Table();

    void read(Stream* pStream);
    void parse();
    int sectionCount();
    Section* getFirstSection();
    Section* getSection(int index);

private:
    Section*                    _pFirstSection;
    std::vector<Section*>       _sections;
};



class Section : public BitField
{
public:
    Section(Poco::UInt8 tableId);
    Section(const std::string& name, Poco::UInt16 pid, Poco::UInt8 tableId, unsigned int timeout);
    ~Section();

    void read(Stream* pStream);
    void stuff();
    virtual Section* clone();
    virtual void parse() {}

    std::string name();
    Poco::UInt16 packetId();
    Poco::UInt8 tableId();
    Poco::UInt16 tableIdExtension();
    Poco::UInt8 sectionNumber();
    Poco::UInt8 lastSectionNumber();

    void setTableId(Poco::UInt8 tid);
    void setSyntaxIndicator(bool syntaxIndicator);
    void setFixed();
    void setLength(Poco::UInt16 sectionLength);
    void setTableIdExtension(Poco::UInt16 tidExt);
    void setVersionNumber(Poco::UInt8 version);
    void setCurrentNextIndicator(bool currentNext);
    void setSectionNumber(Poco::UInt8 section);
    void setLastSectionNumber(Poco::UInt8 lastSection);
    void setCrc();

    unsigned int size();
    unsigned int timeout();

private:
    static const unsigned int crc32Table[];
    
    std::string         _name;
    Poco::UInt16        _pid;
    Poco::UInt8         _tableId;
    const unsigned int  _sizeMax;
    unsigned int        _size;
    unsigned int        _timeout;
};


class PatSection : public Section
{
public:
    PatSection();

    virtual Section* clone();
    virtual void parse();
    static PatSection* create();

    Poco::UInt16 transportStreamId();

    unsigned int serviceCount();
    Poco::UInt16 serviceId(unsigned int serviceIndex);
    Poco::UInt16 pmtPid(unsigned int serviceIndex);

    void addService(Poco::UInt16 serviceId, Poco::UInt16 pmtPid, unsigned int index);

private:
    unsigned int                        _serviceCount;
    std::vector<Poco::UInt16>           _serviceIds;
    std::vector<Poco::UInt16>           _pmtPids;
};


class PmtSection : public Section
{
public:
    PmtSection(Poco::UInt16 pid);

    virtual Section* clone();
    virtual void parse();

    Poco::UInt16 programNumber();
    Poco::UInt16 pcrPid();
    unsigned int streamCount();
    Poco::UInt8 streamType(unsigned int streamIndex);
    Poco::UInt16 streamPid(unsigned int streamIndex);
    Descriptor* esInfoDescriptor(unsigned int streamIndex);

private:
    Poco::UInt16                        _pcrPid;
    std::vector<Poco::UInt8>            _streamTypes;
    std::vector<Poco::UInt16>           _streamPids;
    std::vector<Descriptor*>            _esInfoDescriptors;
};


class TsdtSection : public Section
{

};


class SdtSection : public Section
{
public:
    SdtSection();

    virtual Section* clone();
    virtual void parse();

    unsigned int serviceCount();
    Poco::UInt16 serviceId(unsigned int serviceIndex);
    std::string runningStatus(unsigned int serviceIndex);
    bool scrambled(unsigned int serviceIndex);
    unsigned int serviceDescriptorCount(unsigned int serviceIndex);
    Descriptor* serviceDescriptor(unsigned int serviceIndex, int serviceDescriptorIndex);

private:
    std::vector<Poco::UInt16>                   _serviceIds;
    std::vector<Poco::UInt8>                    _serviceRunningStatus;
    std::vector<bool>                           _serviceScrambled;
    std::vector<std::vector<Descriptor*> >      _serviceDescriptors;
};


class NitSection : public Section
{
public:
    enum {NitActualTableId = 0x40, NitOtherTableId = 0x41};

    NitSection(Poco::UInt8 tableId);

    virtual Section* clone();
    virtual void parse();

    Poco::UInt16 networkId();
    std::string networkName();
    unsigned int networkDescriptorCount();
    Descriptor* networkDescriptor(unsigned int networkDescriptorIndex);

    unsigned int transportStreamCount();
    Poco::UInt16 transportStreamId(unsigned int transportStreamIndex);
    Poco::UInt16 originalNetworkId(unsigned int transportStreamIndex);
    unsigned int transportStreamDescriptorCount(unsigned int transportStreamIndex);
    Descriptor* transportStreamDescriptor(unsigned int transportStreamIndex, unsigned int transportStreamDescriptorIndex);

private:
    std::string                                 _networkName;
    std::vector<Poco::UInt16>                   _transportStreamIds;
    std::vector<Poco::UInt16>                   _originalNetworkIds;
    std::vector<Descriptor*>                    _networkDescriptors;
    std::vector<std::vector<Descriptor*> >      _transportStreamDescriptors;
};


class NitActualSection : public NitSection
{
public:
    NitActualSection() : NitSection(NitActualTableId) {}
};


class NitOtherSection : public NitSection
{
public:
    NitOtherSection() : NitSection(NitOtherTableId) {}
};


}  // namespace Omm
}  // namespace Dvb

#endif
