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

#ifndef Service_INCLUDED
#define Service_INCLUDED

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

#include "../AvStream.h"

namespace Omm {
namespace Dvb {

class Transponder;
class Stream;
class PatSection;
class TransportStreamPacket;

class Service
{
    friend class Transponder;
    friend class Frontend;
//    friend class Device;
    friend class Demux;

public:
    static const std::string TypeDigitalTelevision;
    static const std::string TypeDigitalRadioSound;
    static const std::string TypeTeletext;
    static const std::string TypeNvodReference;
    static const std::string TypeNodTimeShifted;
    static const std::string TypeMosaic;
    static const std::string TypeFmRadio;
    static const std::string TypeDvbSrm;
    static const std::string TypeAdvancedCodecDigitalRadioSound;
    static const std::string TypeAdvancedCodecMosaic;
    static const std::string TypeDataBroadcastService;
    static const std::string TypeRcsMap;
    static const std::string TypeRcsFls;
    static const std::string TypeDvbMhp;
    static const std::string TypeMpeg2HdDigitalTelevision;
    static const std::string TypeAdvancedCodecSdDigitalTelevision;
    static const std::string TypeAdvancedCodecSdNvodTimeShifted;
    static const std::string TypeAdvancedCodecSdNvodReference;
    static const std::string TypeAdvancedCodecHdDigitalTelevision;
    static const std::string TypeAdvancedCodecHdNvodTimeShifted;
    static const std::string TypeAdvancedCodecHdNvodReference;
    static const std::string TypeAdvancedCodecFrameCompatiblePlanoStereoscopicHdTelevision;
    static const std::string TypeAdvancedCodecFrameCompatiblePlanoStereoscopicTimeShifted;
    static const std::string TypeAdvancedCodecFrameCompatiblePlanoStereoscopicReference;

    static const unsigned int InvalidPcrPid;

    static const std::string StatusUndefined;
    static const std::string StatusNotRunning;
    static const std::string StatusStartsShortly;
    static const std::string StatusPausing;
    static const std::string StatusRunning;
    static const std::string StatusOffAir;

    Service(Transponder* pTransponder, const std::string& name, unsigned int sid, unsigned int pmtid);
    ~Service();

    void addStream(Stream* pStream);
    void readXml(Poco::XML::Node* pXmlService);
    void writeXml(Poco::XML::Element* pTransponder);

    std::string getType();
    bool isAudio();
    bool isSdVideo();
    bool isHdVideo();
    std::string getName();
    std::string getStatus();
    bool getScrambled();
    Transponder* getTransponder();
    Stream* getFirstAudioStream();
    Stream* getFirstVideoStream();
    bool hasPacketIdentifier(Poco::UInt16 pid);
    std::istream* getStream();

    static std::string typeToString(Poco::UInt8 status);
    static std::string statusToString(Poco::UInt8 status);

private:
    Transponder*                _pTransponder;
    std::string                 _type;
    std::string                 _providerName;
    std::string                 _name;
    unsigned int                _sid;
    unsigned int                _pmtPid;
    unsigned int                _pcrPid;
    std::string                 _status;
    bool                        _scrambled;

    std::vector<Stream*>        _streams;

    std::istream*               _pOutStream;
    AvStream::ByteQueue         _byteQueue;
    PatSection*                 _pPat;
    TransportStreamPacket*      _pTsPacket;
};

}  // namespace Omm
}  // namespace Dvb

#endif
