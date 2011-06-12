/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <sstream>

#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/DocumentFragment.h>
#include <Poco/XML/XMLWriter.h>

#include "UpnpTypes.h"
#include "UpnpAvLogger.h"
#include "UpnpAv.h"
#include "UpnpAvPrivate.h"
#include "UpnpInternal.h"


namespace Omm {
namespace Av {


const std::string AvTransportArgument::TRANSPORT_STATE_STOPPED = "STOPPED";
const std::string AvTransportArgument::TRANSPORT_STATE_PLAYING = "PLAYING";
const std::string AvTransportArgument::TRANSPORT_STATE_TRANSITIONING = "TRANSITIONING";
const std::string AvTransportArgument::TRANSPORT_STATE_PAUSED_PLAYBACK = "PAUSED_PLAYBACK";
const std::string AvTransportArgument::TRANSPORT_STATE_PAUSED_RECORDING = "PAUSED_RECORDING";
const std::string AvTransportArgument::TRANSPORT_STATE_RECORDING = "RECORDING";
const std::string AvTransportArgument::TRANSPORT_STATE_NO_MEDIA_PRESENT = "NO_MEDIA_PRESENT";
const std::string AvTransportArgument::TRANSPORT_STATUS_OK = "OK";
const std::string AvTransportArgument::TRANSPORT_STATUS_ERROR_OCCURRED = "ERROR_OCCURRED";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_UNKNOWN = "UNKNOWN";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DV = "DV";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_MINI_DV = "MINI_DV";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_VHS = "VHS";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_W_VHS = "W_VHS";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_S_VHS = "S_VHS";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_D_VHS = "D_VHS";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_VHSC = "VHSC";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_VIDEO8 = "VIDEO8";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_HI8 = "HI8";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_CD_ROM = "CD_ROM";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_CD_DA = "CD_DA";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_CD_R = "CD_R";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_CD_RW = "CD_RW";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_VIDEO_CD = "VIDEO_CD";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_SACD = "SACD";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_MD_AUDIO = "MD_AUDIO";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_MD_PICTURE = "MD_PICTURE";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_ROM = "DVD_ROM";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_VIDEO = "DVD_VIDEO";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_R = "DVD_R";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_PLUS_RW = "DVD_PLUS_RW";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_RW = "DVD_RW";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_RAM = "DVD_RAM";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DVD_AUDIO = "DVD_AUDIO";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_DAT = "DAT";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_LD = "LD";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_HDD = "HDD";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_MICRO_MV = "MICRO_MV";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NETWORK = "NETWORK";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NONE = "NONE";
const std::string AvTransportArgument::PLAYBACK_STORAGE_MEDIUM_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_NORMAL = "NORMAL";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_SHUFFLE = "SHUFFLE";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_REPEAT_ONE = "REPEAT_ONE";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_REPEAT_ALL = "REPEAT_ALL";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_RANDOM = "RANDOM";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_DIRECT_1 = "DIRECT_1";
const std::string AvTransportArgument::CURRENT_PLAY_MODE_INTRO = "INTRO";
const std::string AvTransportArgument::TRANSPORT_PLAY_SPEED_1 = "1";
const std::string AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_WRITABLE = "WRITABLE";
const std::string AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_PROTECTED = "PROTECTED";
const std::string AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_NOT_WRITABLE = "NOT_WRITABLE";
const std::string AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_UNKOWN = "UNKOWN";
const std::string AvTransportArgument::RECORD_MEDIUM_WRITE_STATUS_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_0_EP = "0_EP";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_1_LP = "1_LP";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_2_SP = "2_SP";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_0_BASIC = "0_BASIC";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_1_MEDIUM = "1_MEDIUM";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_2_HIGH = "2_HIGH";
const std::string AvTransportArgument::CURRENT_TRACK_DURATION_0 = "00:00:00";
const std::string AvTransportArgument::CURRENT_RECORD_QUALITY_MODE_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::CURRENT_TRACK_META_DATA_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::RELATIVE_TIME_POSITION_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::CURRENT_TRANSPORT_ACTIONS_NOT_IMPLEMENTED = "NOT_IMPLEMENTED";
const std::string AvTransportArgument::SEEK_MODE_TRACK_NR = "TRACK_NR";
const std::string AvTransportArgument::SEEK_MODE_ABS_TIME = "ABS_TIME";
const std::string AvTransportArgument::SEEK_MODE_REL_TIME = "REL_TIME";
const std::string AvTransportArgument::SEEK_MODE_ABS_COUNT = "ABS_COUNT";
const std::string AvTransportArgument::SEEK_MODE_REL_COUNT = "REL_COUNT";
const std::string AvTransportArgument::SEEK_MODE_CHANNEL_FREQ = "CHANNEL_FREQ";
const std::string AvTransportArgument::SEEK_MODE_TAPE_INDEX = "TAPE_INDEX";
const std::string AvTransportArgument::SEEK_MODE_FRAME = "FRAME";
const i4 AvTransportArgument::RELATIVE_COUNTER_POSITION_UNDEFINED = 2147483647;

const std::string AvChannel::MASTER = "Master";
const std::string AvChannel::LF = "LF";
const std::string AvChannel::RF = "RF";
const std::string AvChannel::CF = "CF";
const std::string AvChannel::LFE = "LFE";
const std::string AvChannel::LS = "LS";
const std::string AvChannel::RS = "RS";
const std::string AvChannel::LFC = "LFC";
const std::string AvChannel::RFC = "RFC";
const std::string AvChannel::SD = "SD";
const std::string AvChannel::SL = "SL";
const std::string AvChannel::SR = "SR";
const std::string AvChannel::T = "T";
const std::string AvChannel::B = "B";

const std::string PresetName::FACTORY_DEFAULTS = "FactoryDefaults";
const std::string PresetName::INSTALLATION_DEFAULTS = "InstallationDefaults";


LastChange::LastChange(Service* pService) :
_pService(pService),
_pDoc(0),
_pMessage(0)
{

}


LastChange::~LastChange()
{

}


LastChange::StateVarIterator
LastChange::beginStateVar(int instanceId)
{
    return _stateVars[instanceId].begin();
}


LastChange::StateVarIterator
LastChange::endStateVar(int instanceId)
{
    return _stateVars[instanceId].end();
}


LastChange::StateVarValIterator
LastChange::beginStateVarVal(int instanceId, const std::string& stateVar)
{
    return _stateVars[instanceId][stateVar].begin();
}


LastChange::StateVarValIterator
LastChange::endStateVarVal(int instanceId, const std::string& stateVar)
{
    return _stateVars[instanceId][stateVar].end();
}


void
LastChange::setStateVar(const ui4& InstanceID, const std::string& name, const Variant& val)
{
    _stateVars[InstanceID][name]["val"] = val.getValue();
}


void
LastChange::notify()
{

}


void
LastChange::write()
{
    writeMessageHeader();
    writeMessageData();
    writeMessageClose();
}


void
LastChange::writeMessageHeader()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageHeader()");
    _pDoc = new Poco::XML::Document;

    _pMessage = _pDoc->createElement("Event");

    // FIXME: is this the right way to set the namespaces?
    writeSchemeAttribute();
//    _pMessage->setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
//    _pMessage->setAttribute("xsi:schemaLocation", "urn:schemas-upnp-org:metadata-1-0/RCS/http://www.upnp.org/schemas/av/rcs-event-v1-20060531.xsd");

    _pDoc->appendChild(_pMessage);
}


void
LastChange::writeMessageClose()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageClose() ...");
    Poco::XML::DOMWriter writer;
    writer.setNewLine("\r\n");
    std::stringstream ss;
    writer.writeNode(ss, _pDoc);
    _message = ss.str();
    Log::instance()->upnpav().debug("LastChange::writeMessageClose() writes message: \n" + _message);
}


void
LastChange::writeMessageData()
{
    Log::instance()->upnpav().debug("LastChange::writeMessageData()");

    Poco::XML::Document* pDoc = _pMessage->ownerDocument();


    for (int instanceId = 0; instanceId < _stateVars.size(); ++instanceId) {
        Poco::AutoPtr<Poco::XML::Element> pInstanceId = pDoc->createElement("InstanceID");
        Poco::AutoPtr<Poco::XML::Attr> pInstanceIdVal = pDoc->createAttribute("val");
        pInstanceIdVal->setValue(Poco::NumberFormatter::format(instanceId));
        for (StateVarIterator varIt = beginStateVar(instanceId); varIt != endStateVar(instanceId); ++varIt) {
            std::string varName = (*varIt).first;
            Poco::AutoPtr<Poco::XML::Element> pStateVar = pDoc->createElement(varName);
            for (StateVarValIterator it = beginStateVarVal(instanceId, varName); it != endStateVarVal(instanceId, varName); ++it) {
                Poco::AutoPtr<Poco::XML::Attr> pVal = pDoc->createAttribute((*it).first);
                pVal->setValue((*it).second);
                pStateVar->setAttributeNode(pVal);
            }
            pInstanceId->appendChild(pStateVar);
        }
    }

    Log::instance()->upnpav().debug("LastChange::writeMessageData() finished.");
}


RenderingControlLastChange::RenderingControlLastChange(Service* pService) :
LastChange(pService)
{
}


void
RenderingControlLastChange::writeSchemeAttribute()
{
    _pMessage->setAttribute("xmlns", "urn:schemas-upnp-org:metadata-1-0/RCS/");
}


}  // namespace Omm
}  // namespace Av
