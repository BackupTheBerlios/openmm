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
#include <Poco/DOM/Document.h>

#include "Log.h"
#include "DvbLogger.h"
#include "Stream.h"
#include "ElementaryStream.h"
#include "Mux.h"


namespace Omm {
namespace Dvb {

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
_pid(pid)
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


bool
Stream::isAudio()
{
    if (_type == Audio ||
        _type == AudioMpeg1_11172 ||
        _type == AudioMpeg2_13818_3 ||
        _type == AudioISO13818_7_ADTS ||
        _type == AudioISO14496_3 ||
        _type == AudioAtscAc3
        ) {
        return true;
    }
    return false;
}


bool
Stream::isVideo()
{
    if (_type == Video ||
        _type == VideoMpeg1_11172 ||
        _type == VideoMpeg2_H262
        ) {
        return true;
    }
    return false;
}


Poco::UInt16
Stream::getPid()
{
    return _pid;
}


//int
//Stream::getFileDesc()
//{
//    return _fileDesc;
//}


//void
//Stream::read(Poco::UInt8* buf, int size, int timeout)
//{
//    if (!timeout) {
//        ::read(_fileDesc, buf, size);
//    }
//    else {
//        if (poll(_fileDescPoll, 1, timeout * 1000)) {
//            if (_fileDescPoll[0].revents & POLLIN){
//                ::read(_fileDesc, buf, size);
//            }
//        }
//        else {
//            throw Poco::TimeoutException("dvb read stream timeout");
//        }
//    }
    // NOTE: readsome() doesn't work
//    else {
//        int bytesRead = 0;
//        while (timeout) {
//            bytesRead += _pStream->readsome((char*)buf, size);
////            _pStream->read((char*)buf, size);
////            bytesRead = size;
//            LOG(dvb, trace, "stream bytes read: " + Poco::NumberFormatter::format(bytesRead) + ", timeout: " + Poco::NumberFormatter::format(timeout));
//            if (bytesRead == size) {
//                break;
//            }
//            buf += bytesRead;
//            Poco::Thread::sleep(1000);
//            timeout--;
//        }
//        if (!timeout) {
//            throw Poco::TimeoutException("dvb read stream timeout");
//        }
//    }
//}


//std::istream*
//Stream::getStream()
//{
//    return _pStream;
//}


//void
//Stream::skipToElementaryStreamPacketHeader(Poco::UInt8* skippedBytes, int timeout)
//{
//    skippedBytes = new Poco::UInt8[ElementaryStreamPacket::getMaxSize()];
//    Poco::UInt8* skippedBytesBuf = skippedBytes;
//
//    LOG(dvb, trace, "skipping to PES header ...");
//
//    unsigned int skippedBytesCount = 0;
//    const unsigned int warnAfterBytesSkipped = ElementaryStreamPacket::getMaxSize() / 1024;
//    const unsigned int exitAfterBytesSkipped = ElementaryStreamPacket::getMaxSize();
//    bool foundStartCodePrefix = false;
//    Poco::UInt8 byte;
//
//    while (!foundStartCodePrefix) {
//        try {
//            read(&byte, 1, timeout);
//            if (byte == 0x00) {
//                read(&byte, 1, timeout);
//                if (byte == 0x00) {
//                    read(&byte, 1, timeout);
//                    if (byte == 0x01) {
//                        foundStartCodePrefix = true;
//                    }
//                    else {
//                        skippedBytesCount++;
//                        if (skippedBytes) {
//                            *skippedBytes = byte;
//                            skippedBytes++;
//                        }
//                    }
//                }
//                else {
//                    skippedBytesCount++;
//                    if (skippedBytes) {
//                        *skippedBytes = byte;
//                        skippedBytes++;
//                    }
//                }
//            }
//            else {
//                skippedBytesCount++;
//                if (skippedBytes) {
//                    *skippedBytes = byte;
//                    skippedBytes++;
//                }
//            }
//        }
//        catch(Poco::Exception& e) {
//            LOG(dvb, error, "timeout while skipping to PES header (" + e.displayText() + ")");
//            throw Poco::Exception("timeout while skipping to PES header");
//        }
//        if (!foundStartCodePrefix && (skippedBytesCount % warnAfterBytesSkipped == 0)) {
//            LOG(dvb, warning, "could not find PES header after skipping bytes: " + Poco::NumberFormatter::format(skippedBytesCount));
//        }
//        if (!foundStartCodePrefix && (skippedBytesCount >= exitAfterBytesSkipped)) {
//            LOG(dvb, error, "could not find PES header after reading max bytes: " + Poco::NumberFormatter::format(skippedBytesCount));
//            break;
////            throw Poco::Exception("could not find PES header after reading max bytes: " + Poco::NumberFormatter::format(skippedBytesCount));
//        }
//    }
//    if (foundStartCodePrefix) {
//        LOG(dvb, trace, "found PES header after skipping bytes: " + Poco::NumberFormatter::format(skippedBytesCount));
//    }
//
//    std::ofstream pes((Poco::NumberFormatter::format(_pid) + ".pes." + Poco::NumberFormatter::format(_logSequence++)).c_str());
//    pes.write((char*)(void*)skippedBytesBuf, skippedBytesCount);
//}
//
//
//ElementaryStreamPacket*
//Stream::getElementaryStreamPacket(int timeout)
//{
//    std::ofstream pes((Poco::NumberFormatter::format(_pid) + ".pes." + Poco::NumberFormatter::format(_logSequence++)).c_str());
//
//    LOG(dvb, trace, "read PES packet ...");
//
//    try {
//        ElementaryStreamPacket* pPacket = new ElementaryStreamPacket;
//        read((Poco::UInt8*)pPacket->getDataAfterStartcodePrefix(), 3, timeout);
//        pes.write((char*)pPacket->getDataAfterStartcodePrefix(), 3);
//
//        LOG(dvb, trace, "read packet of type: " + Poco::NumberFormatter::formatHex(pPacket->getStreamId()) + ", size: " + Poco::NumberFormatter::format(pPacket->getSize()));
//        if (pPacket->getSize()) {
//            read((Poco::UInt8*)pPacket->getDataStart(), pPacket->getSize(), timeout);
//            pes.write((char*)pPacket->getDataStart(), pPacket->getSize());
//
//            Poco::UInt8 startCodePrefix[3];
////            read(startCodePrefix, 3, timeout);
//            read(startCodePrefix, 6, timeout);
//            pes.write((char*)startCodePrefix, 6);
//
//            if (startCodePrefix[0] != 0x00 || startCodePrefix[1] != 0x00 || startCodePrefix[2] != 0x01) {
//                LOG(dvb, warning, "no start code prefix found after pes packet with fixed size");
//                return 0;
//            }
//        }
//        else {
//            skipToElementaryStreamPacketHeader((Poco::UInt8*)(pPacket->getDataStart()), timeout);
//        }
//        LOG(dvb, trace, "PES packet successfully read.");
//        return pPacket;
//    }
//    catch(Poco::Exception& e) {
//        LOG(dvb, error, "timeout while reading PES packet (" + e.displayText() + ")");
//        return 0;
//    }
//}


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


Multiplex::Multiplex() :
Stream(Other, 0x2000)
{
}


}  // namespace Omm
}  // namespace Dvb
