#include <jamm/upnp.h>
#include "MediaRenderer.h"
#include "MediaRendererDescriptions.h"


void
MediaRenderer::actionHandler(Action* pAction)
{
    // the great action dispatcher
    if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:AVTransport:1") {
        m_pAVTransportImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "SetAVTransportURI") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
            std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
            m_pAVTransportImpl->SetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
        }
        else if (actionName == "GetMediaInfo") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            Jamm::ui4 NrTracks;
            std::string MediaDuration;
            std::string CurrentURI;
            std::string CurrentURIMetaData;
            std::string NextURI;
            std::string NextURIMetaData;
            std::string PlayMedium;
            std::string RecordMedium;
            std::string WriteStatus;
            m_pAVTransportImpl->GetMediaInfo(InstanceID, NrTracks, MediaDuration, CurrentURI, CurrentURIMetaData, NextURI, NextURIMetaData, PlayMedium, RecordMedium, WriteStatus);
            pAction->setArgument<Jamm::ui4>("NrTracks", NrTracks);
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
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string CurrentTransportState;
            std::string CurrentTransportStatus;
            std::string CurrentSpeed;
            m_pAVTransportImpl->GetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
            pAction->setArgument<std::string>("CurrentTransportState", CurrentTransportState);
            pAction->setArgument<std::string>("CurrentTransportStatus", CurrentTransportStatus);
            pAction->setArgument<std::string>("CurrentSpeed", CurrentSpeed);
        }
        else if (actionName == "GetPositionInfo") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            Jamm::ui4 Track;
            std::string TrackDuration;
            std::string TrackMetaData;
            std::string TrackURI;
            std::string RelTime;
            std::string AbsTime;
            Jamm::i4 RelCount;
            Jamm::i4 AbsCount;
            m_pAVTransportImpl->GetPositionInfo(InstanceID, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
            pAction->setArgument<Jamm::ui4>("Track", Track);
            pAction->setArgument<std::string>("TrackDuration", TrackDuration);
            pAction->setArgument<std::string>("TrackMetaData", TrackMetaData);
            pAction->setArgument<std::string>("TrackURI", TrackURI);
            pAction->setArgument<std::string>("RelTime", RelTime);
            pAction->setArgument<std::string>("AbsTime", AbsTime);
            pAction->setArgument<Jamm::i4>("RelCount", RelCount);
            pAction->setArgument<Jamm::i4>("AbsCount", AbsCount);
        }
        else if (actionName == "GetDeviceCapabilities") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string PlayMedia;
            std::string RecMedia;
            std::string RecQualityModes;
            m_pAVTransportImpl->GetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
            pAction->setArgument<std::string>("PlayMedia", PlayMedia);
            pAction->setArgument<std::string>("RecMedia", RecMedia);
            pAction->setArgument<std::string>("RecQualityModes", RecQualityModes);
        }
        else if (actionName == "GetTransportSettings") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string PlayMode;
            std::string RecQualityMode;
            m_pAVTransportImpl->GetTransportSettings(InstanceID, PlayMode, RecQualityMode);
            pAction->setArgument<std::string>("PlayMode", PlayMode);
            pAction->setArgument<std::string>("RecQualityMode", RecQualityMode);
        }
        else if (actionName == "Stop") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            m_pAVTransportImpl->Stop(InstanceID);
        }
        else if (actionName == "Play") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Speed = pAction->getArgument<std::string>("Speed");
            m_pAVTransportImpl->Play(InstanceID, Speed);
        }
        else if (actionName == "Pause") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            m_pAVTransportImpl->Pause(InstanceID);
        }
        else if (actionName == "Seek") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Unit = pAction->getArgument<std::string>("Unit");
            std::string Target = pAction->getArgument<std::string>("Target");
            m_pAVTransportImpl->Seek(InstanceID, Unit, Target);
        }
        else if (actionName == "Next") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            m_pAVTransportImpl->Next(InstanceID);
        }
        else if (actionName == "Previous") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
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
        else if (actionName == "GetCurrentConnectionIDs") {
            std::string ConnectionIDs;
            m_pConnectionManagerImpl->GetCurrentConnectionIDs(ConnectionIDs);
            pAction->setArgument<std::string>("ConnectionIDs", ConnectionIDs);
        }
        else if (actionName == "GetCurrentConnectionInfo") {
            Jamm::i4 ConnectionID = pAction->getArgument<Jamm::i4>("ConnectionID");
            Jamm::i4 RcsID;
            Jamm::i4 AVTransportID;
            std::string ProtocolInfo;
            std::string PeerConnectionManager;
            Jamm::i4 PeerConnectionID;
            std::string Direction;
            std::string Status;
            m_pConnectionManagerImpl->GetCurrentConnectionInfo(ConnectionID, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
            pAction->setArgument<Jamm::i4>("RcsID", RcsID);
            pAction->setArgument<Jamm::i4>("AVTransportID", AVTransportID);
            pAction->setArgument<std::string>("ProtocolInfo", ProtocolInfo);
            pAction->setArgument<std::string>("PeerConnectionManager", PeerConnectionManager);
            pAction->setArgument<Jamm::i4>("PeerConnectionID", PeerConnectionID);
            pAction->setArgument<std::string>("Direction", Direction);
            pAction->setArgument<std::string>("Status", Status);
        }
    }
    else if (pAction->getService()->getServiceType() == "urn:schemas-upnp-org:service:RenderingControl:1") {
        m_pRenderingControlImpl->m_pService = pAction->getService();
        std::string actionName = pAction->getName();

        if (actionName == "ListPresets") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string CurrentPresetNameList;
            m_pRenderingControlImpl->ListPresets(InstanceID, CurrentPresetNameList);
            pAction->setArgument<std::string>("CurrentPresetNameList", CurrentPresetNameList);
        }
        else if (actionName == "SelectPreset") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string PresetName = pAction->getArgument<std::string>("PresetName");
            m_pRenderingControlImpl->SelectPreset(InstanceID, PresetName);
        }
        else if (actionName == "GetMute") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool CurrentMute;
            m_pRenderingControlImpl->GetMute(InstanceID, Channel, CurrentMute);
            pAction->setArgument<bool>("CurrentMute", CurrentMute);
        }
        else if (actionName == "SetMute") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            bool DesiredMute = pAction->getArgument<bool>("DesiredMute");
            m_pRenderingControlImpl->SetMute(InstanceID, Channel, DesiredMute);
        }
        else if (actionName == "GetVolume") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            Jamm::ui2 CurrentVolume;
            m_pRenderingControlImpl->GetVolume(InstanceID, Channel, CurrentVolume);
            pAction->setArgument<Jamm::ui2>("CurrentVolume", CurrentVolume);
        }
        else if (actionName == "SetVolume") {
            Jamm::ui4 InstanceID = pAction->getArgument<Jamm::ui4>("InstanceID");
            std::string Channel = pAction->getArgument<std::string>("Channel");
            Jamm::ui2 DesiredVolume = pAction->getArgument<Jamm::ui2>("DesiredVolume");
            m_pRenderingControlImpl->SetVolume(InstanceID, Channel, DesiredVolume);
        }
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

    Jamm::StringDescriptionReader descriptionReader(m_descriptions, "/MediaRenderer.xml");
    m_pDeviceRoot = descriptionReader.deviceRoot();
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
AVTransport::_setNumberOfTracks(const Jamm::ui4& val)
{
    m_pService->setStateVar<Jamm::ui4>("NumberOfTracks", val);
}

Jamm::ui4
AVTransport::_getNumberOfTracks()
{
    return m_pService->getStateVar<Jamm::ui4>("NumberOfTracks");
}

void
AVTransport::_setCurrentTrack(const Jamm::ui4& val)
{
    m_pService->setStateVar<Jamm::ui4>("CurrentTrack", val);
}

Jamm::ui4
AVTransport::_getCurrentTrack()
{
    return m_pService->getStateVar<Jamm::ui4>("CurrentTrack");
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
AVTransport::_setRelativeCounterPosition(const Jamm::i4& val)
{
    m_pService->setStateVar<Jamm::i4>("RelativeCounterPosition", val);
}

Jamm::i4
AVTransport::_getRelativeCounterPosition()
{
    return m_pService->getStateVar<Jamm::i4>("RelativeCounterPosition");
}

void
AVTransport::_setAbsoluteCounterPosition(const Jamm::i4& val)
{
    m_pService->setStateVar<Jamm::i4>("AbsoluteCounterPosition", val);
}

Jamm::i4
AVTransport::_getAbsoluteCounterPosition()
{
    return m_pService->getStateVar<Jamm::i4>("AbsoluteCounterPosition");
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
RenderingControl::_setBrightness(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("Brightness", val);
}

Jamm::ui2
RenderingControl::_getBrightness()
{
    return m_pService->getStateVar<Jamm::ui2>("Brightness");
}

void
RenderingControl::_setContrast(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("Contrast", val);
}

Jamm::ui2
RenderingControl::_getContrast()
{
    return m_pService->getStateVar<Jamm::ui2>("Contrast");
}

void
RenderingControl::_setSharpness(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("Sharpness", val);
}

Jamm::ui2
RenderingControl::_getSharpness()
{
    return m_pService->getStateVar<Jamm::ui2>("Sharpness");
}

void
RenderingControl::_setRedVideoGain(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("RedVideoGain", val);
}

Jamm::ui2
RenderingControl::_getRedVideoGain()
{
    return m_pService->getStateVar<Jamm::ui2>("RedVideoGain");
}

void
RenderingControl::_setGreenVideoGain(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("GreenVideoGain", val);
}

Jamm::ui2
RenderingControl::_getGreenVideoGain()
{
    return m_pService->getStateVar<Jamm::ui2>("GreenVideoGain");
}

void
RenderingControl::_setBlueVideoGain(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("BlueVideoGain", val);
}

Jamm::ui2
RenderingControl::_getBlueVideoGain()
{
    return m_pService->getStateVar<Jamm::ui2>("BlueVideoGain");
}

void
RenderingControl::_setRedVideoBlackLevel(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("RedVideoBlackLevel", val);
}

Jamm::ui2
RenderingControl::_getRedVideoBlackLevel()
{
    return m_pService->getStateVar<Jamm::ui2>("RedVideoBlackLevel");
}

void
RenderingControl::_setGreenVideoBlackLevel(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("GreenVideoBlackLevel", val);
}

Jamm::ui2
RenderingControl::_getGreenVideoBlackLevel()
{
    return m_pService->getStateVar<Jamm::ui2>("GreenVideoBlackLevel");
}

void
RenderingControl::_setBlueVideoBlackLevel(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("BlueVideoBlackLevel", val);
}

Jamm::ui2
RenderingControl::_getBlueVideoBlackLevel()
{
    return m_pService->getStateVar<Jamm::ui2>("BlueVideoBlackLevel");
}

void
RenderingControl::_setColorTemperature(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("ColorTemperature", val);
}

Jamm::ui2
RenderingControl::_getColorTemperature()
{
    return m_pService->getStateVar<Jamm::ui2>("ColorTemperature");
}

void
RenderingControl::_setHorizontalKeystone(const Jamm::i2& val)
{
    m_pService->setStateVar<Jamm::i2>("HorizontalKeystone", val);
}

Jamm::i2
RenderingControl::_getHorizontalKeystone()
{
    return m_pService->getStateVar<Jamm::i2>("HorizontalKeystone");
}

void
RenderingControl::_setVerticalKeystone(const Jamm::i2& val)
{
    m_pService->setStateVar<Jamm::i2>("VerticalKeystone", val);
}

Jamm::i2
RenderingControl::_getVerticalKeystone()
{
    return m_pService->getStateVar<Jamm::i2>("VerticalKeystone");
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
RenderingControl::_setVolume(const Jamm::ui2& val)
{
    m_pService->setStateVar<Jamm::ui2>("Volume", val);
}

Jamm::ui2
RenderingControl::_getVolume()
{
    return m_pService->getStateVar<Jamm::ui2>("Volume");
}

void
RenderingControl::_setVolumeDB(const Jamm::i2& val)
{
    m_pService->setStateVar<Jamm::i2>("VolumeDB", val);
}

Jamm::i2
RenderingControl::_getVolumeDB()
{
    return m_pService->getStateVar<Jamm::i2>("VolumeDB");
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


