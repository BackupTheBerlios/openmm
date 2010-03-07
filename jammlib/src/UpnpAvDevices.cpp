/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#include "UpnpPrivate.h"
#include "UpnpAvDevices.h"
#include "UpnpAvDescriptions.h"


void
MediaRenderer::actionHandler(Action* pAction)
{
    // the great action dispatcher
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:AVTransport:1") {
        m_pAVTransportImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetAVTransportURI") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
            std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
            m_pAVTransportImpl->SetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
        }
        else if (actionName == "GetMediaInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui4 NrTracks;
            std::string MediaDuration;
            std::string CurrentURI;
            std::string CurrentURIMetaData;
            std::string NextURI;
            std::string NextURIMetaData;
            std::string PlayMedium;
            std::string RecordMedium;
            std::string WriteStatus;
            m_pAVTransportImpl->GetMediaInfo(InstanceID, NrTracks, MediaDuration, CurrentURI, CurrentURIMetaData, NextURI, NextURIMetaData, PlayMedium, RecordMedium, WriteStatus);
            pAction->setArgument<ui4>("NrTracks", NrTracks);
            pAction->setArgument<std::string>("MediaDuration", MediaDuration);
            pAction->setArgument<std::string>("CurrentURI", CurrentURI);
            pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
            pAction->setArgument<std::string>("NextURI", NextURI);
            pAction->setArgument<std::string>("NextURIMetaData", NextURIMetaData);
            pAction->setArgument<std::string>("PlayMedium", PlayMedium);
            pAction->setArgument<std::string>("RecordMedium", RecordMedium);
            pAction->setArgument<std::string>("WriteStatus", WriteStatus);
        }
        else if (actionName == "GetTransportInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string CurrentTransportState;
            std::string CurrentTransportStatus;
            std::string CurrentSpeed;
            m_pAVTransportImpl->GetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
            pAction->setArgument<std::string>("CurrentTransportState", CurrentTransportState);
            pAction->setArgument<std::string>("CurrentTransportStatus", CurrentTransportStatus);
            pAction->setArgument<std::string>("CurrentSpeed", CurrentSpeed);
        }
        else if (actionName == "GetPositionInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui4 Track;
            std::string TrackDuration;
            std::string TrackMetaData;
            std::string TrackURI;
            std::string RelTime;
            std::string AbsTime;
            i4 RelCount;
            i4 AbsCount;
            m_pAVTransportImpl->GetPositionInfo(InstanceID, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
            pAction->setArgument<ui4>("Track", Track);
            pAction->setArgument<std::string>("TrackDuration", TrackDuration);
            pAction->setArgument<std::string>("TrackMetaData", TrackMetaData);
            pAction->setArgument<std::string>("TrackURI", TrackURI);
            pAction->setArgument<std::string>("RelTime", RelTime);
            pAction->setArgument<std::string>("AbsTime", AbsTime);
            pAction->setArgument<i4>("RelCount", RelCount);
            pAction->setArgument<i4>("AbsCount", AbsCount);
        }
        else if (actionName == "GetDeviceCapabilities") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string PlayMedia;
            std::string RecMedia;
            std::string RecQualityModes;
            m_pAVTransportImpl->GetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
            pAction->setArgument<std::string>("PlayMedia", PlayMedia);
            pAction->setArgument<std::string>("RecMedia", RecMedia);
            pAction->setArgument<std::string>("RecQualityModes", RecQualityModes);
        }
        else if (actionName == "GetTransportSettings") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string PlayMode;
            std::string RecQualityMode;
            m_pAVTransportImpl->GetTransportSettings(InstanceID, PlayMode, RecQualityMode);
            pAction->setArgument<std::string>("PlayMode", PlayMode);
            pAction->setArgument<std::string>("RecQualityMode", RecQualityMode);
        }
        else if (actionName == "Stop") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Stop(InstanceID);
        }
        else if (actionName == "Play") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Speed = pAction->getArgument<std::string>("Speed");
            m_pAVTransportImpl->Play(InstanceID, Speed);
        }
        else if (actionName == "Pause") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Pause(InstanceID);
        }
        else if (actionName == "Seek") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Unit = pAction->getArgument<std::string>("Unit");
            std::string Target = pAction->getArgument<std::string>("Target");
            m_pAVTransportImpl->Seek(InstanceID, Unit, Target);
        }
        else if (actionName == "Next") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Next(InstanceID);
        }
        else if (actionName == "Previous") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Previous(InstanceID);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        m_pConnectionManagerImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "GetProtocolInfo") {
            std::string Source;
            std::string Sink;
            m_pConnectionManagerImpl->GetProtocolInfo(Source, Sink);
            pAction->setArgument<std::string>("Source", Source);
            pAction->setArgument<std::string>("Sink", Sink);
        }
        else if (actionName == "ConnectionComplete") {
            i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
            m_pConnectionManagerImpl->ConnectionComplete(ConnectionID);
        }
        else if (actionName == "GetCurrentConnectionIDs") {
            std::string ConnectionIDs;
            m_pConnectionManagerImpl->GetCurrentConnectionIDs(ConnectionIDs);
            pAction->setArgument<std::string>("ConnectionIDs", ConnectionIDs);
        }
        else if (actionName == "GetCurrentConnectionInfo") {
            i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
            i4 RcsID;
            i4 AVTransportID;
            std::string ProtocolInfo;
            std::string PeerConnectionManager;
            i4 PeerConnectionID;
            std::string Direction;
            std::string Status;
            m_pConnectionManagerImpl->GetCurrentConnectionInfo(ConnectionID, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
            pAction->setArgument<i4>("RcsID", RcsID);
            pAction->setArgument<i4>("AVTransportID", AVTransportID);
            pAction->setArgument<std::string>("ProtocolInfo", ProtocolInfo);
            pAction->setArgument<std::string>("PeerConnectionManager", PeerConnectionManager);
            pAction->setArgument<i4>("PeerConnectionID", PeerConnectionID);
            pAction->setArgument<std::string>("Direction", Direction);
            pAction->setArgument<std::string>("Status", Status);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:RenderingControl:1") {
        m_pRenderingControlImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "ListPresets") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string CurrentPresetNameList;
            m_pRenderingControlImpl->ListPresets(InstanceID, CurrentPresetNameList);
            pAction->setArgument<std::string>("CurrentPresetNameList", CurrentPresetNameList);
        }
        else if (actionName == "SelectPreset") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string PresetName = pAction->getArgument<std::string>("PresetName");
            m_pRenderingControlImpl->SelectPreset(InstanceID, PresetName);
        }
        else if (actionName == "GetBrightness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentBrightness;
            m_pRenderingControlImpl->GetBrightness(InstanceID, CurrentBrightness);
            pAction->setArgument<ui2>("CurrentBrightness", CurrentBrightness);
        }
        else if (actionName == "SetBrightness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredBrightness = pAction->getArgument<ui2>("DesiredBrightness");
            m_pRenderingControlImpl->SetBrightness(InstanceID, DesiredBrightness);
        }
        else if (actionName == "GetContrast") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentContrast;
            m_pRenderingControlImpl->GetContrast(InstanceID, CurrentContrast);
            pAction->setArgument<ui2>("CurrentContrast", CurrentContrast);
        }
        else if (actionName == "SetContrast") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredContrast = pAction->getArgument<ui2>("DesiredContrast");
            m_pRenderingControlImpl->SetContrast(InstanceID, DesiredContrast);
        }
        else if (actionName == "GetSharpness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentSharpness;
            m_pRenderingControlImpl->GetSharpness(InstanceID, CurrentSharpness);
            pAction->setArgument<ui2>("CurrentSharpness", CurrentSharpness);
        }
        else if (actionName == "SetSharpness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredSharpness = pAction->getArgument<ui2>("DesiredSharpness");
            m_pRenderingControlImpl->SetSharpness(InstanceID, DesiredSharpness);
        }
        else if (actionName == "GetRedVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentRedVideoGain;
            m_pRenderingControlImpl->GetRedVideoGain(InstanceID, CurrentRedVideoGain);
            pAction->setArgument<ui2>("CurrentRedVideoGain", CurrentRedVideoGain);
        }
        else if (actionName == "SetRedVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredRedVideoGain = pAction->getArgument<ui2>("DesiredRedVideoGain");
            m_pRenderingControlImpl->SetRedVideoGain(InstanceID, DesiredRedVideoGain);
        }
        else if (actionName == "GetGreenVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentGreenVideoGain;
            m_pRenderingControlImpl->GetGreenVideoGain(InstanceID, CurrentGreenVideoGain);
            pAction->setArgument<ui2>("CurrentGreenVideoGain", CurrentGreenVideoGain);
        }
        else if (actionName == "SetGreenVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredGreenVideoGain = pAction->getArgument<ui2>("DesiredGreenVideoGain");
            m_pRenderingControlImpl->SetGreenVideoGain(InstanceID, DesiredGreenVideoGain);
        }
        else if (actionName == "GetBlueVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentBlueVideoGain;
            m_pRenderingControlImpl->GetBlueVideoGain(InstanceID, CurrentBlueVideoGain);
            pAction->setArgument<ui2>("CurrentBlueVideoGain", CurrentBlueVideoGain);
        }
        else if (actionName == "SetBlueVideoGain") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredBlueVideoGain = pAction->getArgument<ui2>("DesiredBlueVideoGain");
            m_pRenderingControlImpl->SetBlueVideoGain(InstanceID, DesiredBlueVideoGain);
        }
        else if (actionName == "GetRedVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentRedVideoBlackLevel;
            m_pRenderingControlImpl->GetRedVideoBlackLevel(InstanceID, CurrentRedVideoBlackLevel);
            pAction->setArgument<ui2>("CurrentRedVideoBlackLevel", CurrentRedVideoBlackLevel);
        }
        else if (actionName == "SetRedVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredRedVideoBlackLevel = pAction->getArgument<ui2>("DesiredRedVideoBlackLevel");
            m_pRenderingControlImpl->SetRedVideoBlackLevel(InstanceID, DesiredRedVideoBlackLevel);
        }
        else if (actionName == "GetGreenVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentGreenVideoBlackLevel;
            m_pRenderingControlImpl->GetGreenVideoBlackLevel(InstanceID, CurrentGreenVideoBlackLevel);
            pAction->setArgument<ui2>("CurrentGreenVideoBlackLevel", CurrentGreenVideoBlackLevel);
        }
        else if (actionName == "SetGreenVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredGreenVideoBlackLevel = pAction->getArgument<ui2>("DesiredGreenVideoBlackLevel");
            m_pRenderingControlImpl->SetGreenVideoBlackLevel(InstanceID, DesiredGreenVideoBlackLevel);
        }
        else if (actionName == "GetBlueVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentBlueVideoBlackLevel;
            m_pRenderingControlImpl->GetBlueVideoBlackLevel(InstanceID, CurrentBlueVideoBlackLevel);
            pAction->setArgument<ui2>("CurrentBlueVideoBlackLevel", CurrentBlueVideoBlackLevel);
        }
        else if (actionName == "SetBlueVideoBlackLevel") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredBlueVideoBlackLevel = pAction->getArgument<ui2>("DesiredBlueVideoBlackLevel");
            m_pRenderingControlImpl->SetBlueVideoBlackLevel(InstanceID, DesiredBlueVideoBlackLevel);
        }
        else if (actionName == "GetColorTemperature ") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 CurrentColorTemperature;
            m_pRenderingControlImpl->GetColorTemperature (InstanceID, CurrentColorTemperature);
            pAction->setArgument<ui2>("CurrentColorTemperature", CurrentColorTemperature);
        }
        else if (actionName == "SetColorTemperature") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui2 DesiredColorTemperature = pAction->getArgument<ui2>("DesiredColorTemperature");
            m_pRenderingControlImpl->SetColorTemperature(InstanceID, DesiredColorTemperature);
        }
        else if (actionName == "GetHorizontalKeystone") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            i2 CurrentHorizontalKeystone;
            m_pRenderingControlImpl->GetHorizontalKeystone(InstanceID, CurrentHorizontalKeystone);
            pAction->setArgument<i2>("CurrentHorizontalKeystone", CurrentHorizontalKeystone);
        }
        else if (actionName == "SetHorizontalKeystone") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            i2 DesiredHorizontalKeystone = pAction->getArgument<i2>("DesiredHorizontalKeystone");
            m_pRenderingControlImpl->SetHorizontalKeystone(InstanceID, DesiredHorizontalKeystone);
        }
        else if (actionName == "GetVerticalKeystone") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            i2 CurrentVerticalKeystone;
            m_pRenderingControlImpl->GetVerticalKeystone(InstanceID, CurrentVerticalKeystone);
            pAction->setArgument<i2>("CurrentVerticalKeystone", CurrentVerticalKeystone);
        }
        else if (actionName == "SetVerticalKeystone") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            i2 DesiredVerticalKeystone = pAction->getArgument<i2>("DesiredVerticalKeystone");
            m_pRenderingControlImpl->SetVerticalKeystone(InstanceID, DesiredVerticalKeystone);
        }
        else if (actionName == "GetMute") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool CurrentMute;
            m_pRenderingControlImpl->GetMute(InstanceID, Channel, CurrentMute);
            pAction->setArgument<bool>("CurrentMute", CurrentMute);
        }
        else if (actionName == "SetMute") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool DesiredMute = pAction->getArgument<bool>("DesiredMute");
            m_pRenderingControlImpl->SetMute(InstanceID, Channel, DesiredMute);
        }
        else if (actionName == "GetVolume") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            ui2 CurrentVolume;
            m_pRenderingControlImpl->GetVolume(InstanceID, Channel, CurrentVolume);
            pAction->setArgument<ui2>("CurrentVolume", CurrentVolume);
        }
        else if (actionName == "SetVolume") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            ui2 DesiredVolume = pAction->getArgument<ui2>("DesiredVolume");
            m_pRenderingControlImpl->SetVolume(InstanceID, Channel, DesiredVolume);
        }
        else if (actionName == "GetVolumeDB") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            i2 CurrentVolume;
            m_pRenderingControlImpl->GetVolumeDB(InstanceID, Channel, CurrentVolume);
            pAction->setArgument<i2>("CurrentVolume", CurrentVolume);
        }
        else if (actionName == "SetVolumeDB") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            i2 DesiredVolume = pAction->getArgument<i2>("DesiredVolume");
            m_pRenderingControlImpl->SetVolumeDB(InstanceID, Channel, DesiredVolume);
        }
        else if (actionName == "GetVolumeDBRange") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            i2 MinValue;
            i2 MaxValue;
            m_pRenderingControlImpl->GetVolumeDBRange(InstanceID, Channel, MinValue, MaxValue);
            pAction->setArgument<i2>("MinValue", MinValue);
            pAction->setArgument<i2>("MaxValue", MaxValue);
        }
        else if (actionName == "GetLoudness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool CurrentLoudness;
            m_pRenderingControlImpl->GetLoudness(InstanceID, Channel, CurrentLoudness);
            pAction->setArgument<bool>("CurrentLoudness", CurrentLoudness);
        }
        else if (actionName == "SetLoudness") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool DesiredLoudness = pAction->getArgument<bool>("DesiredLoudness");
            m_pRenderingControlImpl->SetLoudness(InstanceID, Channel, DesiredLoudness);
        }
    }
}


void
MediaRenderer::initStateVars(const std::string& serviceType, Service* pThis)
{
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        m_pAVTransportImpl->m_pService = pThis;
        m_pAVTransportImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        m_pConnectionManagerImpl->m_pService = pThis;
        m_pConnectionManagerImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:RenderingControl:1") {
        m_pRenderingControlImpl->m_pService = pThis;
        m_pRenderingControlImpl->initStateVars();
    }
}


MediaRenderer::MediaRenderer(RenderingControl* pRenderingControlImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl) :
DeviceRootImplAdapter(),
m_pRenderingControlImpl(pRenderingControlImpl), 
m_pConnectionManagerImpl(pConnectionManagerImpl), 
m_pAVTransportImpl(pAVTransportImpl)
{
    m_descriptions["/MediaRenderer.xml"] = &MediaRenderer::m_deviceDescription;
    m_descriptions["/RenderingControl.xml"] = &RenderingControl::m_description;
    m_descriptions["/ConnectionManager.xml"] = &ConnectionManager::m_description;
    m_descriptions["/AVTransport.xml"] = &AVTransport::m_description;

    StringDescriptionReader descriptionReader(m_descriptions);
    m_pDeviceRoot = descriptionReader.deviceRoot("/MediaRenderer.xml");
    m_pDeviceRoot->setImplAdapter(this);
}


void
AVTransport::_setTransportState(const std::string& val)
{
    m_pService->setStateVar<std::string>("TransportState", val);
}

std::string
AVTransport::_getTransportState()
{
    return m_pService->getStateVar<std::string>("TransportState");
}

void
AVTransport::_setTransportStatus(const std::string& val)
{
    m_pService->setStateVar<std::string>("TransportStatus", val);
}

std::string
AVTransport::_getTransportStatus()
{
    return m_pService->getStateVar<std::string>("TransportStatus");
}

void
AVTransport::_setPlaybackStorageMedium(const std::string& val)
{
    m_pService->setStateVar<std::string>("PlaybackStorageMedium", val);
}

std::string
AVTransport::_getPlaybackStorageMedium()
{
    return m_pService->getStateVar<std::string>("PlaybackStorageMedium");
}

void
AVTransport::_setRecordStorageMedium(const std::string& val)
{
    m_pService->setStateVar<std::string>("RecordStorageMedium", val);
}

std::string
AVTransport::_getRecordStorageMedium()
{
    return m_pService->getStateVar<std::string>("RecordStorageMedium");
}

void
AVTransport::_setPossiblePlaybackStorageMedia(const std::string& val)
{
    m_pService->setStateVar<std::string>("PossiblePlaybackStorageMedia", val);
}

std::string
AVTransport::_getPossiblePlaybackStorageMedia()
{
    return m_pService->getStateVar<std::string>("PossiblePlaybackStorageMedia");
}

void
AVTransport::_setPossibleRecordStorageMedia(const std::string& val)
{
    m_pService->setStateVar<std::string>("PossibleRecordStorageMedia", val);
}

std::string
AVTransport::_getPossibleRecordStorageMedia()
{
    return m_pService->getStateVar<std::string>("PossibleRecordStorageMedia");
}

void
AVTransport::_setCurrentPlayMode(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentPlayMode", val);
}

std::string
AVTransport::_getCurrentPlayMode()
{
    return m_pService->getStateVar<std::string>("CurrentPlayMode");
}

void
AVTransport::_setTransportPlaySpeed(const std::string& val)
{
    m_pService->setStateVar<std::string>("TransportPlaySpeed", val);
}

std::string
AVTransport::_getTransportPlaySpeed()
{
    return m_pService->getStateVar<std::string>("TransportPlaySpeed");
}

void
AVTransport::_setRecordMediumWriteStatus(const std::string& val)
{
    m_pService->setStateVar<std::string>("RecordMediumWriteStatus", val);
}

std::string
AVTransport::_getRecordMediumWriteStatus()
{
    return m_pService->getStateVar<std::string>("RecordMediumWriteStatus");
}

void
AVTransport::_setCurrentRecordQualityMode(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentRecordQualityMode", val);
}

std::string
AVTransport::_getCurrentRecordQualityMode()
{
    return m_pService->getStateVar<std::string>("CurrentRecordQualityMode");
}

void
AVTransport::_setPossibleRecordQualityModes(const std::string& val)
{
    m_pService->setStateVar<std::string>("PossibleRecordQualityModes", val);
}

std::string
AVTransport::_getPossibleRecordQualityModes()
{
    return m_pService->getStateVar<std::string>("PossibleRecordQualityModes");
}

void
AVTransport::_setNumberOfTracks(const ui4& val)
{
    m_pService->setStateVar<ui4>("NumberOfTracks", val);
}

ui4
AVTransport::_getNumberOfTracks()
{
    return m_pService->getStateVar<ui4>("NumberOfTracks");
}

void
AVTransport::_setCurrentTrack(const ui4& val)
{
    m_pService->setStateVar<ui4>("CurrentTrack", val);
}

ui4
AVTransport::_getCurrentTrack()
{
    return m_pService->getStateVar<ui4>("CurrentTrack");
}

void
AVTransport::_setCurrentTrackDuration(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentTrackDuration", val);
}

std::string
AVTransport::_getCurrentTrackDuration()
{
    return m_pService->getStateVar<std::string>("CurrentTrackDuration");
}

void
AVTransport::_setCurrentMediaDuration(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentMediaDuration", val);
}

std::string
AVTransport::_getCurrentMediaDuration()
{
    return m_pService->getStateVar<std::string>("CurrentMediaDuration");
}

void
AVTransport::_setCurrentTrackMetaData(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentTrackMetaData", val);
}

std::string
AVTransport::_getCurrentTrackMetaData()
{
    return m_pService->getStateVar<std::string>("CurrentTrackMetaData");
}

void
AVTransport::_setCurrentTrackURI(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentTrackURI", val);
}

std::string
AVTransport::_getCurrentTrackURI()
{
    return m_pService->getStateVar<std::string>("CurrentTrackURI");
}

void
AVTransport::_setAVTransportURI(const std::string& val)
{
    m_pService->setStateVar<std::string>("AVTransportURI", val);
}

std::string
AVTransport::_getAVTransportURI()
{
    return m_pService->getStateVar<std::string>("AVTransportURI");
}

void
AVTransport::_setAVTransportURIMetaData(const std::string& val)
{
    m_pService->setStateVar<std::string>("AVTransportURIMetaData", val);
}

std::string
AVTransport::_getAVTransportURIMetaData()
{
    return m_pService->getStateVar<std::string>("AVTransportURIMetaData");
}

void
AVTransport::_setNextAVTransportURI(const std::string& val)
{
    m_pService->setStateVar<std::string>("NextAVTransportURI", val);
}

std::string
AVTransport::_getNextAVTransportURI()
{
    return m_pService->getStateVar<std::string>("NextAVTransportURI");
}

void
AVTransport::_setNextAVTransportURIMetaData(const std::string& val)
{
    m_pService->setStateVar<std::string>("NextAVTransportURIMetaData", val);
}

std::string
AVTransport::_getNextAVTransportURIMetaData()
{
    return m_pService->getStateVar<std::string>("NextAVTransportURIMetaData");
}

void
AVTransport::_setRelativeTimePosition(const std::string& val)
{
    m_pService->setStateVar<std::string>("RelativeTimePosition", val);
}

std::string
AVTransport::_getRelativeTimePosition()
{
    return m_pService->getStateVar<std::string>("RelativeTimePosition");
}

void
AVTransport::_setAbsoluteTimePosition(const std::string& val)
{
    m_pService->setStateVar<std::string>("AbsoluteTimePosition", val);
}

std::string
AVTransport::_getAbsoluteTimePosition()
{
    return m_pService->getStateVar<std::string>("AbsoluteTimePosition");
}

void
AVTransport::_setRelativeCounterPosition(const i4& val)
{
    m_pService->setStateVar<i4>("RelativeCounterPosition", val);
}

i4
AVTransport::_getRelativeCounterPosition()
{
    return m_pService->getStateVar<i4>("RelativeCounterPosition");
}

void
AVTransport::_setAbsoluteCounterPosition(const i4& val)
{
    m_pService->setStateVar<i4>("AbsoluteCounterPosition", val);
}

i4
AVTransport::_getAbsoluteCounterPosition()
{
    return m_pService->getStateVar<i4>("AbsoluteCounterPosition");
}

void
AVTransport::_setCurrentTransportActions(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentTransportActions", val);
}

std::string
AVTransport::_getCurrentTransportActions()
{
    return m_pService->getStateVar<std::string>("CurrentTransportActions");
}

void
AVTransport::_setLastChange(const std::string& val)
{
    m_pService->setStateVar<std::string>("LastChange", val);
}

std::string
AVTransport::_getLastChange()
{
    return m_pService->getStateVar<std::string>("LastChange");
}

void
ConnectionManager::_setSourceProtocolInfo(const std::string& val)
{
    m_pService->setStateVar<std::string>("SourceProtocolInfo", val);
}

std::string
ConnectionManager::_getSourceProtocolInfo()
{
    return m_pService->getStateVar<std::string>("SourceProtocolInfo");
}

void
ConnectionManager::_setSinkProtocolInfo(const std::string& val)
{
    m_pService->setStateVar<std::string>("SinkProtocolInfo", val);
}

std::string
ConnectionManager::_getSinkProtocolInfo()
{
    return m_pService->getStateVar<std::string>("SinkProtocolInfo");
}

void
ConnectionManager::_setCurrentConnectionIDs(const std::string& val)
{
    m_pService->setStateVar<std::string>("CurrentConnectionIDs", val);
}

std::string
ConnectionManager::_getCurrentConnectionIDs()
{
    return m_pService->getStateVar<std::string>("CurrentConnectionIDs");
}

void
RenderingControl::_setPresetNameList(const std::string& val)
{
    m_pService->setStateVar<std::string>("PresetNameList", val);
}

std::string
RenderingControl::_getPresetNameList()
{
    return m_pService->getStateVar<std::string>("PresetNameList");
}

void
RenderingControl::_setLastChange(const std::string& val)
{
    m_pService->setStateVar<std::string>("LastChange", val);
}

std::string
RenderingControl::_getLastChange()
{
    return m_pService->getStateVar<std::string>("LastChange");
}

void
RenderingControl::_setBrightness(const ui2& val)
{
    m_pService->setStateVar<ui2>("Brightness", val);
}

ui2
RenderingControl::_getBrightness()
{
    return m_pService->getStateVar<ui2>("Brightness");
}

void
RenderingControl::_setContrast(const ui2& val)
{
    m_pService->setStateVar<ui2>("Contrast", val);
}

ui2
RenderingControl::_getContrast()
{
    return m_pService->getStateVar<ui2>("Contrast");
}

void
RenderingControl::_setSharpness(const ui2& val)
{
    m_pService->setStateVar<ui2>("Sharpness", val);
}

ui2
RenderingControl::_getSharpness()
{
    return m_pService->getStateVar<ui2>("Sharpness");
}

void
RenderingControl::_setRedVideoGain(const ui2& val)
{
    m_pService->setStateVar<ui2>("RedVideoGain", val);
}

ui2
RenderingControl::_getRedVideoGain()
{
    return m_pService->getStateVar<ui2>("RedVideoGain");
}

void
RenderingControl::_setGreenVideoGain(const ui2& val)
{
    m_pService->setStateVar<ui2>("GreenVideoGain", val);
}

ui2
RenderingControl::_getGreenVideoGain()
{
    return m_pService->getStateVar<ui2>("GreenVideoGain");
}

void
RenderingControl::_setBlueVideoGain(const ui2& val)
{
    m_pService->setStateVar<ui2>("BlueVideoGain", val);
}

ui2
RenderingControl::_getBlueVideoGain()
{
    return m_pService->getStateVar<ui2>("BlueVideoGain");
}

void
RenderingControl::_setRedVideoBlackLevel(const ui2& val)
{
    m_pService->setStateVar<ui2>("RedVideoBlackLevel", val);
}

ui2
RenderingControl::_getRedVideoBlackLevel()
{
    return m_pService->getStateVar<ui2>("RedVideoBlackLevel");
}

void
RenderingControl::_setGreenVideoBlackLevel(const ui2& val)
{
    m_pService->setStateVar<ui2>("GreenVideoBlackLevel", val);
}

ui2
RenderingControl::_getGreenVideoBlackLevel()
{
    return m_pService->getStateVar<ui2>("GreenVideoBlackLevel");
}

void
RenderingControl::_setBlueVideoBlackLevel(const ui2& val)
{
    m_pService->setStateVar<ui2>("BlueVideoBlackLevel", val);
}

ui2
RenderingControl::_getBlueVideoBlackLevel()
{
    return m_pService->getStateVar<ui2>("BlueVideoBlackLevel");
}

void
RenderingControl::_setColorTemperature(const ui2& val)
{
    m_pService->setStateVar<ui2>("ColorTemperature", val);
}

ui2
RenderingControl::_getColorTemperature()
{
    return m_pService->getStateVar<ui2>("ColorTemperature");
}

void
RenderingControl::_setHorizontalKeystone(const i2& val)
{
    m_pService->setStateVar<i2>("HorizontalKeystone", val);
}

i2
RenderingControl::_getHorizontalKeystone()
{
    return m_pService->getStateVar<i2>("HorizontalKeystone");
}

void
RenderingControl::_setVerticalKeystone(const i2& val)
{
    m_pService->setStateVar<i2>("VerticalKeystone", val);
}

i2
RenderingControl::_getVerticalKeystone()
{
    return m_pService->getStateVar<i2>("VerticalKeystone");
}

void
RenderingControl::_setMute(const bool& val)
{
    m_pService->setStateVar<bool>("Mute", val);
}

bool
RenderingControl::_getMute()
{
    return m_pService->getStateVar<bool>("Mute");
}

void
RenderingControl::_setVolume(const ui2& val)
{
    m_pService->setStateVar<ui2>("Volume", val);
}

ui2
RenderingControl::_getVolume()
{
    return m_pService->getStateVar<ui2>("Volume");
}

void
RenderingControl::_setVolumeDB(const i2& val)
{
    m_pService->setStateVar<i2>("VolumeDB", val);
}

i2
RenderingControl::_getVolumeDB()
{
    return m_pService->getStateVar<i2>("VolumeDB");
}

void
RenderingControl::_setLoudness(const bool& val)
{
    m_pService->setStateVar<bool>("Loudness", val);
}

bool
RenderingControl::_getLoudness()
{
    return m_pService->getStateVar<bool>("Loudness");
}


void
MediaServer::actionHandler(Action* pAction)
{
    // the great action dispatcher
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:AVTransport:1") {
        m_pAVTransportImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetAVTransportURI") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
            std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
            m_pAVTransportImpl->SetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
        }
        else if (actionName == "GetMediaInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui4 NrTracks;
            std::string MediaDuration;
            std::string CurrentURI;
            std::string CurrentURIMetaData;
            std::string NextURI;
            std::string NextURIMetaData;
            std::string PlayMedium;
            std::string RecordMedium;
            std::string WriteStatus;
            m_pAVTransportImpl->GetMediaInfo(InstanceID, NrTracks, MediaDuration, CurrentURI, CurrentURIMetaData, NextURI, NextURIMetaData, PlayMedium, RecordMedium, WriteStatus);
            pAction->setArgument<ui4>("NrTracks", NrTracks);
            pAction->setArgument<std::string>("MediaDuration", MediaDuration);
            pAction->setArgument<std::string>("CurrentURI", CurrentURI);
            pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
            pAction->setArgument<std::string>("NextURI", NextURI);
            pAction->setArgument<std::string>("NextURIMetaData", NextURIMetaData);
            pAction->setArgument<std::string>("PlayMedium", PlayMedium);
            pAction->setArgument<std::string>("RecordMedium", RecordMedium);
            pAction->setArgument<std::string>("WriteStatus", WriteStatus);
        }
        else if (actionName == "GetTransportInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string CurrentTransportState;
            std::string CurrentTransportStatus;
            std::string CurrentSpeed;
            m_pAVTransportImpl->GetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
            pAction->setArgument<std::string>("CurrentTransportState", CurrentTransportState);
            pAction->setArgument<std::string>("CurrentTransportStatus", CurrentTransportStatus);
            pAction->setArgument<std::string>("CurrentSpeed", CurrentSpeed);
        }
        else if (actionName == "GetPositionInfo") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            ui4 Track;
            std::string TrackDuration;
            std::string TrackMetaData;
            std::string TrackURI;
            std::string RelTime;
            std::string AbsTime;
            i4 RelCount;
            i4 AbsCount;
            m_pAVTransportImpl->GetPositionInfo(InstanceID, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
            pAction->setArgument<ui4>("Track", Track);
            pAction->setArgument<std::string>("TrackDuration", TrackDuration);
            pAction->setArgument<std::string>("TrackMetaData", TrackMetaData);
            pAction->setArgument<std::string>("TrackURI", TrackURI);
            pAction->setArgument<std::string>("RelTime", RelTime);
            pAction->setArgument<std::string>("AbsTime", AbsTime);
            pAction->setArgument<i4>("RelCount", RelCount);
            pAction->setArgument<i4>("AbsCount", AbsCount);
        }
        else if (actionName == "GetDeviceCapabilities") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string PlayMedia;
            std::string RecMedia;
            std::string RecQualityModes;
            m_pAVTransportImpl->GetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
            pAction->setArgument<std::string>("PlayMedia", PlayMedia);
            pAction->setArgument<std::string>("RecMedia", RecMedia);
            pAction->setArgument<std::string>("RecQualityModes", RecQualityModes);
        }
        else if (actionName == "GetTransportSettings") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string PlayMode;
            std::string RecQualityMode;
            m_pAVTransportImpl->GetTransportSettings(InstanceID, PlayMode, RecQualityMode);
            pAction->setArgument<std::string>("PlayMode", PlayMode);
            pAction->setArgument<std::string>("RecQualityMode", RecQualityMode);
        }
        else if (actionName == "Stop") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Stop(InstanceID);
        }
        else if (actionName == "Play") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Speed = pAction->getArgument<std::string>("Speed");
            m_pAVTransportImpl->Play(InstanceID, Speed);
        }
        else if (actionName == "Pause") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Pause(InstanceID);
        }
        else if (actionName == "Seek") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            std::string Unit = pAction->getArgument<std::string>("Unit");
            std::string Target = pAction->getArgument<std::string>("Target");
            m_pAVTransportImpl->Seek(InstanceID, Unit, Target);
        }
        else if (actionName == "Next") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Next(InstanceID);
        }
        else if (actionName == "Previous") {
            ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
            m_pAVTransportImpl->Previous(InstanceID);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        m_pConnectionManagerImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "GetProtocolInfo") {
            std::string Source;
            std::string Sink;
            m_pConnectionManagerImpl->GetProtocolInfo(Source, Sink);
            pAction->setArgument<std::string>("Source", Source);
            pAction->setArgument<std::string>("Sink", Sink);
        }
        else if (actionName == "ConnectionComplete") {
            i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
            m_pConnectionManagerImpl->ConnectionComplete(ConnectionID);
        }
        else if (actionName == "GetCurrentConnectionIDs") {
            std::string ConnectionIDs;
            m_pConnectionManagerImpl->GetCurrentConnectionIDs(ConnectionIDs);
            pAction->setArgument<std::string>("ConnectionIDs", ConnectionIDs);
        }
        else if (actionName == "GetCurrentConnectionInfo") {
            i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
            i4 RcsID;
            i4 AVTransportID;
            std::string ProtocolInfo;
            std::string PeerConnectionManager;
            i4 PeerConnectionID;
            std::string Direction;
            std::string Status;
            m_pConnectionManagerImpl->GetCurrentConnectionInfo(ConnectionID, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
            pAction->setArgument<i4>("RcsID", RcsID);
            pAction->setArgument<i4>("AVTransportID", AVTransportID);
            pAction->setArgument<std::string>("ProtocolInfo", ProtocolInfo);
            pAction->setArgument<std::string>("PeerConnectionManager", PeerConnectionManager);
            pAction->setArgument<i4>("PeerConnectionID", PeerConnectionID);
            pAction->setArgument<std::string>("Direction", Direction);
            pAction->setArgument<std::string>("Status", Status);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:ContentDirectory:1") {
        m_pContentDirectoryImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "GetSearchCapabilities") {
            std::string SearchCaps;
            m_pContentDirectoryImpl->GetSearchCapabilities(SearchCaps);
            pAction->setArgument<std::string>("SearchCaps", SearchCaps);
        }
        else if (actionName == "GetSortCapabilities") {
            std::string SortCaps;
            m_pContentDirectoryImpl->GetSortCapabilities(SortCaps);
            pAction->setArgument<std::string>("SortCaps", SortCaps);
        }
        else if (actionName == "GetSystemUpdateID") {
            ui4 Id;
            m_pContentDirectoryImpl->GetSystemUpdateID(Id);
            pAction->setArgument<ui4>("Id", Id);
        }
        else if (actionName == "Browse") {
            std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
            std::string BrowseFlag = pAction->getArgument<std::string>("BrowseFlag");
            std::string Filter = pAction->getArgument<std::string>("Filter");
            ui4 StartingIndex = pAction->getArgument<ui4>("StartingIndex");
            ui4 RequestedCount = pAction->getArgument<ui4>("RequestedCount");
            std::string SortCriteria = pAction->getArgument<std::string>("SortCriteria");
            std::string Result;
            ui4 NumberReturned;
            ui4 TotalMatches;
            ui4 UpdateID;
            m_pContentDirectoryImpl->Browse(ObjectID, BrowseFlag, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
            pAction->setArgument<std::string>("Result", Result);
            pAction->setArgument<ui4>("NumberReturned", NumberReturned);
            pAction->setArgument<ui4>("TotalMatches", TotalMatches);
            pAction->setArgument<ui4>("UpdateID", UpdateID);
        }
        else if (actionName == "Search") {
            std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
            std::string SearchCriteria = pAction->getArgument<std::string>("SearchCriteria");
            std::string Filter = pAction->getArgument<std::string>("Filter");
            ui4 StartingIndex = pAction->getArgument<ui4>("StartingIndex");
            ui4 RequestedCount = pAction->getArgument<ui4>("RequestedCount");
            std::string SortCriteria = pAction->getArgument<std::string>("SortCriteria");
            std::string Result;
            ui4 NumberReturned;
            ui4 TotalMatches;
            ui4 UpdateID;
            m_pContentDirectoryImpl->Search(ContainerID, SearchCriteria, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
            pAction->setArgument<std::string>("Result", Result);
            pAction->setArgument<ui4>("NumberReturned", NumberReturned);
            pAction->setArgument<ui4>("TotalMatches", TotalMatches);
            pAction->setArgument<ui4>("UpdateID", UpdateID);
        }
        else if (actionName == "CreateObject") {
            std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
            std::string Elements = pAction->getArgument<std::string>("Elements");
            std::string ObjectID;
            std::string Result;
            m_pContentDirectoryImpl->CreateObject(ContainerID, Elements, ObjectID, Result);
            pAction->setArgument<std::string>("ObjectID", ObjectID);
            pAction->setArgument<std::string>("Result", Result);
        }
        else if (actionName == "DestroyObject") {
            std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
            m_pContentDirectoryImpl->DestroyObject(ObjectID);
        }
        else if (actionName == "UpdateObject") {
            std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
            std::string CurrentTagValue = pAction->getArgument<std::string>("CurrentTagValue");
            std::string NewTagValue = pAction->getArgument<std::string>("NewTagValue");
            m_pContentDirectoryImpl->UpdateObject(ObjectID, CurrentTagValue, NewTagValue);
        }
        else if (actionName == "ImportResource") {
            uri SourceURI = pAction->getArgument<uri>("SourceURI");
            uri DestinationURI = pAction->getArgument<uri>("DestinationURI");
            ui4 TransferID;
            m_pContentDirectoryImpl->ImportResource(SourceURI, DestinationURI, TransferID);
            pAction->setArgument<ui4>("TransferID", TransferID);
        }
        else if (actionName == "GetTransferProgress") {
            ui4 TransferID = pAction->getArgument<ui4>("TransferID");
            std::string TransferStatus;
            std::string TransferLength;
            std::string TransferTotal;
            m_pContentDirectoryImpl->GetTransferProgress(TransferID, TransferStatus, TransferLength, TransferTotal);
            pAction->setArgument<std::string>("TransferStatus", TransferStatus);
            pAction->setArgument<std::string>("TransferLength", TransferLength);
            pAction->setArgument<std::string>("TransferTotal", TransferTotal);
        }
        else if (actionName == "DeleteResource") {
            uri ResourceURI = pAction->getArgument<uri>("ResourceURI");
            m_pContentDirectoryImpl->DeleteResource(ResourceURI);
        }
        else if (actionName == "CreateReference") {
            std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
            std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
            std::string NewID;
            m_pContentDirectoryImpl->CreateReference(ContainerID, ObjectID, NewID);
            pAction->setArgument<std::string>("NewID", NewID);
        }
    }
}


void
MediaServer::initStateVars(const std::string& serviceType, Service* pThis)
{
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        m_pAVTransportImpl->m_pService = pThis;
        m_pAVTransportImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        m_pConnectionManagerImpl->m_pService = pThis;
        m_pConnectionManagerImpl->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ContentDirectory:1") {
        m_pContentDirectoryImpl->m_pService = pThis;
        m_pContentDirectoryImpl->initStateVars();
    }
}


MediaServer::MediaServer(ContentDirectory* pContentDirectoryImpl, ConnectionManager* pConnectionManagerImpl, AVTransport* pAVTransportImpl) :
DeviceRootImplAdapter(),
m_pContentDirectoryImpl(pContentDirectoryImpl), 
m_pConnectionManagerImpl(pConnectionManagerImpl), 
m_pAVTransportImpl(pAVTransportImpl)
{
    m_descriptions["/MediaServer.xml"] = &MediaServer::m_deviceDescription;
    m_descriptions["/ContentDirectory.xml"] = &ContentDirectory::m_description;
    m_descriptions["/ConnectionManager.xml"] = &ConnectionManager::m_description;
    m_descriptions["/AVTransport.xml"] = &AVTransport::m_description;

    StringDescriptionReader descriptionReader(m_descriptions);
    m_pDeviceRoot = descriptionReader.deviceRoot("/MediaServer.xml");
    m_pDeviceRoot->setImplAdapter(this);
}


void
ContentDirectory::_setTransferIDs(const std::string& val)
{
    m_pService->setStateVar<std::string>("TransferIDs", val);
}

std::string
ContentDirectory::_getTransferIDs()
{
    return m_pService->getStateVar<std::string>("TransferIDs");
}

void
ContentDirectory::_setSearchCapabilities(const std::string& val)
{
    m_pService->setStateVar<std::string>("SearchCapabilities", val);
}

std::string
ContentDirectory::_getSearchCapabilities()
{
    return m_pService->getStateVar<std::string>("SearchCapabilities");
}

void
ContentDirectory::_setSortCapabilities(const std::string& val)
{
    m_pService->setStateVar<std::string>("SortCapabilities", val);
}

std::string
ContentDirectory::_getSortCapabilities()
{
    return m_pService->getStateVar<std::string>("SortCapabilities");
}

void
ContentDirectory::_setSystemUpdateID(const ui4& val)
{
    m_pService->setStateVar<ui4>("SystemUpdateID", val);
}

ui4
ContentDirectory::_getSystemUpdateID()
{
    return m_pService->getStateVar<ui4>("SystemUpdateID");
}

void
ContentDirectory::_setContainerUpdateIDs(const std::string& val)
{
    m_pService->setStateVar<std::string>("ContainerUpdateIDs", val);
}

std::string
ContentDirectory::_getContainerUpdateIDs()
{
    return m_pService->getStateVar<std::string>("ContainerUpdateIDs");
}


