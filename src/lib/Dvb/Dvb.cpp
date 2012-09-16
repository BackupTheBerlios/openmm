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

#include <fstream>
#include <sstream>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cstring>

#include <Poco/File.h>
#include <Poco/String.h>
#include <Poco/StreamCopier.h>
#include <Poco/BufferedStreamBuf.h>
//#include <Poco/UnbufferedStreamBuf.h>
#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
#include <Poco/DOM/AbstractContainerNode.h>
#include <Poco/DOM/DOMException.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/SAX/InputSource.h>
#include <Poco/Zip/ZipArchive.h>
#include <Poco/Zip/ZipStream.h>

#include <linux/dvb/frontend.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Thread.h>

#include "Dvb.h"
#include "Log.h"
#include "TransponderData.h"


namespace Omm {
namespace Dvb {

#ifndef NDEBUG
Log* Log::_pInstance = 0;

// possible log levels: trace, debug, information, notice, warning, error, critical, fatal

Log::Log()
{
    Poco::Channel* pChannel = Util::Log::instance()->channel();
//    _pDvbLogger = &Poco::Logger::create("DVB", pChannel, Poco::Message::PRIO_DEBUG);
    _pDvbLogger = &Poco::Logger::create("DVB", pChannel, Poco::Message::PRIO_TRACE);
}


Log*
Log::instance()
{
    if (!_pInstance) {
        _pInstance = new Log;
    }
    return _pInstance;
}


Poco::Logger&
Log::dvb()
{
    return *_pDvbLogger;
}
#endif // NDEBUG


class UnixFileStreamBuf : public Poco::BufferedStreamBuf
//class UnixFileStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    UnixFileStreamBuf(int fileDesc, int bufSize) : BasicBufferedStreamBuf(bufSize, std::ios_base::in), _fileDesc(fileDesc) {}
//    UnixFileStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        return read(_fileDesc, buffer, length);
    }

private:
    int         _fileDesc;
};


class UnixFileIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    UnixFileIStream(int fileDesc, int bufSize = 2048) : _streamBuf(fileDesc, bufSize), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    UnixFileStreamBuf   _streamBuf;
};


class ByteQueueStreamBuf : public Poco::BufferedStreamBuf
//class ByteQueueStreamBuf : public Poco::UnbufferedStreamBuf
{
public:
    ByteQueueStreamBuf(AvStream::ByteQueue& byteQueue) : BasicBufferedStreamBuf(byteQueue.size(), std::ios_base::in), _byteQueue(byteQueue) {}
//    ByteQueueStreamBuf(int fileDesc) : BasicUnbufferedStreamBuf(), _fileDesc(fileDesc) {}

    virtual int readFromDevice(char_type* buffer, std::streamsize length)
    {
        _byteQueue.read(buffer, length);
        return length;
    }

private:
    AvStream::ByteQueue&         _byteQueue;
};


class ByteQueueIStream : public std::basic_istream<char, std::char_traits<char> >
{
public:
    ByteQueueIStream(AvStream::ByteQueue& byteQueue) : _streamBuf(byteQueue), std::basic_istream<char, std::char_traits<char> >(&_streamBuf) { clear(); }

private:
    ByteQueueStreamBuf   _streamBuf;
};


//Channel::Channel(const std::string& name, unsigned int satNum, unsigned int freq, Polarization pol, unsigned int symbolRate, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid) :
//_name(name),
//_satNum(satNum),
//_freq(freq),
//_pol(pol),
//_symbolRate(symbolRate),
//_vpid(vpid),
//_cpid(cpid),
//_apid(apid),
//_sid(sid),
//_tid(tid),
//_pmtid(pmtid)
//{
//}
//
//
//std::string
//Channel::getName()
//{
//    return _name;
//}
//
//
//Channels::ChannelIterator
//Channels::beginChannel()
//{
//    return _channelMap.begin();
//}
//
//
//Channels::ChannelIterator
//Channels::endChannel()
//{
//    return _channelMap.end();
//}
//
//
//Channel*
//Channels::getChannel(const std::string& name)
//{
//    return _channelMap[name];
//}
//
//
//void
//ChannelsConf::setConfFilePath(const std::string& confFilePath)
//{
//    _confFilePath = confFilePath;
//}
//
//
//void
//ChannelsConf::scanChannels()
//{
//    scanChannelConfig(_confFilePath);
//}
//
//
//void
//ChannelsConf::scanChannelConfig(const std::string& channelConfig)
//{
//    LOG(dvb, debug, "scan channel config ...");
//
//    clearChannels();
//
//    std::ifstream channels(channelConfig.c_str());
//    std::string line;
//    while (getline(channels, line)) {
//        Poco::StringTokenizer channelParams(line, ":");
//        Poco::StringTokenizer channelName(channelParams[0], ";");
//        unsigned int freq = Poco::NumberParser::parseUnsigned(channelParams[1]) * 1000;
//        Channel::Polarization pol = (channelParams[2][0] == 'h') ? Channel::HORIZ : Channel::VERT;
//        unsigned int symbolRate = Poco::NumberParser::parseUnsigned(channelParams[4]) * 1000;
//        Poco::StringTokenizer videoPid(channelParams[5], "+");
//        unsigned int vpid = Poco::NumberParser::parseUnsigned(videoPid[0]);
//        unsigned int cpid = vpid;
//        if (videoPid.count() > 1) {
//            cpid = Poco::NumberParser::parseUnsigned(videoPid[1]);
//        }
//        Poco::StringTokenizer audioChannel(channelParams[6], ";");
//        Poco::StringTokenizer audioPid(audioChannel[0], "=");
//        unsigned int apid = Poco::NumberParser::parseUnsigned(audioPid[0]);
//        int sid = Poco::NumberParser::parseUnsigned(channelParams[9]);
//        unsigned int tid = Poco::NumberParser::parseUnsigned(channelParams[11]);
//        unsigned int pmtid = 0;
//        if (channelParams.count() > 13) {
//            pmtid = Poco::NumberParser::parseUnsigned(channelParams[13]);
//        }
//        _channelMap[channelName[0]] = new Omm::Dvb::Channel(channelName[0], 0, freq, pol, symbolRate, vpid, cpid, apid, sid, tid, pmtid);
//    }
//
//    LOG(dvb, debug, "scan channel config finished.");
//}
//
//
//void
//ChannelsConf::clearChannels()
//{
//    for (std::map<std::string, Omm::Dvb::Channel*>::iterator it = _channelMap.begin(); it != _channelMap.end(); ++it) {
//        delete it->second;
//    }
//    _channelMap.clear();
//}


Section::Section(Poco::UInt8 tableId) :
_tableId(tableId),
_sizeMax(4096),
_size(0)
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
    pStream->read((Poco::UInt8*)_data, 3);

    Poco::UInt16 sectionLength = getValue<Poco::UInt16>(12, 12);

    LOG(dvb, debug, "section length: " + Poco::NumberFormatter::format(sectionLength));
    pStream->read((Poco::UInt8*)(_data) + 3, sectionLength);
    _size = sectionLength + 3;
}


unsigned int
Section::length()
{
    return _size;
}


void*
Section::data()
{
    return _data;
}


const std::string Stream::Video("video");
const std::string Stream::Audio("audio");
const std::string Stream::VideoMpeg1_11172("videoMpeg1_11172");
const std::string Stream::VideoMpeg2_H262("videoMpeg2_H262");
const std::string Stream::AudioMpeg1_11172("audioMpeg1_11172");
const std::string Stream::AudioMpeg2_13818_3("audioMpeg2_13818_3");
const std::string Stream::Mpeg2PrivateTableSections("mpeg2PrivateTableSections");
const std::string Stream::Mpeg2PesPrivateData("mpeg2PesPrivateData");
const std::string Stream::MhegPackets("mhegPackets");
const std::string Stream::Mpeg2AnnexA_DSMCC("mpeg2AnnexA_DSMCC");
const std::string Stream::ITUTRecH2221("itu-TRecH2221");
const std::string Stream::ISO13818_6_typeA("iso-13818_6_typeA");
const std::string Stream::ISO13818_6_typeB("iso-13818_6_typeB");
const std::string Stream::ISO13818_6_typeC("iso-13818_6_typeC");
const std::string Stream::ISO13818_6_typeD("iso-13818_6_typeD");
const std::string Stream::Mpeg2ISO13818_1_Aux("mpeg2ISO13818_1_Aux");
const std::string Stream::AudioISO13818_7_ADTS("audioISO13818_7_ADTS");
const std::string Stream::Mpeg4ISO14496_2("mpeg4ISO14496_2");
const std::string Stream::AudioISO14496_3("audioISO14496_3");
const std::string Stream::ISO14496_1_PesPackets("iso-14496_1_PesPackets");
const std::string Stream::ISO14496_1_Sections("iso-14496_1_Sections");
const std::string Stream::ISO13818_6_DownloadProt("iso-13818_6_DownloadProt");
const std::string Stream::MetaDataPesPackets("metaDataPesPackets");
const std::string Stream::MetaDataSections("metaDataSections");
const std::string Stream::Mpeg2UserPrivate("mpeg2UserPrivate");
const std::string Stream::AudioAtscAc3("audioAtscAc3");
const std::string Stream::ProgramClock("programClock");
const std::string Stream::ProgramMapTable("programMapTable");
const std::string Stream::Other("other");

Stream::Stream(const std::string& type, Poco::UInt16 pid) :
_type(type),
_pid(pid),
_fileDesc(-1)
{
}


void
Stream::readXml(Poco::XML::Node* pXmlStream)
{

}


void
Stream::writeXml(Poco::XML::Element* pService)
{
    LOG(dvb, debug, "write stream ...");

    Poco::XML::Document* pDoc = pService->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pStream = pDoc->createElement("stream");
    pService->appendChild(pStream);
    pStream->setAttribute("type", _type);
    pStream->setAttribute("pid", Poco::NumberFormatter::format(_pid));

    LOG(dvb, debug, "wrote stream.");
}


std::string
Stream::getType()
{
    return _type;
}


unsigned int
Stream::getPid()
{
    return _pid;
}


int
Stream::getFileDesc()
{
    return _fileDesc;
}


void
Stream::read(Poco::UInt8* buf, int size)
{
    ::read(_fileDesc, buf, size);
}


Poco::UInt8
Stream::streamTypeFromString(const std::string& val)
{
    if (val == VideoMpeg1_11172) {
        return 0x01;
    }
    else if (val == VideoMpeg2_H262) {
        return 0x02;
    }
    else if (val == AudioMpeg1_11172) {
        return 0x03;
    }
    else if (val == AudioMpeg2_13818_3) {
        return 0x04;
    }
    else if (val == Mpeg2PrivateTableSections) {
        return 0x05;
    }
    else if (val == Mpeg2PesPrivateData) {
        return 0x06;
    }
    else if (val == MhegPackets) {
        return 0x07;
    }
    else if (val == Mpeg2AnnexA_DSMCC) {
        return 0x08;
    }
    else if (val == ITUTRecH2221) {
        return 0x09;
    }
    else if (val == ISO13818_6_typeA) {
        return 0x0A;
    }
    else if (val == ISO13818_6_typeB) {
        return 0x0B;
    }
    else if (val == ISO13818_6_typeC) {
        return 0x0C;
    }
    else if (val == ISO13818_6_typeD) {
        return 0x0D;
    }
    else if (val == Mpeg2ISO13818_1_Aux) {
        return 0x0E;
    }
    else if (val == AudioISO13818_7_ADTS) {
        return 0x0F;
    }
    else if (val == Mpeg4ISO14496_2) {
        return 0x10;
    }
    else if (val == AudioISO14496_3) {
        return 0x11;
    }
    else if (val == ISO14496_1_PesPackets) {
        return 0x12;
    }
    else if (val == ISO14496_1_Sections) {
        return 0x13;
    }
    else if (val == ISO13818_6_DownloadProt) {
        return 0x14;
    }
    else if (val == MetaDataPesPackets) {
        return 0x15;
    }
    else if (val == MetaDataSections) {
        return 0x16;
    }
    else if (val == Mpeg2UserPrivate) {
        return 0x80;
    }
    else if (val == AudioAtscAc3) {
        return 0x81;
    }
    else {
        return 0x82;
    }
}


std::string
Stream::streamTypeToString(Poco::UInt8 val)
{
    switch (val) {
        case 0x01:
            return VideoMpeg1_11172;
        case 0x02:
            return VideoMpeg2_H262;
        case 0x03:
            return AudioMpeg1_11172;
        case 0x04:
            return AudioMpeg2_13818_3;
        case 0x05:
            return Mpeg2PrivateTableSections;
        case 0x06:
            return Mpeg2PesPrivateData;
        case 0x07:
            return MhegPackets;
        case 0x08:
            return Mpeg2AnnexA_DSMCC;
        case 0x09:
            return ITUTRecH2221;
        case 0x0A:
            return ISO13818_6_typeA;
        case 0x0B:
            return ISO13818_6_typeB;
        case 0x0C:
            return ISO13818_6_typeC;
        case 0x0D:
            return ISO13818_6_typeD;
        case 0x0E:
            return Mpeg2ISO13818_1_Aux;
        case 0x0F:
            return AudioISO13818_7_ADTS;
        case 0x10:
            return Mpeg4ISO14496_2;
        case 0x11:
            return AudioISO14496_3;
        case 0x12:
            return ISO14496_1_PesPackets;
        case 0x13:
            return ISO14496_1_Sections;
        case 0x14:
            return ISO13818_6_DownloadProt;
        case 0x15:
            return MetaDataPesPackets;
        case 0x16:
            return MetaDataSections;
        case 0x80:
            return Mpeg2UserPrivate;
        case 0x81:
            return AudioAtscAc3;
        default:
            return Mpeg2UserPrivate;
    }
}


//Service::Service(const std::string& name, unsigned int vpid, unsigned int cpid, unsigned int apid, int sid, unsigned int tid, unsigned int pmtid) :
//_name(name),
//_vpid(vpid),
//_cpid(cpid),
//_apid(apid),
//_sid(sid),
//_tid(tid),
//_pmtid(pmtid)
//{
//}


Service::Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid) :
_pTransponder(pTransponder),
_name(name),
_sid(sid),
_pmtid(pmtid)
{
}


void
Service::addStream(Stream* pStream)
{
    _streams.push_back(pStream);
}


void
Service::readXml(Poco::XML::Node* pXmlService)
{
    LOG(dvb, debug, "read service ...");

    if (pXmlService->hasChildNodes()) {
        Poco::XML::Node* pXmlStream = pXmlService->firstChild();
        while (pXmlStream && pXmlStream->nodeName() == "stream") {
            std::string type = static_cast<Poco::XML::Element*>(pXmlStream)->getAttribute("type");
            int pid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlStream)->getAttribute("pid"));
            Stream* pStream = new Stream(type, pid);
            addStream(pStream);
            pXmlStream = pXmlStream->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb service description contains no stream");
        return;
    }

    LOG(dvb, debug, "read service.");
}


void
Service::writeXml(Poco::XML::Element* pTransponder)
{
    LOG(dvb, debug, "write service ...");

    Poco::XML::Document* pDoc = pTransponder->ownerDocument();
    Poco::AutoPtr<Poco::XML::Element> pService = pDoc->createElement("service");
    pTransponder->appendChild(pService);
    pService->setAttribute("name", _name);
    pService->setAttribute("sid", Poco::NumberFormatter::format(_sid));
    pService->setAttribute("pmtid", Poco::NumberFormatter::format(_pmtid));

    for (std::vector<Stream*>::iterator it = _streams.begin(); it != _streams.end(); ++it) {
        (*it)->writeXml(pService);
    }

    LOG(dvb, debug, "wrote service.");
}


Transponder::Transponder(Frontend* pFrontend, unsigned int freq, unsigned int tid) :
_pFrontend(pFrontend),
//Transponder::Transponder(unsigned int freq, unsigned int tid) :
_freq(freq),
_tid(tid)
{
}


void
Transponder::addService(Dvb::Service* pService)
{
    _services.push_back(pService);
}


Service*
Transponder::getService(unsigned int serviceId)
{
    std::vector<Dvb::Service*>::iterator it = _services.begin();
    while (it != _services.end() && (*it)->_sid != serviceId) {
        ++it;
    }
    if (it != _services.end()) {
        return *it;
    }
    else {
        return 0;
    }
}


Service*
Transponder::getService(const std::string& serviceName)
{
    std::vector<Dvb::Service*>::iterator it = _services.begin();
    while (it != _services.end() && (*it)->_name != serviceName) {
        ++it;
    }
    if (it != _services.end()) {
        return *it;
    }
    else {
        return 0;
    }
}


void
Transponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read transponder ...");

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlService = pXmlTransponder->firstChild();
        while (pXmlService && pXmlService->nodeName() == "service") {
            std::string name = static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("name");
            int sid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("sid"));
            unsigned int pmtid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("pmtid"));
            LOG(dvb, debug, "new service with name: " + name);

            Service* pService = new Service(this, name, sid, pmtid);
            pService->readXml(pXmlService);
            addService(pService);
            pXmlService = pXmlService->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb transponder description contains no services");
        return;
    }

    LOG(dvb, debug, "read transponder.");
}


void
Transponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write transponder ...");

    Poco::XML::Document* pDoc = pFrontend->ownerDocument();
    _pXmlTransponder = pDoc->createElement("transponder");
    pFrontend->appendChild(_pXmlTransponder);
    _pXmlTransponder->setAttribute("frequency", Poco::NumberFormatter::format(_freq));
    _pXmlTransponder->setAttribute("tid", Poco::NumberFormatter::format(_tid));

    for (std::vector<Service*>::iterator it = _services.begin(); it != _services.end(); ++it) {
        (*it)->writeXml(_pXmlTransponder);
    }

    LOG(dvb, debug, "wrote transponder.");
}

const std::string SatTransponder::POL_HORIZ("H");
const std::string SatTransponder::POL_VERT("V");


SatTransponder::SatTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid) :
Transponder(pFrontend, freq, tid)
//SatTransponder::SatTransponder(unsigned int freq, unsigned int tid) :
//Transponder(freq, tid)
{

}


void
SatTransponder::init(unsigned int satNum, unsigned int symbolRate, const std::string& polarization)
{
    LOG(dvb, debug, "init sat transponder.");

    _satNum = satNum;
    _symbolRate = symbolRate;
    _polarization = polarization;
}


void
SatTransponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read sat transponder ...");

    Transponder::readXml(pXmlTransponder);

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlTransponder->firstChild();
        while (pXmlParam) {
            Poco::XML::Node* pXmlParamVal = pXmlParam->firstChild();
            if (!pXmlParamVal) {
                LOG(dvb, error, "dvb sat transponder parameter without value: " + pXmlParam->nodeName());
                pXmlParam = pXmlParam->nextSibling();
                continue;
            }
            if (pXmlParam->nodeName() == "service") {
                // do nothing
            }
            else if (pXmlParam->nodeName() == "satNum") {
                _satNum = Poco::NumberParser::parse(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "symbolRate") {
                _symbolRate = Poco::NumberParser::parse(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "polarization") {
                _polarization = pXmlParamVal->innerText();
            }
            else {
                LOG(dvb, error, "dvb sat transponder unknown parameter: " + pXmlParam->nodeName());
            }

            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb sat transponder description contains no tuning parameters");
        return;
    }

    LOG(dvb, debug, "read sat transponder.");
}


void
SatTransponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write sat transponder ...");

    Transponder::writeXml(pFrontend);
    Poco::XML::Document* pDoc = pFrontend->ownerDocument();

    Poco::AutoPtr<Poco::XML::Element> pSatNum = pDoc->createElement("satNum");
    Poco::AutoPtr<Poco::XML::Text> pSatNumVal = pDoc->createTextNode(Poco::NumberFormatter::format(_satNum));
    pSatNum->appendChild(pSatNumVal);
    _pXmlTransponder->appendChild(pSatNum);

    Poco::AutoPtr<Poco::XML::Element> pSymbolRate = pDoc->createElement("symbolRate");
    Poco::AutoPtr<Poco::XML::Text> pSymbolRateVal = pDoc->createTextNode(Poco::NumberFormatter::format(_symbolRate));
    pSymbolRate->appendChild(pSymbolRateVal);
    _pXmlTransponder->appendChild(pSymbolRate);

    Poco::AutoPtr<Poco::XML::Element> pPolarization = pDoc->createElement("polarization");
    Poco::AutoPtr<Poco::XML::Text> pPolarizationVal = pDoc->createTextNode(_polarization);
    pPolarization->appendChild(pPolarizationVal);
    _pXmlTransponder->appendChild(pPolarization);

    LOG(dvb, debug, "wrote sat transponder.");
}


bool
SatTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "S" || params.count() < 4) {
        LOG(dvb, error, "invalid parameter data for sat transponder.");
        return false;
    }
    unsigned int symbolRate;
    try {
        symbolRate = Poco::NumberParser::parse(params[3]);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "invalid symbol rate for sat transponder: " + params[3]);
        return false;
    }
    init(0, symbolRate, params[2]);
    return true;
}


const std::string TerrestrialTransponder::BANDWIDTH_8_MHZ("8_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_7_MHZ("7_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_6_MHZ("6_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_AUTO("AUTO");
const std::string TerrestrialTransponder::BANDWIDTH_5_MHZ("5_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_10_MHZ("10_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_1_712_MHZ("1_712_MHZ");
const std::string TerrestrialTransponder::FEC_NONE("NONE");
const std::string TerrestrialTransponder::FEC_1_2("1/2");
const std::string TerrestrialTransponder::FEC_2_3("2/3");
const std::string TerrestrialTransponder::FEC_3_4("3/4");
const std::string TerrestrialTransponder::FEC_4_5("4/5");
const std::string TerrestrialTransponder::FEC_5_6("5/6");
const std::string TerrestrialTransponder::FEC_6_7("6/7");
const std::string TerrestrialTransponder::FEC_7_8("7/8");
const std::string TerrestrialTransponder::FEC_8_9("8/9");
const std::string TerrestrialTransponder::FEC_AUTO("AUTO");
const std::string TerrestrialTransponder::FEC_3_5("3/5");
const std::string TerrestrialTransponder::FEC_9_10("9/10");
const std::string TerrestrialTransponder::QPSK("QPSK");
const std::string TerrestrialTransponder::QAM_16("QAM16");
const std::string TerrestrialTransponder::QAM_32("QAM32");
const std::string TerrestrialTransponder::QAM_64("QAM64");
const std::string TerrestrialTransponder::QAM_128("QAM128");
const std::string TerrestrialTransponder::QAM_256("QAM256");
const std::string TerrestrialTransponder::QAM_AUTO("AUTO");
const std::string TerrestrialTransponder::VSB_8("8VSB");
const std::string TerrestrialTransponder::VSB_16("16VSB");
const std::string TerrestrialTransponder::PSK_8("8PSK");
const std::string TerrestrialTransponder::APSK_16("16APSK");
const std::string TerrestrialTransponder::APSK_32("32APSK");
const std::string TerrestrialTransponder::DQPSK("DQPSK");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_2K("2k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_8K("8k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_AUTO("AUTO");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_4K("4k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_1K("1k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_16K("16k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_32K("32k");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_32("1/32");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_16("1/16");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_8("1/8");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_4("1/4");
const std::string TerrestrialTransponder::GUARD_INTERVAL_AUTO("AUTO");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_128("1/128");
const std::string TerrestrialTransponder::GUARD_INTERVAL_19_128("19/128");
const std::string TerrestrialTransponder::GUARD_INTERVAL_19_256("19/256");
const std::string TerrestrialTransponder::HIERARCHY_NONE("NONE");
const std::string TerrestrialTransponder::HIERARCHY_1("1");
const std::string TerrestrialTransponder::HIERARCHY_2("2");
const std::string TerrestrialTransponder::HIERARCHY_4("4");
const std::string TerrestrialTransponder::HIERARCHY_AUTO("AUTO");


TerrestrialTransponder::TerrestrialTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid) :
Transponder(pFrontend, freq, tid)
//TerrestrialTransponder::TerrestrialTransponder(unsigned int freq, unsigned int tid) :
//Transponder(freq, tid)
{
}


void
TerrestrialTransponder::init(fe_bandwidth_t bandwidth,
            fe_code_rate_t code_rate_HP,
            fe_code_rate_t code_rate_LP,
            fe_modulation_t constellation,
            fe_transmit_mode_t transmission_mode,
            fe_guard_interval_t guard_interval,
            fe_hierarchy_t hierarchy_information)
{
    LOG(dvb, debug, "init terrestrial transponder: " +
            Poco::NumberFormatter::format(bandwidth) + ", " +
            Poco::NumberFormatter::format(code_rate_HP) + ", " +
            Poco::NumberFormatter::format(code_rate_LP) + ", " +
            Poco::NumberFormatter::format(constellation) + ", " +
            Poco::NumberFormatter::format(transmission_mode) + ", " +
            Poco::NumberFormatter::format(guard_interval) + ", " +
            Poco::NumberFormatter::format(hierarchy_information));

    _bandwidth = bandwidth;
    _code_rate_HP = code_rate_HP;
    _code_rate_LP = code_rate_LP;
    _constellation = constellation;
    _transmission_mode = transmission_mode;
    _guard_interval = guard_interval;
    _hierarchy_information = hierarchy_information;
}


void
TerrestrialTransponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read terrestrial transponder ...");

    Transponder::readXml(pXmlTransponder);

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlTransponder->firstChild();
        while (pXmlParam) {
            Poco::XML::Node* pXmlParamVal = pXmlParam->firstChild();
            if (!pXmlParamVal) {
                LOG(dvb, error, "dvb terrestrial transponder parameter without value: " + pXmlParam->nodeName());
                pXmlParam = pXmlParam->nextSibling();
                continue;
            }
            if (pXmlParam->nodeName() == "service") {
                // do nothing
            }
            else if (pXmlParam->nodeName() == "bandwidth") {
                _bandwidth = bandwidthFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "codeRateHP") {
                _code_rate_HP = coderateFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "codeRateLP") {
                _code_rate_LP = coderateFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "constellation") {
                _constellation = modulationFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "transmissionMode") {
                _transmission_mode = transmitModeFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "guardInterval") {
                _guard_interval = guard_intervalFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "hierarchyInformation") {
                _hierarchy_information = hierarchyFromString(pXmlParamVal->innerText());
            }
            else {
                LOG(dvb, error, "dvb terrestrial transponder unknown parameter: " + pXmlParam->nodeName());
            }

            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb terrestrial transponder description contains no tuning parameters");
        return;
    }

    LOG(dvb, debug, "read terrestrial transponder.");
}


void
TerrestrialTransponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write terrestrial transponder ...");

    Transponder::writeXml(pFrontend);
    Poco::XML::Document* pDoc = pFrontend->ownerDocument();

    Poco::AutoPtr<Poco::XML::Element> pBandwidth = pDoc->createElement("bandwidth");
    Poco::AutoPtr<Poco::XML::Text> pBandwidthVal = pDoc->createTextNode(bandwidthToString(_bandwidth));
    pBandwidth->appendChild(pBandwidthVal);
    _pXmlTransponder->appendChild(pBandwidth);

    Poco::AutoPtr<Poco::XML::Element> pCodeRateHP = pDoc->createElement("codeRateHP");
    Poco::AutoPtr<Poco::XML::Text> pCodeRateHPVal = pDoc->createTextNode(coderateToString(_code_rate_HP));
    pCodeRateHP->appendChild(pCodeRateHPVal);
    _pXmlTransponder->appendChild(pCodeRateHP);

    Poco::AutoPtr<Poco::XML::Element> pCodeRateLP = pDoc->createElement("codeRateLP");
    Poco::AutoPtr<Poco::XML::Text> pCodeRateLPVal = pDoc->createTextNode(coderateToString(_code_rate_LP));
    pCodeRateLP->appendChild(pCodeRateLPVal);
    _pXmlTransponder->appendChild(pCodeRateLP);

    Poco::AutoPtr<Poco::XML::Element> pConstellation = pDoc->createElement("constellation");
    Poco::AutoPtr<Poco::XML::Text> pConstellationVal = pDoc->createTextNode(modulationToString(_constellation));
    pConstellation->appendChild(pConstellationVal);
    _pXmlTransponder->appendChild(pConstellation);

    Poco::AutoPtr<Poco::XML::Element> pTransmissionMode = pDoc->createElement("transmissionMode");
    Poco::AutoPtr<Poco::XML::Text> pTransmissionModeVal = pDoc->createTextNode(transmitModeToString(_transmission_mode));
    pTransmissionMode->appendChild(pTransmissionModeVal);
    _pXmlTransponder->appendChild(pTransmissionMode);

    Poco::AutoPtr<Poco::XML::Element> pGuardInterval = pDoc->createElement("guardInterval");
    Poco::AutoPtr<Poco::XML::Text> pGuardIntervalVal = pDoc->createTextNode(guard_intervalToString(_guard_interval));
    pGuardInterval->appendChild(pGuardIntervalVal);
    _pXmlTransponder->appendChild(pGuardInterval);

    Poco::AutoPtr<Poco::XML::Element> pHierarchyInformation = pDoc->createElement("hierarchyInformation");
    Poco::AutoPtr<Poco::XML::Text> pHierarchyInformationVal = pDoc->createTextNode(hierarchyToString(_hierarchy_information));
    pHierarchyInformation->appendChild(pHierarchyInformationVal);
    _pXmlTransponder->appendChild(pHierarchyInformation);

    LOG(dvb, debug, "wrote terrestrial transponder.");
}


bool
TerrestrialTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "T" || params.count() < 9) {
        LOG(dvb, error, "invalid parameter data for terrestrial transponder.");
        return false;
    }
    init(bandwidthFromString(params[2]),
        coderateFromString(params[3]),
        coderateFromString(params[4]),
        modulationFromString(params[5]),
        transmitModeFromString(params[6]),
        guard_intervalFromString(params[7]),
        hierarchyFromString(params[8]));
    return true;
}


fe_bandwidth_t
TerrestrialTransponder::bandwidthFromString(const std::string& val)
{
    if (val == BANDWIDTH_8_MHZ) {
        return ::BANDWIDTH_8_MHZ;
    }
    else if (val == BANDWIDTH_7_MHZ) {
        return ::BANDWIDTH_7_MHZ;
    }
    else if (val == BANDWIDTH_6_MHZ) {
        return ::BANDWIDTH_6_MHZ;
    }
    else if (val == BANDWIDTH_AUTO) {
        return ::BANDWIDTH_AUTO;
    }
    else if (val == BANDWIDTH_5_MHZ) {
        return ::BANDWIDTH_5_MHZ;
    }
    else if (val == BANDWIDTH_10_MHZ) {
        return ::BANDWIDTH_10_MHZ;
    }
    else if (val == BANDWIDTH_1_712_MHZ) {
        return ::BANDWIDTH_1_712_MHZ;
    }
}


std::string
TerrestrialTransponder::bandwidthToString(fe_bandwidth_t val)
{
    if (val == ::BANDWIDTH_8_MHZ) {
        return BANDWIDTH_8_MHZ;
    }
    else if (val == ::BANDWIDTH_7_MHZ) {
        return BANDWIDTH_7_MHZ;
    }
    else if (val == ::BANDWIDTH_6_MHZ) {
        return BANDWIDTH_6_MHZ;
    }
    else if (val == ::BANDWIDTH_AUTO) {
        return BANDWIDTH_AUTO;
    }
    else if (val == ::BANDWIDTH_5_MHZ) {
        return BANDWIDTH_5_MHZ;
    }
    else if (val == ::BANDWIDTH_10_MHZ) {
        return BANDWIDTH_10_MHZ;
    }
    else if (val == ::BANDWIDTH_1_712_MHZ) {
        return BANDWIDTH_1_712_MHZ;
    }
}


fe_code_rate_t
TerrestrialTransponder::coderateFromString(const std::string& val)
{
    if (val == FEC_NONE) {
        return ::FEC_NONE;
    }
    else if (val == FEC_1_2) {
        return ::FEC_1_2;
    }
    else if (val == FEC_2_3) {
        return ::FEC_2_3;
    }
    else if (val == FEC_3_4) {
        return ::FEC_3_4;
    }
    else if (val == FEC_4_5) {
        return ::FEC_4_5;
    }
    else if (val == FEC_5_6) {
        return ::FEC_5_6;
    }
    else if (val == FEC_6_7) {
        return ::FEC_6_7;
    }
    else if (val == FEC_7_8) {
        return ::FEC_7_8;
    }
    else if (val == FEC_8_9) {
        return ::FEC_8_9;
    }
    else if (val == FEC_AUTO) {
        return ::FEC_AUTO;
    }
    else if (val == FEC_3_5) {
        return ::FEC_3_5;
    }
    else if (val == FEC_9_10) {
        return ::FEC_9_10;
    }
}


std::string
TerrestrialTransponder::coderateToString(fe_code_rate_t val)
{
    if (val == ::FEC_NONE) {
        return FEC_NONE;
    }
    else if (val == ::FEC_1_2) {
        return FEC_1_2;
    }
    else if (val == ::FEC_2_3) {
        return FEC_2_3;
    }
    else if (val == ::FEC_3_4) {
        return FEC_3_4;
    }
    else if (val == ::FEC_4_5) {
        return FEC_4_5;
    }
    else if (val == ::FEC_5_6) {
        return FEC_5_6;
    }
    else if (val == ::FEC_6_7) {
        return FEC_6_7;
    }
    else if (val == ::FEC_7_8) {
        return FEC_7_8;
    }
    else if (val == ::FEC_8_9) {
        return FEC_8_9;
    }
    else if (val == ::FEC_AUTO) {
        return FEC_AUTO;
    }
    else if (val == ::FEC_3_5) {
        return FEC_3_5;
    }
    else if (val == ::FEC_9_10) {
        return FEC_9_10;
    }
}


fe_modulation_t
TerrestrialTransponder::modulationFromString(const std::string& val)
{
    if (val == QPSK) {
        return ::QPSK;
    }
    else if (val == QAM_16) {
        return ::QAM_16;
    }
    else if (val == QAM_32) {
        return ::QAM_32;
    }
    else if (val == QAM_64) {
        return ::QAM_64;
    }
    else if (val == QAM_128) {
        return ::QAM_128;
    }
    else if (val == QAM_256) {
        return ::QAM_256;
    }
    else if (val == QAM_AUTO) {
        return ::QAM_AUTO;
    }
    else if (val == VSB_8) {
        return ::VSB_8;
    }
    else if (val == VSB_16) {
        return ::VSB_16;
    }
    else if (val == PSK_8) {
        return ::PSK_8;
    }
    else if (val == APSK_16) {
        return ::APSK_16;
    }
    else if (val == APSK_32) {
        return ::APSK_32;
    }
    else if (val == DQPSK) {
        return ::DQPSK;
    }
}


std::string
TerrestrialTransponder::modulationToString(fe_modulation_t val)
{
    if (val == ::QPSK) {
        return QPSK;
    }
    else if (val == ::QAM_16) {
        return QAM_16;
    }
    if (val == ::QAM_32) {
        return QAM_32;
    }
    else if (val == ::QAM_64) {
        return QAM_64;
    }
    if (val == ::QAM_128) {
        return QAM_128;
    }
    else if (val == ::QAM_256) {
        return QAM_256;
    }
    if (val == ::QAM_AUTO) {
        return QAM_AUTO;
    }
    else if (val == ::VSB_8) {
        return VSB_8;
    }
    if (val == ::VSB_16) {
        return VSB_16;
    }
    else if (val == ::PSK_8) {
        return PSK_8;
    }
    if (val == ::APSK_16) {
        return APSK_16;
    }
    else if (val == ::APSK_32) {
        return APSK_32;
    }
    if (val == ::DQPSK) {
        return DQPSK;
    }
}


fe_transmit_mode_t
TerrestrialTransponder::transmitModeFromString(const std::string& val)
{
    if (val == TRANSMISSION_MODE_2K) {
        return ::TRANSMISSION_MODE_2K;
    }
    else if (val == TRANSMISSION_MODE_8K) {
        return ::TRANSMISSION_MODE_8K;
    }
    else if (val == TRANSMISSION_MODE_AUTO) {
        return ::TRANSMISSION_MODE_AUTO;
    }
    else if (val == TRANSMISSION_MODE_4K) {
        return ::TRANSMISSION_MODE_4K;
    }
    else if (val == TRANSMISSION_MODE_1K) {
        return ::TRANSMISSION_MODE_1K;
    }
    else if (val == TRANSMISSION_MODE_16K) {
        return ::TRANSMISSION_MODE_16K;
    }
    else if (val == TRANSMISSION_MODE_32K) {
        return ::TRANSMISSION_MODE_32K;
    }
}


std::string
TerrestrialTransponder::transmitModeToString(fe_transmit_mode_t val)
{
    if (val == ::TRANSMISSION_MODE_2K) {
        return TRANSMISSION_MODE_2K;
    }
    else if (val == ::TRANSMISSION_MODE_8K) {
        return TRANSMISSION_MODE_8K;
    }
    else if (val == ::TRANSMISSION_MODE_AUTO) {
        return TRANSMISSION_MODE_AUTO;
    }
    else if (val == ::TRANSMISSION_MODE_4K) {
        return TRANSMISSION_MODE_4K;
    }
    else if (val == ::TRANSMISSION_MODE_1K) {
        return TRANSMISSION_MODE_1K;
    }
    else if (val == ::TRANSMISSION_MODE_16K) {
        return TRANSMISSION_MODE_16K;
    }
    else if (val == ::TRANSMISSION_MODE_32K) {
        return TRANSMISSION_MODE_32K;
    }
}


fe_guard_interval_t
TerrestrialTransponder::guard_intervalFromString(const std::string& val)
{
    if (val == GUARD_INTERVAL_1_32) {
        return ::GUARD_INTERVAL_1_32;
    }
    else if (val == GUARD_INTERVAL_1_16) {
        return ::GUARD_INTERVAL_1_16;
    }
    else if (val == GUARD_INTERVAL_1_8) {
        return ::GUARD_INTERVAL_1_8;
    }
    else if (val == GUARD_INTERVAL_1_4) {
        return ::GUARD_INTERVAL_1_4;
    }
    else if (val == GUARD_INTERVAL_AUTO) {
        return ::GUARD_INTERVAL_AUTO;
    }
    else if (val == GUARD_INTERVAL_1_128) {
        return ::GUARD_INTERVAL_1_128;
    }
    else if (val == GUARD_INTERVAL_19_128) {
        return ::GUARD_INTERVAL_19_128;
    }
    else if (val == GUARD_INTERVAL_19_256) {
        return ::GUARD_INTERVAL_19_256;
    }

}


std::string
TerrestrialTransponder::guard_intervalToString(fe_guard_interval_t val)
{
    if (val == ::GUARD_INTERVAL_1_32) {
        return GUARD_INTERVAL_1_32;
    }
    else if (val == ::GUARD_INTERVAL_1_16) {
        return GUARD_INTERVAL_1_16;
    }
    else if (val == ::GUARD_INTERVAL_1_8) {
        return GUARD_INTERVAL_1_8;
    }
    else if (val == ::GUARD_INTERVAL_1_4) {
        return GUARD_INTERVAL_1_4;
    }
    else if (val == ::GUARD_INTERVAL_AUTO) {
        return GUARD_INTERVAL_AUTO;
    }
    else if (val == ::GUARD_INTERVAL_1_128) {
        return GUARD_INTERVAL_1_128;
    }
    else if (val == ::GUARD_INTERVAL_19_128) {
        return GUARD_INTERVAL_19_128;
    }
    else if (val == ::GUARD_INTERVAL_19_256) {
        return GUARD_INTERVAL_19_256;
    }
}


fe_hierarchy_t
TerrestrialTransponder::hierarchyFromString(const std::string& val)
{
    if (val == HIERARCHY_NONE) {
        return ::HIERARCHY_NONE;
    }
    else if (val == HIERARCHY_1) {
        return ::HIERARCHY_1;
    }
    else if (val == HIERARCHY_2) {
        return ::HIERARCHY_2;
    }
    else if (val == HIERARCHY_4) {
        return ::HIERARCHY_4;
    }
    else if (val == HIERARCHY_AUTO) {
        return ::HIERARCHY_AUTO;
    }
}


std::string
TerrestrialTransponder::hierarchyToString(fe_hierarchy_t val)
{
    if (val == ::HIERARCHY_NONE) {
        return HIERARCHY_NONE;
    }
    else if (val == ::HIERARCHY_1) {
        return HIERARCHY_1;
    }
    else if (val == ::HIERARCHY_2) {
        return HIERARCHY_2;
    }
    else if (val == ::HIERARCHY_4) {
        return HIERARCHY_4;
    }
    else if (val == ::HIERARCHY_AUTO) {
        return HIERARCHY_AUTO;
    }
}


CableTransponder::CableTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid) :
Transponder(pFrontend, freq, tid)
//CableTransponder::CableTransponder(unsigned int freq, unsigned int tid) :
//Transponder(freq, tid)
{

}


bool
CableTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "C") {
        LOG(dvb, error, "invalid parameter data for cable transponder.");
        return false;
    }
    return true;
}


AtscTransponder::AtscTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tid) :
Transponder(pFrontend, freq, tid)
//AtscTransponder::AtscTransponder(unsigned int freq, unsigned int tid) :
//Transponder(freq, tid)
{

}


bool
AtscTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "A") {
        LOG(dvb, error, "invalid parameter data for atsc transponder.");
        return false;
    }
    return true;
}


class SignalCheckThread : public Poco::Runnable
{
public:
    SignalCheckThread(Frontend* pFrontend);

    void run();
    void stop();

private:
    Frontend*        _pFrontend;
    bool                _stop;
};


SignalCheckThread::SignalCheckThread(Frontend* pFrontend) :
_pFrontend(pFrontend),
_stop(false)
{
}


void
SignalCheckThread::run()
{
//     Poco::ScopedLock<Poco::FastMutex> lock(DvbDevice::instance()->_tuneLock);
    do {
        _pFrontend->checkFrontend();
        Poco::Thread::sleep(1000);
    } while(!_stop);
}


void
SignalCheckThread::stop()
{
    _stop = true;
}


const std::string Frontend::Unknown("unknown");
const std::string Frontend::DVBS("DVB-S");
const std::string Frontend::DVBT("DVB-T");
const std::string Frontend::DVBC("DVB-C");
const std::string Frontend::ATSC("ATSC");

Frontend::Frontend(Adapter* pAdapter, int num) :
_fileDescFrontend(-1),
_pAdapter(pAdapter),
_num(num),
//_frontendTimeout(2000000)
_frontendTimeout(1000000)
{
    _deviceName = _pAdapter->_deviceName + "/frontend" + Poco::NumberFormatter::format(_num);
    _pDemux = new Demux(pAdapter, 0);
    if (Device::instance()->useDvrDevice()) {
        _pDvr = new Dvr(pAdapter, 0);
    }
    _pTextConverter = new Poco::TextConverter(_sourceEncoding, _targetEncoding);
}


Frontend::~Frontend()
{
    closeFrontend();
    delete _pDemux;
    if (Device::instance()->useDvrDevice()) {
        delete _pDvr;
    }
}


void
Frontend::addTransponder(Transponder* pTransponder)
{
    _transponders.push_back(pTransponder);
}


void
Frontend::openFrontend()
{
    LOG(dvb, debug, "opening frontend");

    if ((_fileDescFrontend = open(_deviceName.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "opening frontend failed: " + std::string(strerror(errno)));
    }

    int result = ioctl(_fileDescFrontend, FE_GET_INFO, &_feInfo);

    if (result < 0) {
        LOG(dvb, error, "ioctl FE_GET_INFO failed");
        closeFrontend();
    }

    switch (_feInfo.type) {
        case FE_QPSK:
            _type = DVBS;
            break;
        case FE_OFDM:
            _type = DVBT;
            break;
        case FE_QAM:
            _type = DVBC;
            break;
        case FE_ATSC:
            _type = ATSC;
            break;
        default:
            _type = Unknown;
    }

    if (!typeSupported()) {
        LOG(dvb, error, "frontend device is not a DVB-S or DVB-T device, not yet supported");
        closeFrontend();
    }

//    _pDemux->openDemux(false);
    if (Device::instance()->useDvrDevice()) {
        _pDvr->openDvr(Device::instance()->blockDvrDevice());
    }
}


void
Frontend::closeFrontend()
{
    LOG(dvb, debug, "closing frontend");

    if (Device::instance()->useDvrDevice()) {
        _pDvr->closeDvr();
    }
//    _pDemux->closeDemux();

    if (close(_fileDescFrontend)) {
        LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
    }
}


void
Frontend::scan(const std::string& initialTransponderData)
{
    getInitialTransponderData(initialTransponderData);
    LOG(dvb, debug, "number of initial transponders: " + Poco::NumberFormatter::format(_initialTransponders.size()));
    openFrontend();
    for (std::vector<Transponder*>::iterator it = _initialTransponders.begin(); it != _initialTransponders.end(); ++it) {
//    std::vector<Transponder*>::iterator it = _initialTransponders.begin() + 2;
        if (tune(*it)) {
            _transponders.push_back(*it);

            Poco::UInt16 patPid = 0x00;
            Stream* pPat = new Stream(Stream::Other, patPid);
            _pDemux->selectStream(pPat, Demux::TargetDemux, true);
            Poco::UInt8 patTableId = 0x00;
            _pDemux->setSectionFilter(pPat, patTableId);
            _pDemux->runStream(pPat, true);
            Section patTable(patTableId);
            patTable.read(pPat);
            _pDemux->runStream(pPat, false);

            LOG(dvb, debug, "pat table length: " + Poco::NumberFormatter::format(patTable.length()));
            Poco::UInt16 transportStreamId = patTable.getValue<Poco::UInt16>(24, 16);
            LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transportStreamId));
            (*it)->_tid = transportStreamId;

            int serviceCount = (patTable.length() - 8 - 4) / 4;  // section header size = 8,  crc = 4, service section size = 4
            unsigned int headerSize = 8 * 8;
            unsigned int serviceSize = 4 * 8;
            while (serviceCount--) {
                LOG(dvb, trace, "------- SERVICE -------");
                Poco::UInt16 serviceId = patTable.getValue<Poco::UInt16>(headerSize + serviceCount * serviceSize, 16);
                LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(serviceId));

                Poco::UInt16 pmtPid = patTable.getValue<Poco::UInt16>(headerSize + serviceCount * serviceSize + 19, 13);
                LOG(dvb, trace, "pmt pid: " + Poco::NumberFormatter::format(pmtPid));

                if (serviceId) {  // no NIT service
                    Service* pService = new Dvb::Service(*it, "", serviceId, pmtPid);
                    (*it)->addService(pService);

                    Stream* pPmt = new Stream(Stream::ProgramMapTable, pmtPid);
                    _pDemux->selectStream(pPmt, Demux::TargetDemux, true);
                    Poco::UInt8 pmtTableId = 0x02;
                    _pDemux->setSectionFilter(pPmt, pmtTableId);
                    _pDemux->runStream(pPmt, true);
                    Section pmtTable(pmtTableId);
                    pmtTable.read(pPmt);
                    _pDemux->runStream(pPmt, false);
                    LOG(dvb, debug, "pmt table length: " + Poco::NumberFormatter::format(pmtTable.length()));
                    Poco::UInt16 programInfoLength = pmtTable.getValue<Poco::UInt16>(84, 12);
                    LOG(dvb, trace, "program info length: " + Poco::NumberFormatter::format(programInfoLength));
                    unsigned int headerSize = 96 + programInfoLength * 8;
                    unsigned int totalStreamSectionSize = pmtTable.length() * 8 - headerSize - 4 * 8;
                    unsigned int offset = 0;
                    while (offset < totalStreamSectionSize) {
                        Poco::UInt8 streamType = pmtTable.getValue<Poco::UInt8>(headerSize + offset, 8);
                        LOG(dvb, trace, "stream type: " + Poco::NumberFormatter::format(streamType));
                        Poco::UInt16 streamPid = pmtTable.getValue<Poco::UInt16>(headerSize + offset + 11, 13);
                        LOG(dvb, trace, "stream pid: " + Poco::NumberFormatter::format(streamPid));
                        Poco::UInt16 esInfoLength = pmtTable.getValue<Poco::UInt16>(headerSize + offset + 28, 12);
                        LOG(dvb, trace, "es info length: " + Poco::NumberFormatter::format(esInfoLength));
                        offset += 40 + esInfoLength * 8;

                        Stream* pStream = new Stream(Stream::streamTypeToString(streamType), streamPid);
                        pService->addStream(pStream);
                    }
                    pService->addStream(pPmt);
                }
            }

            Poco::UInt16 sdtPid = 0x11;
            Stream* pSdt = new Stream(Stream::Other, sdtPid);
            _pDemux->selectStream(pSdt, Demux::TargetDemux, true);
            Poco::UInt8 sdtTableId = 0x42;
            _pDemux->setSectionFilter(pSdt, sdtTableId);
            _pDemux->runStream(pSdt, true);
            Section sdtTable(sdtTableId);
            sdtTable.read(pSdt);
            _pDemux->runStream(pSdt, false);
            unsigned int sdtHeaderSize = 88;
            unsigned int totalSdtSectionSize = sdtTable.length() * 8 - sdtHeaderSize - 4 * 8;
            unsigned int sdtOffset = 0;
            while (sdtOffset < totalSdtSectionSize) {
                LOG(dvb, trace, "------- SERVICE -------");
                Poco::UInt16 sdtServiceId = sdtTable.getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset, 16);
                LOG(dvb, trace, "std service id: " + Poco::NumberFormatter::format(sdtServiceId));
                Poco::UInt8 runningStatus = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 24, 3);
                LOG(dvb, trace, "std running status: " + Poco::NumberFormatter::format(runningStatus));
                Poco::UInt8 scrambled = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 27, 1);
                LOG(dvb, trace, "std scrambled: " + Poco::NumberFormatter::format(scrambled));
                Poco::UInt16 sdtInfoLength = sdtTable.getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset + 28, 12);
                LOG(dvb, trace, "sdt descriptor total length: " + Poco::NumberFormatter::format(sdtInfoLength));
                Poco::UInt8 descId = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 40, 8);
                LOG(dvb, trace, "std descriptor tag: " + Poco::NumberFormatter::format(descId));
                Poco::UInt8 descLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 48, 8);
                LOG(dvb, trace, "std descriptor length: " + Poco::NumberFormatter::format(descLength));
                Poco::UInt8 descType = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 56, 8);
                LOG(dvb, trace, "std descriptor type: " + Poco::NumberFormatter::format(descType));
                Poco::UInt8 descProvNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 64, 8);
                LOG(dvb, trace, "std descriptor provider name length: " + Poco::NumberFormatter::format(descProvNameLength));
                std::string providerName((char*)(sdtTable.data()) + (sdtHeaderSize + sdtOffset + 72) / 8, descProvNameLength);
                LOG(dvb, trace, "std provider name: " + providerName);
                Poco::UInt8 descServiceNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 72 + descProvNameLength * 8, 8);
                LOG(dvb, trace, "std descriptor service name length: " + Poco::NumberFormatter::format(descServiceNameLength));
                std::string serviceName((char*)(sdtTable.data()) + (sdtHeaderSize + sdtOffset + 80 + descProvNameLength * 8) / 8, descServiceNameLength);
                LOG(dvb, trace, "std service name: " + serviceName);

                Service* pService = (*it)->getService(sdtServiceId);
                if (pService) {
                    pService->_name = trim(serviceName);
                }

                sdtOffset += 40 + sdtInfoLength * 8;
            }
        }
    }

    closeFrontend();
}


void
Frontend::readXml(Poco::XML::Node* pXmlFrontend)
{
    LOG(dvb, debug, "read frontend ...");

    _name = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("name");
    _type = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("type");

    if (pXmlFrontend->hasChildNodes()) {
        Poco::XML::Node* pXmlTransponder = pXmlFrontend->firstChild();
        while (pXmlTransponder && pXmlTransponder->nodeName() == "transponder") {
            unsigned int freq = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlTransponder)->getAttribute("frequency"));
            unsigned int tid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlTransponder)->getAttribute("tid"));
            LOG(dvb, debug, "new transponder with freq: " + Poco::NumberFormatter::format(freq));

            Transponder* pTransponder;
            if (_type == DVBS) {
                pTransponder = new SatTransponder(this, freq, tid);
            }
            else if (_type == DVBT) {
                pTransponder = new TerrestrialTransponder(this, freq, tid);
            }
            else if (_type == DVBC) {
                pTransponder = new CableTransponder(this, freq, tid);
            }
            else if (_type == ATSC) {
                pTransponder = new AtscTransponder(this, freq, tid);
            }
            else {
                LOG(dvb, error, "dvb frontend type unknown, cannot create transponders");
                return;
            }

            pTransponder->readXml(pXmlTransponder);
            addTransponder(pTransponder);
            pXmlTransponder = pXmlTransponder->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no transponders");
        return;
    }

    LOG(dvb, debug, "read frontend.");
}


void
Frontend::writeXml(Poco::XML::Element* pAdapter)
{
    LOG(dvb, debug, "write frontend ...");

    Poco::XML::Document* pDoc = pAdapter->ownerDocument();
    Poco::XML::Element* pFrontend = pDoc->createElement("frontend");
    pFrontend->setAttribute("type", getType());
    pFrontend->setAttribute("name", getName());
    pAdapter->appendChild(pFrontend);
    for (std::vector<Transponder*>::iterator it = _transponders.begin(); it != _transponders.end(); ++it) {
        (*it)->writeXml(pFrontend);
    }

    LOG(dvb, debug, "wrote frontend.");
}


const std::string
Frontend::getType()
{
//    LOG(dvb, debug, "frontend type: " + Poco::NumberFormatter::format(_feInfo.type));

    return _type;

    // FIXME: dvb-s and dvb-t seam to be swapped on my systems ...?
//    switch (_feInfo.type) {
//        case FE_QPSK:
////            return DVBS;
//            return DVBT;
//        case FE_OFDM:
//            return DVBS;
////            return DVBT;
//        case FE_QAM:
//            return DVBC;
//        case FE_ATSC:
//            return ATSC;
//        default:
//            return "";
//    }
}


const std::string
Frontend::getName()
{
    return _name;
//    return _feInfo.name;
}


bool
Frontend::typeSupported()
{
    return getType() == DVBS || getType() == DVBT;
}


void
Frontend::listInitialTransponderData()
{
    LOG(dvb, debug, "compressed transponder data size: " + Poco::NumberFormatter::format(TransponderData::instance()->getResource("transponder.zip").size()));
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"));
    Poco::Zip::ZipArchive arch(ss);
    for (Poco::Zip::ZipArchive::FileInfos::const_iterator it = arch.fileInfoBegin(); it != arch.fileInfoEnd(); ++it) {
        LOG(dvb, debug, it->second.getFileName());
    }
}


void
Frontend::getInitialTransponderKeys(const std::string& countryCode, std::vector<std::string>& keys)
{

}


void
Frontend::getInitialTransponderData(const std::string& key)
{
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"), std::ios::binary);
    Poco::Zip::ZipArchive arch(ss);
    Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.findHeader(key);
    if (it == arch.headerEnd()) {
        LOG(dvb, error, "transponder data not found for: " + key);
        return;
    }
    Poco::Zip::ZipInputStream zipin(ss, it->second);
    std::string line;
    while (getline(zipin, line)) {
        std::string::size_type pos = line.find('#');
        line = line.substr(0, pos);
        if (line.length() == 0) {
            continue;
        }
        LOG(dvb, debug, line);

        Poco::StringTokenizer params(line, " ");
        unsigned int freq = 0;
        try {
            freq = Poco::NumberParser::parse(params[1]);
        }
        catch (Poco::Exception& e) {
            LOG(dvb, error, "transponder data without frequency: " + line);
            continue;
        }
        Transponder* pTransponder = createTransponder(freq, 0);
        if (pTransponder->initTransponder(params)) {
            _initialTransponders.push_back(pTransponder);
        }
        else {
            LOG(dvb, error, "transponder initialization failed: " + line);
        }
    }
}


//bool
//Frontend::tune(Channel* pChannel)
//{
//    LOG(dvb, debug, "frontend tuning to channel");
//
//    bool success = false;
//    unsigned int ifreq;
//    if (getType() == DVBS) {
//        bool hiBand = _pAdapter->_pLnb->isHiBand(pChannel->_freq, ifreq);
//        diseqc(pChannel->_satNum, pChannel->_pol, hiBand);
//    }
//    else if (getType() == DVBT) {
//        ifreq = pChannel->_freq;
//    }
//    else {
//        LOG(dvb, error, "frontend type not yet supported, stop tuning.");
//        return false;
//    }
//
//    if (tuneFrontend(ifreq, pChannel->_symbolRate)) {
//        if (_pAdapter->_pDemux->setVideoPid(pChannel->_vpid) &&
//            _pAdapter->_pDemux->setAudioPid(pChannel->_apid) &&
//            _pAdapter->_pDemux->setPcrPid(pChannel->_cpid)) {
//                success = true;
//                if (_pAdapter->_recPsi) {
//                    unsigned int pmtPid = pChannel->_pmtid;
//                    if (pmtPid == 0) {
//                        pmtPid = _pAdapter->_pDemux->getPmtPid(pChannel->_tid, pChannel->_sid);
//                    }
//                    LOG(dvb, debug, "pmt pid: " + Poco::NumberFormatter::format(pmtPid));
//                    if (pmtPid == 0) {
//                        LOG(dvb, error, "couldn't find pmt-pid for sid");
//                        success = false;
//                    }
//                    else if (!_pAdapter->_pDemux->setPatPid(0) ||
//                             !_pAdapter->_pDemux->setPmtPid(pmtPid))
//                    {
//                        success = false;
//                    }
//                }
//        }
//    }
//
//    LOG(dvb, debug, "frontend tuning " + std::string(success ? "success." : "failed."));
//    return success;
//}


//void
//Frontend::stopTune()
//{
//    _pt->stop();
//    delete _pt;
//}


//bool
//Frontend::tuneFrontend(unsigned int freq, unsigned int symbolRate)
//{
//    LOG(dvb, debug, "tune frontend to frequency: " + Poco::NumberFormatter::format(freq) + " ...");
//
//    struct dvb_frontend_parameters tuneto;
//    struct dvb_frontend_event event;
//
//    // discard stale QPSK events
//    while (1) {
//        if (ioctl(_fileDescFrontend, FE_GET_EVENT, &event) == -1)
//            break;
//    }
//
//    tuneto.frequency = freq;
//    tuneto.inversion = INVERSION_AUTO;
//    if (getType() == DVBS) {
//        tuneto.u.qpsk.symbol_rate = symbolRate;
//        tuneto.u.qpsk.fec_inner = FEC_AUTO;
//    }
//    else if (getType() == DVBT) {
//        tuneto.u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
//        tuneto.u.ofdm.code_rate_HP = FEC_2_3;
//        tuneto.u.ofdm.code_rate_LP = FEC_NONE;
//        tuneto.u.ofdm.constellation = QAM_16;
//        tuneto.u.ofdm.transmission_mode = TRANSMISSION_MODE_8K;
//        tuneto.u.ofdm.guard_interval = GUARD_INTERVAL_1_4;
//        tuneto.u.ofdm.hierarchy_information = HIERARCHY_NONE;
//    }
//
//    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
//        LOG(dvb, error, "FE_SET_FRONTEND failed");
//        return false;
//    }
//
//    LOG(dvb, debug, "tune frontend to frequency finished.");
//
//    return true;
//}


void
Frontend::checkFrontend()
{
    fe_status_t status;
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;
//     int timeout = 0;

    if (ioctl(_fileDescFrontend, FE_READ_STATUS, &status) == -1) {
        LOG(dvb, error, "FE_READ_STATUS failed");
    }
    /* some frontends might not support all these ioctls, thus we
    * avoid printing errors */
    if (ioctl(_fileDescFrontend, FE_READ_SIGNAL_STRENGTH, &signal) == -1) {
        signal = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_SNR, &snr) == -1) {
        snr = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_BER, &ber) == -1) {
        ber = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks) == -1) {
        uncorrected_blocks = -2;
    }

    /*
    if (human_readable) {
        printf ("status %02x | signal %3u%% | snr %3u%% | ber %d | unc %d | ",
                status, (signal * 100) / 0xffff, (snr * 100) / 0xffff, ber, uncorrected_blocks);
    } else {
        printf ("status %02x | signal %04x | snr %04x | ber %08x | unc %08x | ",
                status, signal, snr, ber, uncorrected_blocks);
    }
    */

    if (status & FE_HAS_LOCK) {
        LOG(dvb, debug, "FE_HAS_LOCK");
    }
}


bool
Frontend::waitForLock(Poco::Timestamp::TimeDiff timeout)
{
    LOG(dvb, debug, "frontend wait for lock ...");
    Poco::Timestamp now;
    while (!timeout || (!hasLock() && now.elapsed() < timeout)) {
        Poco::Thread::sleep(10);
    }
    if (hasLock()) {
        LOG(dvb, debug, "frontend has lock.");
        return true;
    }
    else {
        LOG(dvb, error, "frontend timeout.");
        return false;
    }
}


bool
Frontend::hasLock()
{
    fe_status_t status;
    if (!ioctl(_fileDescFrontend, FE_READ_STATUS, &status) && (status & FE_HAS_LOCK)) {
        return true;
    }
    else {
        return false;
    }
}


std::string
Frontend::trim(const std::string& str)
	/// Returns a copy of str with all leading and
	/// trailing whitespace removed.
{
	int first = 0;
	int last  = int(str.size()) - 1;

	while (first <= last && std::iscntrl(str[first])) ++first;
	while (last >= first && std::iscntrl(str[last])) --last;

	return std::string(str, first, last - first + 1);
}


Lnb::Lnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal) :
_desc(desc),
_lowVal(lowVal),
_highVal(highVal),
_switchVal(switchVal)
{
}


bool
Lnb::isHiBand(unsigned int freq, unsigned int& ifreq)
{
    bool hiBand = false;

    if (_switchVal && _highVal && freq >= _switchVal) {
        hiBand = true;
    }

    if (hiBand)
        ifreq = freq - _highVal;
    else {
        if (freq < _lowVal)
            ifreq = _lowVal - freq;
        else
            ifreq = freq - _lowVal;
    }

    return hiBand;
}


SatFrontend::SatFrontend(Adapter* pAdapter, int num) :
Frontend(pAdapter, num)
{
    // FIXME: when do we need to multiply freqs with 1000?
    _lnbs["UNIVERSAL"] = new Lnb("Europe | 10800 to 11800 MHz and 11600 to 12700 Mhz | Dual LO, loband 9750, hiband 10600 MHz",
                                    9750000, 10600000, 11700000);
    _lnbs["DBS"] = new Lnb("Expressvu, North America | 12200 to 12700 MHz | Single LO, 11250 MHz",
                              11250, 0, 0);
    _lnbs["STANDARD"] = new Lnb("10945 to 11450 Mhz | Single LO, 10000 Mhz",
                                   10000, 0, 0);
    _lnbs["ENHANCED"] = new Lnb("Astra | 10700 to 11700 MHz | Single LO, 9750 MHz",
                                   9750, 0, 0);
    _lnbs["C-BAND"] = new Lnb("Big Dish | 3700 to 4200 MHz | Single LO, 5150 Mhz",
                              5150, 0, 0);
    _pLnb = _lnbs["UNIVERSAL"];
}


bool
SatFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "sat frontend tuning to transponder with frequency: " + Poco::NumberFormatter::format(pTransponder->_freq) + " ...");

    SatTransponder* pTrans = static_cast<SatTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    unsigned int ifreq;
    bool hiBand = _pLnb->isHiBand(pTrans->_freq, ifreq);
    diseqc(pTrans->_satNum, pTrans->_polarization, hiBand);

    tuneto.frequency = ifreq;
    tuneto.inversion = INVERSION_AUTO;
    tuneto.u.qpsk.symbol_rate = pTrans->_symbolRate;
    tuneto.u.qpsk.fec_inner = FEC_AUTO;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "sat frontend tuning failed.");
        return false;
    }
    return waitForLock(_frontendTimeout);
}


Transponder*
SatFrontend::createTransponder(unsigned int freq, unsigned int tid)
{
    return new SatTransponder(this, freq, tid);
}


void
SatFrontend::diseqc(unsigned int satNum, const std::string& polarization, bool hiBand)
{
    LOG(dvb, debug, "diseqc command on sat: " + Poco::NumberFormatter::format(satNum) + " ...");

    struct diseqc_cmd {
        struct dvb_diseqc_master_cmd cmd;
        uint32_t wait;
    } cmd = { {{0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00}, 4}, 0 };

    // param: high nibble: reset bits, low nibble set bits,
    // bits are: option, position, polarization, band

    cmd.cmd.msg[3] =
        0xf0 | (((satNum * 4) & 0x0f) | (hiBand ? 1 : 0) | ((polarization == SatTransponder::POL_VERT) ? 0 : 2));

    fe_sec_voltage_t voltage = (polarization == SatTransponder::POL_VERT) ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18;
    fe_sec_tone_mode_t tone = hiBand ? SEC_TONE_ON : SEC_TONE_OFF;
    fe_sec_mini_cmd_t burst = satNum % 2 ? SEC_MINI_B : SEC_MINI_A;

    if (ioctl(_fileDescFrontend, FE_SET_TONE, SEC_TONE_OFF) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }
    if (ioctl(_fileDescFrontend, FE_SET_VOLTAGE, voltage) == -1) {
        LOG(dvb, error, "FE_SET_VOLTAGE failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_MASTER_CMD, &cmd.cmd) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_MASTER_CMD failed");
    }
    usleep(cmd.wait * 1000);
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_BURST, burst) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_BURST failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_SET_TONE, tone) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }

    LOG(dvb, debug, "diseqc command finished.");
}


bool
TerrestrialFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "terrestrial frontend tuning to transponder with frequency: " + Poco::NumberFormatter::format(pTransponder->_freq) + " ...");

    TerrestrialTransponder* pTrans = static_cast<TerrestrialTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    tuneto.frequency = pTrans->_freq;
    tuneto.inversion = INVERSION_AUTO;
    tuneto.u.ofdm.bandwidth = pTrans->_bandwidth;
    tuneto.u.ofdm.code_rate_HP = pTrans->_code_rate_HP;
    tuneto.u.ofdm.code_rate_LP = pTrans->_code_rate_LP;
    tuneto.u.ofdm.constellation = pTrans->_constellation;
    tuneto.u.ofdm.transmission_mode = pTrans->_transmission_mode;
    tuneto.u.ofdm.guard_interval = pTrans->_guard_interval;
    tuneto.u.ofdm.hierarchy_information = pTrans->_hierarchy_information;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "terrestrial frontend tuning failed.");
        return false;
    }
    return waitForLock(_frontendTimeout);
}


Transponder*
TerrestrialFrontend::createTransponder(unsigned int freq, unsigned int tid)
{
    return new TerrestrialTransponder(this, freq, tid);
}


bool
CableFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "cable frontend tuning to transponder: " + Poco::NumberFormatter::format(pTransponder->_freq) + " ...");

    CableTransponder* pTrans = static_cast<CableTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "cable frontend tuning failed.");
        return false;
    }
    return waitForLock(_frontendTimeout);
}


Transponder*
CableFrontend::createTransponder(unsigned int freq, unsigned int tid)
{
    return new CableTransponder(this, freq, tid);
}


bool
AtscFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "atsc frontend tuning to transponder: " + Poco::NumberFormatter::format(pTransponder->_freq) + " ...");

    AtscTransponder* pTrans = static_cast<AtscTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "atsc frontend tuning failed.");
        return false;
    }
    return waitForLock(_frontendTimeout);
}


Transponder*
AtscFrontend::createTransponder(unsigned int freq, unsigned int tid)
{
    return new AtscTransponder(this, freq, tid);
}


Demux::Demux(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num)
{
    _deviceName = _pAdapter->_deviceName + "/demux" + Poco::NumberFormatter::format(_num);
}


Demux::~Demux()
{
//    closeDemux();
}


bool
Demux::selectService(Service* pService, Target target, bool blocking)
{
    // FIXME: only add stream, if service doesn't have it already
    pService->addStream(new Stream(Stream::Other, 0));
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!selectStream(*it, target, blocking)) {
            LOG(dvb, error, "demuxer failed to select service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::unselectService(Service* pService)
{
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!unselectStream(*it)) {
            LOG(dvb, error, "demuxer failed to unselect service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::runService(Service* pService, bool run)
{
    for (std::vector<Stream*>::iterator it = pService->_streams.begin(); it != pService->_streams.end(); ++it) {
        if (!runStream(*it, run)) {
            LOG(dvb, error, "demuxer failed to run service: " + pService->_name);
            return false;
        }
    }
    return true;
}


bool
Demux::selectStream(Stream* pStream, Target target, bool blocking)
{
    dmx_pes_type_t pesType;
//    if (pStream->_type == Stream::Audio) {
//        pesType = DMX_PES_AUDIO;
//    }
//    else if (pStream->_type == Stream::Video) {
//        pesType = DMX_PES_VIDEO;
//    }
//    else if (pStream->_type == Stream::ProgramClock) {
//        pesType = DMX_PES_PCR;
//    }
//    else {
        pesType = DMX_PES_OTHER;
//    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pStream->_pid;
    pesfilter.input = DMX_IN_FRONTEND;
    if (target == TargetDvr) {
        pesfilter.output = DMX_OUT_TS_TAP; // send output to dvr device
    }
    else {
        pesfilter.output = DMX_OUT_TAP; // send output to demux device
    }
    pesfilter.pes_type = pesType;
    pesfilter.flags = 0;

    bool success = true;
    int flag = blocking ? O_RDWR : O_RDWR | O_NONBLOCK;
    if ((pStream->_fileDesc = open(_deviceName.c_str(), flag)) < 0) {
        LOG(dvb, error, "opening elementary stream: " + std::string(strerror(errno)));
        success = false;
    }
    if (success && ioctl(pStream->_fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        success = false;
    }
    if (success) {
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to select stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::unselectStream(Stream* pStream)
{
    bool success = true;
    if (close(pStream->_fileDesc)) {
        LOG(dvb, error, "closing elementary stream: " + std::string(strerror(errno)));
        success = false;
    }
    if (success) {
        pStream->_fileDesc = -1;
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to unselect stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::runStream(Stream* pStream, bool run)
{
    bool success = true;
    if (run) {
        if (ioctl(pStream->_fileDesc, DMX_START) == -1) {
            LOG(dvb, error, "starting stream: " + std::string(strerror(errno)));
            success = false;
        }
    }
    else {
        if (ioctl(pStream->_fileDesc, DMX_STOP) == -1) {
            LOG(dvb, error, "stopping stream: " + std::string(strerror(errno)));
            success = false;
        }
    }
    if (success) {
        return true;
    }
    else {
        LOG(dvb, error, "demuxer failed to set run state of stream with pid: " + Poco::NumberFormatter::format(pStream->_pid));
        return false;
    }
}


bool
Demux::setSectionFilter(Stream* pStream, Poco::UInt8 tableId)
{
    struct dmx_sct_filter_params sectionFilter;

    memset(&sectionFilter, 0, sizeof(sectionFilter));
    sectionFilter.pid = pStream->_pid;
    sectionFilter.filter.filter[0] = tableId;
    sectionFilter.filter.mask[0] = 0xff;
    sectionFilter.flags |= DMX_CHECK_CRC;

    if (ioctl(pStream->_fileDesc, DMX_SET_FILTER, &sectionFilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    else {
        return true;
    }
}


bool
Demux::setSectionFilter(Stream* pStream, uint8_t filter[18], uint8_t mask[18])
{
    struct dmx_sct_filter_params sctfilter;

    memset(&sctfilter, 0, sizeof(sctfilter));
    sctfilter.pid = pStream->_pid;
    memcpy(sctfilter.filter.filter, filter, 1);
    memcpy(sctfilter.filter.filter+1, filter+3, 15);
    memcpy(sctfilter.filter.mask, mask, 1);
    memcpy(sctfilter.filter.mask+1, mask+3, 15);
    memset(sctfilter.filter.mode, 0, 16);
    sctfilter.flags |= DMX_CHECK_CRC;

    if (ioctl(pStream->_fileDesc, DMX_SET_FILTER, &sctfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed: " + std::string(strerror(errno)));
        return false;
    }
    else {
        return true;
    }
}


//void
//Demux::openDemux(bool blocking)
//{
//    LOG(dvb, debug, "opening demuxer");
//
//    int flag = blocking ? O_RDWR : O_RDWR | O_NONBLOCK;
//
//    if ((_fileDescVideo = open(_deviceName.c_str(), flag)) < 0) {
//        LOG(dvb, error, "opening video elementary stream failed: " + std::string(strerror(errno)));
//    }
//    if ((_fileDescAudio = open(_deviceName.c_str(), flag)) < 0) {
//        LOG(dvb, error, "opening audio elementary stream failed: " + std::string(strerror(errno)));
//    }
//    if ((_fileDescPcr = open(_deviceName.c_str(), flag)) < 0) {
//        LOG(dvb, error, "opening pcr elementary stream failed: " + std::string(strerror(errno)));
//    }
//    if (_pAdapter->_recPsi){
//        if ((_fileDescPat = open(_deviceName.c_str(), flag)) < 0) {
//            LOG(dvb, error, "opening pat elementary stream failed: " + std::string(strerror(errno)));
//        }
//        if ((_fileDescPmt = open(_deviceName.c_str(), flag)) < 0) {
//            LOG(dvb, error, "opening pmt elementary stream failed: " + std::string(strerror(errno)));
//        }
//    }
//}
//
//
//void
//Demux::closeDemux()
//{
//    LOG(dvb, debug, "closing demuxer");
//
//
//    if (close(_fileDescVideo)) {
//        LOG(dvb, error, "failed to close video elementary stream: " + std::string(strerror(errno)));
//    }
//    if (close(_fileDescAudio)) {
//        LOG(dvb, error, "failed to close audio elementary stream: " + std::string(strerror(errno)));
//    }
//    if (close(_fileDescPcr)) {
//        LOG(dvb, error, "failed to close pcr elementary stream: " + std::string(strerror(errno)));
//    }
//    if (_pAdapter->_recPsi) {
//        if (close(_fileDescPat)) {
//            LOG(dvb, error, "failed to close pat elementary stream: " + std::string(strerror(errno)));
//        }
//        if (close(_fileDescPmt)) {
//            LOG(dvb, error, "failed to close pmt elementary stream: " + std::string(strerror(errno)));
//        }
//    }
//}
//
//
//void
//Demux::start()
//{
//    LOG(dvb, debug, "start demuxer");
//
//    if (ioctl(_fileDescVideo, DMX_START) == -1) {
//        LOG(dvb, error, "starting video dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescAudio, DMX_START) == -1) {
//        LOG(dvb, error, "starting audio dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPcr, DMX_START) == -1) {
//        LOG(dvb, error, "starting pcr dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPat, DMX_START) == -1) {
//        LOG(dvb, error, "starting pat dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPmt, DMX_START) == -1) {
//        LOG(dvb, error, "starting pmt dmx filter failed: " + std::string(strerror(errno)));
//    }
//}
//
//
//void
//Demux::stop()
//{
//    LOG(dvb, debug, "stop demuxer");
//
//    if (ioctl(_fileDescVideo, DMX_STOP) == -1) {
//        LOG(dvb, error, "stopping video dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescAudio, DMX_STOP) == -1) {
//        LOG(dvb, error, "stopping audio dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPcr, DMX_STOP) == -1) {
//        LOG(dvb, error, "stopping pcr dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPat, DMX_STOP) == -1) {
//        LOG(dvb, error, "stopping pat dmx filter failed: " + std::string(strerror(errno)));
//    }
//    if (ioctl(_fileDescPmt, DMX_STOP) == -1) {
//        LOG(dvb, error, "stopping pmt dmx filter failed: " + std::string(strerror(errno)));
//    }
//}
//
//
//bool
//Demux::setVideoPid(unsigned int pid)
//{
//    return setPid(_fileDescVideo, pid, DMX_PES_VIDEO);
//}
//
//
//bool
//Demux::setAudioPid(unsigned int pid)
//{
//    return setPid(_fileDescAudio, pid, DMX_PES_AUDIO);
//}
//
//
//bool
//Demux::setPcrPid(unsigned int pid)
//{
//    return setPid(_fileDescPcr, pid, DMX_PES_PCR);
//}
//
//
//bool
//Demux::setPatPid(unsigned int pid)
//{
//    if (_pAdapter->_recPsi) {
//        return setPid(_fileDescPat, pid, DMX_PES_OTHER);
//    }
//    return false;
//}
//
//
//bool
//Demux::setPmtPid(unsigned int pid)
//{
//    if (_pAdapter->_recPsi) {
//        return setPid(_fileDescPmt, pid, DMX_PES_OTHER);
//    }
//    return false;
//}
//
//
//std::istream*
//Demux::getVideoStream()
//{
//    return new UnixFileIStream(_fileDescVideo);
//}
//
//
//std::istream*
//Demux::getAudioStream()
//{
//    return new UnixFileIStream(_fileDescAudio);
//}


bool
Demux::setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType)
{
    if (pid >= 0x1fff) {  // ignore this pid to allow radio services
        return true;
    }

//    int buffersize = 64 * 1024;
//    if (ioctl(fileDesc, DMX_SET_BUFFER_SIZE, buffersize) == -1) {
//        LOG(dvb, error, "DMX_SET_BUFFER_SIZE failed: " + std::string(strerror(errno)));
//    }

    struct dmx_pes_filter_params pesfilter;
    pesfilter.pid = pid;
    pesfilter.input = DMX_IN_FRONTEND;
    if (Device::instance()->useDvrDevice()) {
        pesfilter.output = DMX_OUT_TS_TAP; // send output to dvr device
    }
    else {
        pesfilter.output = DMX_OUT_TAP; // send output to demux device
    }
    pesfilter.pes_type = pesType;
    pesfilter.flags = 0;

    if (ioctl(fileDesc, DMX_SET_PES_FILTER, &pesfilter) == -1) {
        LOG(dvb, error, "DMX_SET_PES_FILTER failed");
        return false;
    }

    return true;
}


unsigned int
Demux::getPmtPid(unsigned int tid, int sid)
{
    LOG(dvb, trace, "get PMT PID for service id: " + Poco::NumberFormatter::format(sid) + " ...");

    int pmt_pid = 0;
    int count;
    int section_length;
    unsigned int transport_stream_id;
    unsigned char buft[4096];
    unsigned char *buf = buft;
    struct dmx_sct_filter_params f;

    memset(&f, 0, sizeof(f));
    f.pid = 0;
    f.filter.filter[0] = 0x00;
    f.filter.mask[0] = 0xff;
    f.timeout = 0;
    // FIXME: don't set DMX_IMMEDIATE_START, use start() and stop()
    f.flags = DMX_IMMEDIATE_START | DMX_CHECK_CRC;

    // FIXME: pat filter set twice?
    LOG(dvb, trace, "set demuxer filter ...");
    if (ioctl(_fileDescPat, DMX_SET_FILTER, &f) == -1) {
        LOG(dvb, error, "DMX_SET_FILTER failed");
        return 0;
    }
    LOG(dvb, trace, "set demuxer filter finished.");

    bool patread = false;
    while (!patread) {
        LOG(dvb, trace, "read PAT (pid 0) section into buffer ...");
        if (((count = read(_fileDescPat, buf, sizeof(buft))) < 0) && errno == EOVERFLOW) {
            LOG(dvb, trace, "read elementary stream pid 0 into buffer failed, second try ...");
            count = read(_fileDescPat, buf, sizeof(buft));
        }
        if (count < 0) {
            LOG(dvb, error, "while reading sections");
            return 0;
        }
        LOG(dvb, trace, "read " + Poco::NumberFormatter::format(count) + " bytes from elementary stream pid 0 into buffer.");

        section_length = ((buf[1] & 0x0f) << 8) | buf[2];
        LOG(dvb, trace, "section length: " + Poco::NumberFormatter::format(section_length));

        if (count != section_length + 3) {
            LOG(dvb, error, "PAT size mismatch, next read attempt.");
            continue;
        }

        transport_stream_id = (buf[3] << 8) | buf[4];
        LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transport_stream_id));
        if (transport_stream_id != tid) {
            LOG(dvb, error, "PAT tid mismatch (" + Poco::NumberFormatter::format(tid) + "), next read attempt.");
            continue;
        }

        buf += 8;
        section_length -= 8;

        patread = true;    // assumes one section contains the whole pat
        while (section_length > 0) {
            int service_id = (buf[0] << 8) | buf[1];
            LOG(dvb, trace, "search for service id in section, found: " + Poco::NumberFormatter::format(service_id));
            if (service_id == sid) {
                pmt_pid = ((buf[2] & 0x1f) << 8) | buf[3];
                section_length = 0;
                LOG(dvb, trace, "found service id, pmt pid is: " + Poco::NumberFormatter::format(pmt_pid));
            }
            buf += 4;
            section_length -= 4;
        }
    }
    LOG(dvb, trace, "get PMT PID for service id finished.");

    return pmt_pid;
}


Dvr::Dvr(Adapter* pAdapter, int num) :
_pAdapter(pAdapter),
_num(num),
_pDvrStream(0),
//_useByteQueue(true),
_useByteQueue(false),
_fileDescDvr(-1),
_byteQueue(100*1024),
_bufferSize(50*1024),
_pollTimeout(1),
_pReadThread(0),
_readThreadRunnable(*this, &Dvr::readThread),
_readThreadRunning(false)
{
    _deviceName = _pAdapter->_deviceName + "/dvr" + Poco::NumberFormatter::format(_num);
}


Dvr::~Dvr()
{
}


void
Dvr::openDvr(bool blocking)
{
    LOG(dvb, debug, "opening dvb rec device.");

    if (_pDvrStream) {
        LOG(dvb, debug, "dvb rec device already open.");
        return;
    }
    else {
        int flags = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if ((_fileDescDvr = open(_deviceName.c_str(), flags)) < 0) {
            LOG(dvb, error, "failed to open dvb rec device \"" + _deviceName + "\": " + strerror(errno));
            return;
        }
        if (_useByteQueue) {
            _fileDescPoll[0].fd = _fileDescDvr;
            _fileDescPoll[0].events = POLLIN;
            _pDvrStream = new ByteQueueIStream(_byteQueue);
        }
        else {
            _pDvrStream = new UnixFileIStream(_fileDescDvr, _bufferSize);
        }
        if (!_pDvrStream) {
            LOG(dvb, error, "failed to open dvb rec stream.");
            return;
        }
//        if (_useByteQueue) {
//            startReadThread();
//        }
    }
}


void
Dvr::closeDvr()
{
    LOG(dvb, debug, "closing dvb rec device.");

//    stopReadThread();
    if (_pDvrStream) {
        delete _pDvrStream;
        _pDvrStream = 0;
        if (close(_fileDescDvr)) {
            LOG(dvb, error, "failed to close dvb rec device \"" + _deviceName + "\": " + strerror(errno));
        }
        _fileDescDvr = -1;
    }
    else {
        LOG(dvb, debug, "dvb rec device already closed.");
    }
}


void
Dvr::clearBuffer()
{
    if (_pDvrStream) {
        const int bufsize(_bufferSize);
        char buf[bufsize];
        while (int bytes = _pDvrStream->readsome(buf, bufsize)) {
            LOG(dvb, debug, "clear buffer: " + Poco::NumberFormatter::format(bytes) + " bytes");
        }
    }
}


void
Dvr::prefillBuffer()
{
    if (_pReadThread) {
        while (_byteQueue.level() < _byteQueue.size() * 0.5) {
            Poco::Thread::sleep(1);
        }
    }
}


void
Dvr::setBlocking(bool blocking)
{
    LOG(dvb, debug, "set dvb rec device to " + std::string(blocking ? "blocking" : "non-blocking"));

    if (_pDvrStream) {
        long fcntlFlag = blocking ? O_RDONLY : O_RDONLY | O_NONBLOCK;
        if (fcntl(_fileDescDvr, F_SETFL, fcntlFlag) < 0) {
            LOG(dvb, error, "failed to set dvb rec device \"" + _deviceName + "\" to " + (blocking ? "blocking" : "non-blocking") + ":" + strerror(errno));
        }
    }
}


void
Dvr::startReadThread()
{
    LOG(dvb, debug, "start read thread ...");

    if (!_pReadThread) {
        _readThreadRunning = true;
        _pReadThread = new Poco::Thread;
        _pReadThread->start(_readThreadRunnable);
    }
}


void
Dvr::stopReadThread()
{
    LOG(dvb, debug, "stop read thread ...");

    if (_pReadThread) {
        _readThreadLock.lock();
        _readThreadRunning = false;
        _readThreadLock.unlock();
        if (!_pReadThread->tryJoin(_pollTimeout)) {
            LOG(dvb, error, "failed to join read thread");
        }
        delete _pReadThread;
        _pReadThread = 0;
    }

    LOG(dvb, debug, "read thread stopped.");
}


bool
Dvr::readThreadRunning()
{
    Poco::ScopedLock<Poco::FastMutex> lock(_readThreadLock);
    return _readThreadRunning;
}


std::istream*
Dvr::getStream()
{
    LOG(dvb, debug, "get dvb rec stream.");

    return _pDvrStream;
}


void
Dvr::readThread()
{
    LOG(dvb, debug, "read thread started.");

    char buf[_bufferSize];
    while (readThreadRunning()) {
        if (poll(_fileDescPoll, 1, _pollTimeout)) {
            if (_fileDescPoll[0].revents & POLLIN){
                int len = read(_fileDescDvr, buf, _bufferSize);
                if (len > 0) {
                    _byteQueue.write(buf, len);
                }
                else if (len < 0) {
                    LOG(dvb, error, "dvr read thread failed to read from device");
                    break;
                }
            }
        }
    }

    LOG(dvb, debug, "read thread finished.");
}


Adapter::Adapter(int num)
{
    _deviceName = "/dev/dvb/adapter" + Poco::NumberFormatter::format(num);
    _recPsi = true;

//    _pFrontend = new Frontend(this, 0);

}


Adapter::~Adapter()
{
//    delete _pFrontend;

}


void
Adapter::addFrontend(Frontend* pFrontend)
{
//    _pFrontend = pFrontend;
    _frontends.push_back(pFrontend);
}


void
Adapter::openAdapter()
{
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->openFrontend();
    }
//    _pFrontend->openFrontend();

}


void
Adapter::closeAdapter()
{
//    _pFrontend->closeFrontend();
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->closeFrontend();
    }
}


//Demux*
//Adapter::getDemux()
//{
//    return _pDemux;
//}
//
//
//Dvr*
//Adapter::getDvr()
//{
//    return _pDvr;
//}


void
Adapter::readXml(Poco::XML::Node* pXmlAdapter)
{
    LOG(dvb, debug, "read adapter ...");

    if (pXmlAdapter->hasChildNodes()) {
        Poco::XML::Node* pXmlFrontend = pXmlAdapter->firstChild();
        std::string frontendType = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("type");
        int numFrontend = 0;
        while (pXmlFrontend && pXmlFrontend->nodeName() == "frontend") {
            Frontend* pFrontend;
            if (frontendType == Frontend::DVBS) {
                pFrontend = new SatFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::DVBT) {
                pFrontend = new TerrestrialFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::DVBC) {
                pFrontend = new CableFrontend(this, numFrontend);
            }
            else if (frontendType == Frontend::ATSC) {
                pFrontend = new AtscFrontend(this, numFrontend);
            }
            addFrontend(pFrontend);
            pFrontend->readXml(pXmlFrontend);
            pXmlFrontend = pXmlFrontend->nextSibling();
            numFrontend++;
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no frontends");
        return;
    }

    LOG(dvb, debug, "read adapter.");
}


void
Adapter::writeXml(Poco::XML::Element* pDvbDevice)
{
    LOG(dvb, debug, "write adapter ...");

    Poco::XML::Document* pDoc = pDvbDevice->ownerDocument();
    Poco::XML::Element* pAdapter = pDoc->createElement("adapter");
    pDvbDevice->appendChild(pAdapter);
//    if (_pFrontend) {
//        _pFrontend->writeXml(pAdapter);
//    }
    for (std::vector<Frontend*>::iterator it = _frontends.begin(); it != _frontends.end(); ++it) {
        (*it)->writeXml(pAdapter);
    }


    LOG(dvb, debug, "wrote adapter.");
}


Device* Device::_pInstance = 0;

Device::Device() :
_useDvrDevice(true),
//_blockDvrDevice(false),
_blockDvrDevice(true),
// _blockDvrDevice = true then reopen device fails (see _reopenDvrDevice), _blockDvrDevice = false then stream has zero length
//_reopenDvrDevice(true)
_reopenDvrDevice(false)
// renderer in same process as dvb server: reopenDvrDevice = true then dvr device busy, reopenDvrDevice = false then stream sometimes broken
// renderer in different process as dvb server: reopenDvrDevice = true ok, reopenDvrDevice = false then stream sometimes broken
{
}


Device::~Device()
{
    for(std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        delete *it;
    }
}


Device*
Device::instance()
{
    if (!_pInstance) {
        _pInstance = new Device;
    }
    return _pInstance;
}


Device::ServiceIterator
Device::serviceBegin()
{
    return _serviceMap.begin();
}


Device::ServiceIterator
Device::serviceEnd()
{
    return _serviceMap.end();
}


//void
//Device::init()
//{
//    Omm::Dvb::Adapter* pAdapter = new Omm::Dvb::Adapter(0);
//    pAdapter->openAdapter();
//    Omm::Dvb::Device::instance()->addAdapter(pAdapter);
//}


void
Device::open()
{
    for (std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
        (*it)->openAdapter();
    }
}


void
Device::scan(const std::string& initialTransponderData)
{
    // TODO: allocate adapters and frontend according to device nodes in system
    _adapters[0]->_frontends[0]->scan(initialTransponderData);
    initServiceMap();
}


void
Device::readXml(std::istream& istream)
{
    LOG(dvb, debug, "read device ...");

    Poco::AutoPtr<Poco::XML::Document> pXmlDoc = new Poco::XML::Document;
    Poco::XML::InputSource xmlFile(istream);

    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    try {
        pXmlDoc = parser.parse(&xmlFile);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "parsing dvb description failed: " + e.displayText());
        return;
    }

    Poco::XML::Node* pDvbDevice = pXmlDoc->documentElement();
    if (!pDvbDevice || pDvbDevice->nodeName() != "device") {
        LOG(dvb, error, "xml not a valid dvb description");
        return;
    }
    if (pDvbDevice->hasChildNodes()) {
        Poco::XML::Node* pXmlAdapter = pDvbDevice->firstChild();
        int numAdapter = 0;
        while (pXmlAdapter && pXmlAdapter->nodeName() == "adapter") {
            Adapter* pAdapter = new Adapter(numAdapter);
            addAdapter(pAdapter);
            pAdapter->readXml(pXmlAdapter);
            pXmlAdapter = pXmlAdapter->nextSibling();
            numAdapter++;
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no adapters");
        return;
    }
    initServiceMap();

    LOG(dvb, debug, "read device.");
}


void
Device::writeXml(std::ostream& ostream)
{
    LOG(dvb, debug, "write device ...");
    Poco::AutoPtr<Poco::XML::Document> pXmlDoc = new Poco::XML::Document;

    Poco::XML::DOMWriter writer;
    writer.setOptions(Poco::XML::XMLWriter::WRITE_XML_DECLARATION | Poco::XML::XMLWriter::PRETTY_PRINT);

    Poco::XML::Element* pDvbDevice = pXmlDoc->createElement("device");
    pXmlDoc->appendChild(pDvbDevice);
    try {
        for (std::vector<Adapter*>::iterator it = _adapters.begin(); it != _adapters.end(); ++it) {
            (*it)->writeXml(pDvbDevice);
        }
        writer.writeNode(ostream, pXmlDoc);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "writing dvb description failed: " + e.displayText());
    }
    LOG(dvb, debug, "wrote device.");
}


void
Device::addAdapter(Adapter* pAdapter)
{
    _adapters.push_back(pAdapter);
}


//bool
//Device::tune(Transponder* pTransponder)
//{
//    LOG(dvb, debug, "start tuning ...");
//
//    if (_adapters.size() == 0) {
//        LOG(dvb, error, "no adapter found, tuning aborted.");
//        return false;
//    }
////    if (!_adapters[0]->_pFrontend) {
////        LOG(dvb, error, "no frontend found, tuning aborted.");
////        return false;
////    }
//    if (_adapters[0]->_frontends.size() == 0) {
//        LOG(dvb, error, "no frontend found, tuning aborted.");
//        return false;
//    }
//    return _adapters[0]->_frontends[0]->tune(pTransponder);
//}


//void
//Device::stopTune()
//{
//    _adapters[0]->_frontends[0]->stopTune();
//    LOG(dvb, debug, "stopped tuning.");
//}


Transponder*
Device::getTransponder(const std::string& serviceName)
{
    ServiceIterator it = _serviceMap.find(serviceName);
    if (it != _serviceMap.end() && it->second.size()) {
        return it->second[0];
    }
    else {
        LOG(dvb, error, "could not find transponder for service: " + serviceName);
        return 0;
    }
}


    // FIXME: two subsequent getStream() without stopping the stream may lead to
    //        a blocked dvr device: engine stops reading the previous stream
    //        when receiving the new stream. This may overlap and the file
    //        handle is still open. Even if the engine is stopped right before
    //        playing a new stream, it could take a while until reading of stream
    //        is stopped, too (stop() and play() are typically async calls into
    //        the engine).
    //        DvbModel needs a way to interrupt current stream and close file
    //        handles.
    //        UPDATE: this only happens when renderer and dvb server run in the
    //        same process.
    //        UPDATE: man(2) close:
    //        It is probably unwise to close file descriptors while they may be in use by system calls in other threads  in  the
    //        same  process.  Since a file descriptor may be reused, there are some obscure race conditions that may cause unin-
    //        tended side effects.
    //        UPDATE: as long as the same thread accesses the device, it is not busy. This happens, if the http request is
    //        run in the same thread as the previous http request. Correction: even in same thread, dvr device cannot be
    //        opened ("Device or resource busy").

std::istream*
Device::getStream(const std::string& serviceName)
{
    Transponder* pTransponder = getTransponder(serviceName);
    Frontend* pFrontend = pTransponder->_pFrontend;
    // TODO: check if not already tuned to transponder
    bool tuneSuccess = pFrontend->tune(pTransponder);
    if (!tuneSuccess) {
        return 0;
    }

    Service* pService = pTransponder->getService(serviceName);
    Demux* pDemux = pFrontend->_pDemux;
    // TODO: check if service not already selected on demuxer
    pDemux->selectService(pService, Demux::TargetDvr);
    pDemux->runService(pService, true);

    LOG(dvb, debug, "reading from dvr device ...");
    // TODO: return stream from Service (which gets it from own muxer)
    Dvr* pDvr = pFrontend->_pDvr;
    std::istream* pStream = pDvr->getStream();
    _streamMap[pStream] = pService;
    return pStream;
}


//std::istream*
//Device::getStream()
//{
//    if (_useDvrDevice && _reopenDvrDevice) {
//        _adapters[0]->getDvr()->openDvr(_blockDvrDevice);
//    }
//    _adapters[0]->getDemux()->start();
//    if (_useDvrDevice) {
//        _adapters[0]->getDvr()->startReadThread();
//        _adapters[0]->getDvr()->prefillBuffer();
//        return _adapters[0]->getDvr()->getStream();
//    }
//    else {
//        return _adapters[0]->getDemux()->getAudioStream();
////        return _adapters[0]->getDemux()->getVideoStream();
//    }
//}


void
Device::freeStream(std::istream* pIstream)
{

    // TODO: only stop and free service stream (not complete demux)
    Service* pService = _streamMap[pIstream];
    if (!pService) {
        return;
    }
    Demux* pDemux = pService->_pTransponder->_pFrontend->_pDemux;
    pDemux->runService(pService, false);
    pDemux->unselectService(pService);
    Dvr* pDvr = pService->_pTransponder->_pFrontend->_pDvr;
    if (_useDvrDevice) {
        pDvr->clearBuffer();
    }
    _streamMap.erase(pIstream);

    //    _adapters[0]->getDemux()->stop();
//    if (_useDvrDevice) {
//        _adapters[0]->getDvr()->stopReadThread();
//        _adapters[0]->getDvr()->clearBuffer();
//        if (_reopenDvrDevice) {
////            _adapters[0]->getDvr()->setBlocking(false);
//            _adapters[0]->getDvr()->closeDvr();
//        }
//    }
}


bool
Device::useDvrDevice()
{
    return _useDvrDevice;
}


bool
Device::blockDvrDevice()
{
    return _blockDvrDevice;
}


void
Device::initServiceMap()
{
    for (std::vector<Adapter*>::iterator ait = _adapters.begin(); ait != _adapters.end(); ++ait) {
        for (std::vector<Frontend*>::iterator fit = (*ait)->_frontends.begin(); fit != (*ait)->_frontends.end(); ++fit) {
            for (std::vector<Transponder*>::iterator tit = (*fit)->_transponders.begin(); tit != (*fit)->_transponders.end(); ++tit) {
                for (std::vector<Service*>::iterator sit = (*tit)->_services.begin(); sit != (*tit)->_services.end(); ++sit) {
                    _serviceMap[(*sit)->_name].push_back(*tit);
                }
            }
        }
    }
}


}  // namespace Omm
}  // namespace Dvb
