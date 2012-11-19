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

#ifndef Stream_INCLUDED
#define Stream_INCLUDED

#include <sys/poll.h>

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

#include "DvbUtil.h"


namespace Omm {
namespace Dvb {

class ElementaryStreamPacket;


class Stream
{
    friend class Demux;
    friend class Device;

public:
    static const std::string Video;
    static const std::string Audio;
    static const std::string VideoMpeg1_11172;
    static const std::string VideoMpeg2_H262;
    static const std::string AudioMpeg1_11172;
    static const std::string AudioMpeg2_13818_3;
    static const std::string Mpeg2PrivateTableSections;
    static const std::string Mpeg2PesPrivateData;
    static const std::string MhegPackets;
    static const std::string Mpeg2AnnexA_DSMCC;
    static const std::string ITUTRecH2221;
    static const std::string ISO13818_6_typeA;
    static const std::string ISO13818_6_typeB;
    static const std::string ISO13818_6_typeC;
    static const std::string ISO13818_6_typeD;
    static const std::string Mpeg2ISO13818_1_Aux;
    static const std::string AudioISO13818_7_ADTS;
    static const std::string Mpeg4ISO14496_2;
    static const std::string AudioISO14496_3;
    static const std::string ISO14496_1_PesPackets;
    static const std::string ISO14496_1_Sections;
    static const std::string ISO13818_6_DownloadProt;
    static const std::string MetaDataPesPackets;
    static const std::string MetaDataSections;
    static const std::string Mpeg2UserPrivate;
    static const std::string AudioAtscAc3;
    static const std::string ProgramClock;
    static const std::string ProgramMapTable;
    static const std::string Other;

    Stream(const std::string& type, Poco::UInt16 pid);

    void readXml(Poco::XML::Node* pXmlStream);
    void writeXml(Poco::XML::Element* pService);

    std::string getType();
    bool isAudio();
    bool isVideo();
    Poco::UInt16 getPid();
    int getFileDesc();
    void read(Poco::UInt8* buf, int size, int timeout = 0);
    /// timeout in secs, timeout = 0 means unlimited
//    std::istream* getStream();

    void skipToElementaryStreamPacketHeader(Poco::UInt8* skippedBytes, int timeout = 0);
    ElementaryStreamPacket* getElementaryStreamPacket(int timeout = 0);

    static Poco::UInt8 streamTypeFromString(const std::string& val);
    static std::string streamTypeToString(Poco::UInt8 val);

private:
    std::string         _type;
    Poco::UInt16        _pid;

    int                 _fileDesc;
    struct pollfd       _fileDescPoll[1];
//    UnixFileIStream*    _pStream;

    int                 _logSequence;
};


class Multiplex : public Stream
{
    friend class Demux;

public:
    Multiplex();
};


}  // namespace Omm
}  // namespace Dvb

#endif
