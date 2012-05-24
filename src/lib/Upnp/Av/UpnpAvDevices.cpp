/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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

#include "UpnpAvDevices.h"


namespace Omm {
namespace Av {


void
DevMediaRenderer::actionHandler(Action* pAction)
{
    std::string serviceType = pAction->getService()->getServiceType();
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        _pDevAVTransport->actionHandler(pAction);
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        _pDevConnectionManager->actionHandler(pAction);
    }
    else if (serviceType == "urn:schemas-upnp-org:service:RenderingControl:1") {
        _pDevRenderingControl->actionHandler(pAction);
    }
}


void
DevMediaRenderer::initStateVars(Service* pService)
{
    std::string serviceType = pService->getServiceType();
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        _pDevAVTransport->_pService = pService;
        _pDevAVTransport->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        _pDevConnectionManager->_pService = pService;
        _pDevConnectionManager->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:RenderingControl:1") {
        _pDevRenderingControl->_pService = pService;
        _pDevRenderingControl->initStateVars();
    }
}


DevMediaRenderer::DevMediaRenderer(DevRenderingControl* pRenderingControl, DevConnectionManager* pConnectionManager, DevAVTransport* pAVTransport) :
_pDevRenderingControl(pRenderingControl),
_pDevConnectionManager(pConnectionManager),
_pDevAVTransport(pAVTransport)
{
}


DevMediaRenderer::~DevMediaRenderer()
{
    delete _pDevRenderingControl;
    delete _pDevConnectionManager;
    delete _pDevAVTransport;
}


void
DevMediaServer::actionHandler(Action* pAction)
{
    std::string serviceType = pAction->getService()->getServiceType();
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        _pDevAVTransport->actionHandler(pAction);
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        _pDevConnectionManager->actionHandler(pAction);
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ContentDirectory:1") {
        _pDevContentDirectory->actionHandler(pAction);
    }
}


void
DevMediaServer::initStateVars(Service* pService)
{
    std::string serviceType = pService->getServiceType();
    if (serviceType == "urn:schemas-upnp-org:service:AVTransport:1") {
        _pDevAVTransport->_pService = pService;
        _pDevAVTransport->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ConnectionManager:1") {
        _pDevConnectionManager->_pService = pService;
        _pDevConnectionManager->initStateVars();
    }
    else if (serviceType == "urn:schemas-upnp-org:service:ContentDirectory:1") {
        _pDevContentDirectory->_pService = pService;
        _pDevContentDirectory->initStateVars();
    }
}


DevMediaServer::DevMediaServer(DevContentDirectory* pContentDirectory, DevConnectionManager* pConnectionManager, DevAVTransport* pAVTransport) :
_pDevContentDirectory(pContentDirectory),
_pDevConnectionManager(pConnectionManager),
_pDevAVTransport(pAVTransport)
{
}


DevMediaServer::~DevMediaServer()
{
    delete _pDevContentDirectory;
    delete _pDevConnectionManager;
    delete _pDevAVTransport;
}


void
DevAVTransport::actionHandler(Action* pAction)
{
    std::string actionName = pAction->getName();

    if (actionName == "SetAVTransportURI") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
        std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
        SetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
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
        GetMediaInfo(InstanceID, NrTracks, MediaDuration, CurrentURI, CurrentURIMetaData, NextURI, NextURIMetaData, PlayMedium, RecordMedium, WriteStatus);
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
        GetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
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
        GetPositionInfo(InstanceID, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
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
        GetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
        pAction->setArgument<std::string>("PlayMedia", PlayMedia);
        pAction->setArgument<std::string>("RecMedia", RecMedia);
        pAction->setArgument<std::string>("RecQualityModes", RecQualityModes);
    }
    else if (actionName == "GetTransportSettings") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string PlayMode;
        std::string RecQualityMode;
        GetTransportSettings(InstanceID, PlayMode, RecQualityMode);
        pAction->setArgument<std::string>("PlayMode", PlayMode);
        pAction->setArgument<std::string>("RecQualityMode", RecQualityMode);
    }
    else if (actionName == "Stop") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        Stop(InstanceID);
    }
    else if (actionName == "Play") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Speed = pAction->getArgument<std::string>("Speed");
        Play(InstanceID, Speed);
    }
    else if (actionName == "Pause") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        Pause(InstanceID);
    }
    else if (actionName == "Seek") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Unit = pAction->getArgument<std::string>("Unit");
        std::string Target = pAction->getArgument<std::string>("Target");
        Seek(InstanceID, Unit, Target);
    }
    else if (actionName == "Next") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        Next(InstanceID);
    }
    else if (actionName == "Previous") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        Previous(InstanceID);
    }
}


void
DevAVTransport::_setTransportState(const std::string& val)
{
    _pService->setStateVar<std::string>("TransportState", val);
}


std::string
DevAVTransport::_getTransportState()
{
    return _pService->getStateVar<std::string>("TransportState");
}


void
DevAVTransport::_setTransportStatus(const std::string& val)
{
    _pService->setStateVar<std::string>("TransportStatus", val);
}


std::string
DevAVTransport::_getTransportStatus()
{
    return _pService->getStateVar<std::string>("TransportStatus");
}


void
DevAVTransport::_setPlaybackStorageMedium(const std::string& val)
{
    _pService->setStateVar<std::string>("PlaybackStorageMedium", val);
}


std::string
DevAVTransport::_getPlaybackStorageMedium()
{
    return _pService->getStateVar<std::string>("PlaybackStorageMedium");
}


void
DevAVTransport::_setRecordStorageMedium(const std::string& val)
{
    _pService->setStateVar<std::string>("RecordStorageMedium", val);
}


std::string
DevAVTransport::_getRecordStorageMedium()
{
    return _pService->getStateVar<std::string>("RecordStorageMedium");
}


void
DevAVTransport::_setPossiblePlaybackStorageMedia(const std::string& val)
{
    _pService->setStateVar<std::string>("PossiblePlaybackStorageMedia", val);
}


std::string
DevAVTransport::_getPossiblePlaybackStorageMedia()
{
    return _pService->getStateVar<std::string>("PossiblePlaybackStorageMedia");
}


void
DevAVTransport::_setPossibleRecordStorageMedia(const std::string& val)
{
    _pService->setStateVar<std::string>("PossibleRecordStorageMedia", val);
}


std::string
DevAVTransport::_getPossibleRecordStorageMedia()
{
    return _pService->getStateVar<std::string>("PossibleRecordStorageMedia");
}


void
DevAVTransport::_setCurrentPlayMode(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentPlayMode", val);
}


std::string
DevAVTransport::_getCurrentPlayMode()
{
    return _pService->getStateVar<std::string>("CurrentPlayMode");
}


void
DevAVTransport::_setTransportPlaySpeed(const std::string& val)
{
    _pService->setStateVar<std::string>("TransportPlaySpeed", val);
}


std::string
DevAVTransport::_getTransportPlaySpeed()
{
    return _pService->getStateVar<std::string>("TransportPlaySpeed");
}


void
DevAVTransport::_setRecordMediumWriteStatus(const std::string& val)
{
    _pService->setStateVar<std::string>("RecordMediumWriteStatus", val);
}


std::string
DevAVTransport::_getRecordMediumWriteStatus()
{
    return _pService->getStateVar<std::string>("RecordMediumWriteStatus");
}


void
DevAVTransport::_setCurrentRecordQualityMode(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentRecordQualityMode", val);
}


std::string
DevAVTransport::_getCurrentRecordQualityMode()
{
    return _pService->getStateVar<std::string>("CurrentRecordQualityMode");
}


void
DevAVTransport::_setPossibleRecordQualityModes(const std::string& val)
{
    _pService->setStateVar<std::string>("PossibleRecordQualityModes", val);
}


std::string
DevAVTransport::_getPossibleRecordQualityModes()
{
    return _pService->getStateVar<std::string>("PossibleRecordQualityModes");
}


void
DevAVTransport::_setNumberOfTracks(const ui4& val)
{
    _pService->setStateVar<ui4>("NumberOfTracks", val);
}


ui4
DevAVTransport::_getNumberOfTracks()
{
    return _pService->getStateVar<ui4>("NumberOfTracks");
}


void
DevAVTransport::_setCurrentTrack(const ui4& val)
{
    _pService->setStateVar<ui4>("CurrentTrack", val);
}


ui4
DevAVTransport::_getCurrentTrack()
{
    return _pService->getStateVar<ui4>("CurrentTrack");
}


void
DevAVTransport::_setCurrentTrackDuration(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentTrackDuration", val);
}


std::string
DevAVTransport::_getCurrentTrackDuration()
{
    return _pService->getStateVar<std::string>("CurrentTrackDuration");
}


void
DevAVTransport::_setCurrentMediaDuration(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentMediaDuration", val);
}


std::string
DevAVTransport::_getCurrentMediaDuration()
{
    return _pService->getStateVar<std::string>("CurrentMediaDuration");
}


void
DevAVTransport::_setCurrentTrackMetaData(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentTrackMetaData", val);
}


std::string
DevAVTransport::_getCurrentTrackMetaData()
{
    return _pService->getStateVar<std::string>("CurrentTrackMetaData");
}


void
DevAVTransport::_setCurrentTrackURI(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentTrackURI", val);
}


std::string
DevAVTransport::_getCurrentTrackURI()
{
    return _pService->getStateVar<std::string>("CurrentTrackURI");
}


void
DevAVTransport::_setAVTransportURI(const std::string& val)
{
    _pService->setStateVar<std::string>("AVTransportURI", val);
}


std::string
DevAVTransport::_getAVTransportURI()
{
    return _pService->getStateVar<std::string>("AVTransportURI");
}


void
DevAVTransport::_setAVTransportURIMetaData(const std::string& val)
{
    _pService->setStateVar<std::string>("AVTransportURIMetaData", val);
}


std::string
DevAVTransport::_getAVTransportURIMetaData()
{
    return _pService->getStateVar<std::string>("AVTransportURIMetaData");
}


void
DevAVTransport::_setNextAVTransportURI(const std::string& val)
{
    _pService->setStateVar<std::string>("NextAVTransportURI", val);
}


std::string
DevAVTransport::_getNextAVTransportURI()
{
    return _pService->getStateVar<std::string>("NextAVTransportURI");
}


void
DevAVTransport::_setNextAVTransportURIMetaData(const std::string& val)
{
    _pService->setStateVar<std::string>("NextAVTransportURIMetaData", val);
}


std::string
DevAVTransport::_getNextAVTransportURIMetaData()
{
    return _pService->getStateVar<std::string>("NextAVTransportURIMetaData");
}


void
DevAVTransport::_setRelativeTimePosition(const std::string& val)
{
    _pService->setStateVar<std::string>("RelativeTimePosition", val);
}


std::string
DevAVTransport::_getRelativeTimePosition()
{
    return _pService->getStateVar<std::string>("RelativeTimePosition");
}


void
DevAVTransport::_setAbsoluteTimePosition(const std::string& val)
{
    _pService->setStateVar<std::string>("AbsoluteTimePosition", val);
}


std::string
DevAVTransport::_getAbsoluteTimePosition()
{
    return _pService->getStateVar<std::string>("AbsoluteTimePosition");
}


void
DevAVTransport::_setRelativeCounterPosition(const i4& val)
{
    _pService->setStateVar<i4>("RelativeCounterPosition", val);
}


i4
DevAVTransport::_getRelativeCounterPosition()
{
    return _pService->getStateVar<i4>("RelativeCounterPosition");
}


void
DevAVTransport::_setAbsoluteCounterPosition(const i4& val)
{
    _pService->setStateVar<i4>("AbsoluteCounterPosition", val);
}


i4
DevAVTransport::_getAbsoluteCounterPosition()
{
    return _pService->getStateVar<i4>("AbsoluteCounterPosition");
}


void
DevAVTransport::_setCurrentTransportActions(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentTransportActions", val);
}


std::string
DevAVTransport::_getCurrentTransportActions()
{
    return _pService->getStateVar<std::string>("CurrentTransportActions");
}


void
DevAVTransport::_setLastChange(const std::string& val)
{
    _pService->setStateVar<std::string>("LastChange", val);
}


std::string
DevAVTransport::_getLastChange()
{
    return _pService->getStateVar<std::string>("LastChange");
}


void
DevConnectionManager::actionHandler(Action* pAction)
{
    std::string actionName = pAction->getName();
    if (actionName == "GetProtocolInfo") {
        std::string Source;
        std::string Sink;
        GetProtocolInfo(Source, Sink);
        pAction->setArgument<std::string>("Source", Source);
        pAction->setArgument<std::string>("Sink", Sink);
    }
    else if (actionName == "PrepareForConnection") {
        std::string RemoteProtocolInfo = pAction->getArgument<std::string>("RemoteProtocolInfo");
        std::string PeerConnectionManager = pAction->getArgument<std::string>("PeerConnectionManager");
        i4 PeerConnectionID = pAction->getArgument<i4>("PeerConnectionID");
        std::string Direction = pAction->getArgument<std::string>("Direction");
        i4 ConnectionID;
        i4 AVTransportID;
        i4 RcsID;
        PrepareForConnection(RemoteProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, ConnectionID, AVTransportID, RcsID);
        pAction->setArgument<i4>("ConnectionID", ConnectionID);
        pAction->setArgument<i4>("AVTransportID", AVTransportID);
        pAction->setArgument<i4>("RcsID", RcsID);
    }
    else if (actionName == "ConnectionComplete") {
        i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
        ConnectionComplete(ConnectionID);
    }
    else if (actionName == "GetCurrentConnectionIDs") {
        std::string ConnectionIDs;
        GetCurrentConnectionIDs(ConnectionIDs);
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
        GetCurrentConnectionInfo(ConnectionID, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
        pAction->setArgument<i4>("RcsID", RcsID);
        pAction->setArgument<i4>("AVTransportID", AVTransportID);
        pAction->setArgument<std::string>("ProtocolInfo", ProtocolInfo);
        pAction->setArgument<std::string>("PeerConnectionManager", PeerConnectionManager);
        pAction->setArgument<i4>("PeerConnectionID", PeerConnectionID);
        pAction->setArgument<std::string>("Direction", Direction);
        pAction->setArgument<std::string>("Status", Status);
    }
}


void
DevConnectionManager::_setSourceProtocolInfo(const std::string& val)
{
    _pService->setStateVar<std::string>("SourceProtocolInfo", val);
}


std::string
DevConnectionManager::_getSourceProtocolInfo()
{
    return _pService->getStateVar<std::string>("SourceProtocolInfo");
}


void
DevConnectionManager::_setSinkProtocolInfo(const std::string& val)
{
    _pService->setStateVar<std::string>("SinkProtocolInfo", val);
}


std::string
DevConnectionManager::_getSinkProtocolInfo()
{
    return _pService->getStateVar<std::string>("SinkProtocolInfo");
}


void
DevConnectionManager::_setCurrentConnectionIDs(const std::string& val)
{
    _pService->setStateVar<std::string>("CurrentConnectionIDs", val);
}


std::string
DevConnectionManager::_getCurrentConnectionIDs()
{
    return _pService->getStateVar<std::string>("CurrentConnectionIDs");
}


void
DevRenderingControl::actionHandler(Action* pAction)
{
    std::string actionName = pAction->getName();
    if (actionName == "ListPresets") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string CurrentPresetNameList;
        ListPresets(InstanceID, CurrentPresetNameList);
        pAction->setArgument<std::string>("CurrentPresetNameList", CurrentPresetNameList);
    }
    else if (actionName == "SelectPreset") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string PresetName = pAction->getArgument<std::string>("PresetName");
        SelectPreset(InstanceID, PresetName);
    }
    else if (actionName == "GetBrightness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentBrightness;
        GetBrightness(InstanceID, CurrentBrightness);
        pAction->setArgument<ui2>("CurrentBrightness", CurrentBrightness);
    }
    else if (actionName == "SetBrightness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredBrightness = pAction->getArgument<ui2>("DesiredBrightness");
        SetBrightness(InstanceID, DesiredBrightness);
    }
    else if (actionName == "GetContrast") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentContrast;
        GetContrast(InstanceID, CurrentContrast);
        pAction->setArgument<ui2>("CurrentContrast", CurrentContrast);
    }
    else if (actionName == "SetContrast") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredContrast = pAction->getArgument<ui2>("DesiredContrast");
        SetContrast(InstanceID, DesiredContrast);
    }
    else if (actionName == "GetSharpness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentSharpness;
        GetSharpness(InstanceID, CurrentSharpness);
        pAction->setArgument<ui2>("CurrentSharpness", CurrentSharpness);
    }
    else if (actionName == "SetSharpness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredSharpness = pAction->getArgument<ui2>("DesiredSharpness");
        SetSharpness(InstanceID, DesiredSharpness);
    }
    else if (actionName == "GetRedVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentRedVideoGain;
        GetRedVideoGain(InstanceID, CurrentRedVideoGain);
        pAction->setArgument<ui2>("CurrentRedVideoGain", CurrentRedVideoGain);
    }
    else if (actionName == "SetRedVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredRedVideoGain = pAction->getArgument<ui2>("DesiredRedVideoGain");
        SetRedVideoGain(InstanceID, DesiredRedVideoGain);
    }
    else if (actionName == "GetGreenVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentGreenVideoGain;
        GetGreenVideoGain(InstanceID, CurrentGreenVideoGain);
        pAction->setArgument<ui2>("CurrentGreenVideoGain", CurrentGreenVideoGain);
    }
    else if (actionName == "SetGreenVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredGreenVideoGain = pAction->getArgument<ui2>("DesiredGreenVideoGain");
        SetGreenVideoGain(InstanceID, DesiredGreenVideoGain);
    }
    else if (actionName == "GetBlueVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentBlueVideoGain;
        GetBlueVideoGain(InstanceID, CurrentBlueVideoGain);
        pAction->setArgument<ui2>("CurrentBlueVideoGain", CurrentBlueVideoGain);
    }
    else if (actionName == "SetBlueVideoGain") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredBlueVideoGain = pAction->getArgument<ui2>("DesiredBlueVideoGain");
        SetBlueVideoGain(InstanceID, DesiredBlueVideoGain);
    }
    else if (actionName == "GetRedVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentRedVideoBlackLevel;
        GetRedVideoBlackLevel(InstanceID, CurrentRedVideoBlackLevel);
        pAction->setArgument<ui2>("CurrentRedVideoBlackLevel", CurrentRedVideoBlackLevel);
    }
    else if (actionName == "SetRedVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredRedVideoBlackLevel = pAction->getArgument<ui2>("DesiredRedVideoBlackLevel");
        SetRedVideoBlackLevel(InstanceID, DesiredRedVideoBlackLevel);
    }
    else if (actionName == "GetGreenVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentGreenVideoBlackLevel;
        GetGreenVideoBlackLevel(InstanceID, CurrentGreenVideoBlackLevel);
        pAction->setArgument<ui2>("CurrentGreenVideoBlackLevel", CurrentGreenVideoBlackLevel);
    }
    else if (actionName == "SetGreenVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredGreenVideoBlackLevel = pAction->getArgument<ui2>("DesiredGreenVideoBlackLevel");
        SetGreenVideoBlackLevel(InstanceID, DesiredGreenVideoBlackLevel);
    }
    else if (actionName == "GetBlueVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentBlueVideoBlackLevel;
        GetBlueVideoBlackLevel(InstanceID, CurrentBlueVideoBlackLevel);
        pAction->setArgument<ui2>("CurrentBlueVideoBlackLevel", CurrentBlueVideoBlackLevel);
    }
    else if (actionName == "SetBlueVideoBlackLevel") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredBlueVideoBlackLevel = pAction->getArgument<ui2>("DesiredBlueVideoBlackLevel");
        SetBlueVideoBlackLevel(InstanceID, DesiredBlueVideoBlackLevel);
    }
    else if (actionName == "GetColorTemperature ") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 CurrentColorTemperature;
        GetColorTemperature (InstanceID, CurrentColorTemperature);
        pAction->setArgument<ui2>("CurrentColorTemperature", CurrentColorTemperature);
    }
    else if (actionName == "SetColorTemperature") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        ui2 DesiredColorTemperature = pAction->getArgument<ui2>("DesiredColorTemperature");
        SetColorTemperature(InstanceID, DesiredColorTemperature);
    }
    else if (actionName == "GetHorizontalKeystone") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        i2 CurrentHorizontalKeystone;
        GetHorizontalKeystone(InstanceID, CurrentHorizontalKeystone);
        pAction->setArgument<i2>("CurrentHorizontalKeystone", CurrentHorizontalKeystone);
    }
    else if (actionName == "SetHorizontalKeystone") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        i2 DesiredHorizontalKeystone = pAction->getArgument<i2>("DesiredHorizontalKeystone");
        SetHorizontalKeystone(InstanceID, DesiredHorizontalKeystone);
    }
    else if (actionName == "GetVerticalKeystone") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        i2 CurrentVerticalKeystone;
        GetVerticalKeystone(InstanceID, CurrentVerticalKeystone);
        pAction->setArgument<i2>("CurrentVerticalKeystone", CurrentVerticalKeystone);
    }
    else if (actionName == "SetVerticalKeystone") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        i2 DesiredVerticalKeystone = pAction->getArgument<i2>("DesiredVerticalKeystone");
        SetVerticalKeystone(InstanceID, DesiredVerticalKeystone);
    }
    else if (actionName == "GetMute") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        bool CurrentMute;
        GetMute(InstanceID, Channel, CurrentMute);
        pAction->setArgument<bool>("CurrentMute", CurrentMute);
    }
    else if (actionName == "SetMute") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        bool DesiredMute = pAction->getArgument<bool>("DesiredMute");
        SetMute(InstanceID, Channel, DesiredMute);
    }
    else if (actionName == "GetVolume") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        ui2 CurrentVolume;
        GetVolume(InstanceID, Channel, CurrentVolume);
        pAction->setArgument<ui2>("CurrentVolume", CurrentVolume);
    }
    else if (actionName == "SetVolume") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        ui2 DesiredVolume = pAction->getArgument<ui2>("DesiredVolume");
        SetVolume(InstanceID, Channel, DesiredVolume);
    }
    else if (actionName == "GetVolumeDB") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        i2 CurrentVolume;
        GetVolumeDB(InstanceID, Channel, CurrentVolume);
        pAction->setArgument<i2>("CurrentVolume", CurrentVolume);
    }
    else if (actionName == "SetVolumeDB") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        i2 DesiredVolume = pAction->getArgument<i2>("DesiredVolume");
        SetVolumeDB(InstanceID, Channel, DesiredVolume);
    }
    else if (actionName == "GetVolumeDBRange") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        i2 MinValue;
        i2 MaxValue;
        GetVolumeDBRange(InstanceID, Channel, MinValue, MaxValue);
        pAction->setArgument<i2>("MinValue", MinValue);
        pAction->setArgument<i2>("MaxValue", MaxValue);
    }
    else if (actionName == "GetLoudness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        bool CurrentLoudness;
        GetLoudness(InstanceID, Channel, CurrentLoudness);
        pAction->setArgument<bool>("CurrentLoudness", CurrentLoudness);
    }
    else if (actionName == "SetLoudness") {
        ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
        std::string Channel = pAction->getArgument<std::string>("Channel");
        bool DesiredLoudness = pAction->getArgument<bool>("DesiredLoudness");
        SetLoudness(InstanceID, Channel, DesiredLoudness);
    }
}


void
DevRenderingControl::_setPresetNameList(const std::string& val)
{
    _pService->setStateVar<std::string>("PresetNameList", val);
}


std::string
DevRenderingControl::_getPresetNameList()
{
    return _pService->getStateVar<std::string>("PresetNameList");
}


void
DevRenderingControl::_setLastChange(const std::string& val)
{
    _pService->setStateVar<std::string>("LastChange", val);
}


std::string
DevRenderingControl::_getLastChange()
{
    return _pService->getStateVar<std::string>("LastChange");
}


void
DevRenderingControl::_setBrightness(const ui2& val)
{
    _pService->setStateVar<ui2>("Brightness", val);
}


ui2
DevRenderingControl::_getBrightness()
{
    return _pService->getStateVar<ui2>("Brightness");
}


void
DevRenderingControl::_setContrast(const ui2& val)
{
    _pService->setStateVar<ui2>("Contrast", val);
}


ui2
DevRenderingControl::_getContrast()
{
    return _pService->getStateVar<ui2>("Contrast");
}


void
DevRenderingControl::_setSharpness(const ui2& val)
{
    _pService->setStateVar<ui2>("Sharpness", val);
}


ui2
DevRenderingControl::_getSharpness()
{
    return _pService->getStateVar<ui2>("Sharpness");
}


void
DevRenderingControl::_setRedVideoGain(const ui2& val)
{
    _pService->setStateVar<ui2>("RedVideoGain", val);
}


ui2
DevRenderingControl::_getRedVideoGain()
{
    return _pService->getStateVar<ui2>("RedVideoGain");
}


void
DevRenderingControl::_setGreenVideoGain(const ui2& val)
{
    _pService->setStateVar<ui2>("GreenVideoGain", val);
}


ui2
DevRenderingControl::_getGreenVideoGain()
{
    return _pService->getStateVar<ui2>("GreenVideoGain");
}


void
DevRenderingControl::_setBlueVideoGain(const ui2& val)
{
    _pService->setStateVar<ui2>("BlueVideoGain", val);
}


ui2
DevRenderingControl::_getBlueVideoGain()
{
    return _pService->getStateVar<ui2>("BlueVideoGain");
}


void
DevRenderingControl::_setRedVideoBlackLevel(const ui2& val)
{
    _pService->setStateVar<ui2>("RedVideoBlackLevel", val);
}


ui2
DevRenderingControl::_getRedVideoBlackLevel()
{
    return _pService->getStateVar<ui2>("RedVideoBlackLevel");
}


void
DevRenderingControl::_setGreenVideoBlackLevel(const ui2& val)
{
    _pService->setStateVar<ui2>("GreenVideoBlackLevel", val);
}


ui2
DevRenderingControl::_getGreenVideoBlackLevel()
{
    return _pService->getStateVar<ui2>("GreenVideoBlackLevel");
}


void
DevRenderingControl::_setBlueVideoBlackLevel(const ui2& val)
{
    _pService->setStateVar<ui2>("BlueVideoBlackLevel", val);
}


ui2
DevRenderingControl::_getBlueVideoBlackLevel()
{
    return _pService->getStateVar<ui2>("BlueVideoBlackLevel");
}


void
DevRenderingControl::_setColorTemperature(const ui2& val)
{
    _pService->setStateVar<ui2>("ColorTemperature", val);
}


ui2
DevRenderingControl::_getColorTemperature()
{
    return _pService->getStateVar<ui2>("ColorTemperature");
}


void
DevRenderingControl::_setHorizontalKeystone(const i2& val)
{
    _pService->setStateVar<i2>("HorizontalKeystone", val);
}


i2
DevRenderingControl::_getHorizontalKeystone()
{
    return _pService->getStateVar<i2>("HorizontalKeystone");
}


void
DevRenderingControl::_setVerticalKeystone(const i2& val)
{
    _pService->setStateVar<i2>("VerticalKeystone", val);
}


i2
DevRenderingControl::_getVerticalKeystone()
{
    return _pService->getStateVar<i2>("VerticalKeystone");
}


void
DevRenderingControl::_setMute(const bool& val)
{
    _pService->setStateVar<bool>("Mute", val);
}


bool
DevRenderingControl::_getMute()
{
    return _pService->getStateVar<bool>("Mute");
}


void
DevRenderingControl::_setVolume(const ui2& val)
{
    _pService->setStateVar<ui2>("Volume", val);
}


ui2
DevRenderingControl::_getVolume()
{
    return _pService->getStateVar<ui2>("Volume");
}


void
DevRenderingControl::_setVolumeDB(const i2& val)
{
    _pService->setStateVar<i2>("VolumeDB", val);
}


i2
DevRenderingControl::_getVolumeDB()
{
    return _pService->getStateVar<i2>("VolumeDB");
}


void
DevRenderingControl::_setLoudness(const bool& val)
{
    _pService->setStateVar<bool>("Loudness", val);
}


bool
DevRenderingControl::_getLoudness()
{
    return _pService->getStateVar<bool>("Loudness");
}


void
DevContentDirectory::actionHandler(Action* pAction)
{
    std::string actionName = pAction->getName();
    if (actionName == "GetSearchCapabilities") {
        std::string SearchCaps;
        GetSearchCapabilities(SearchCaps);
        pAction->setArgument<std::string>("SearchCaps", SearchCaps);
    }
    else if (actionName == "GetSortCapabilities") {
        std::string SortCaps;
        GetSortCapabilities(SortCaps);
        pAction->setArgument<std::string>("SortCaps", SortCaps);
    }
    else if (actionName == "GetSystemUpdateID") {
        ui4 Id;
        GetSystemUpdateID(Id);
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
        Browse(ObjectID, BrowseFlag, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
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
        Search(ContainerID, SearchCriteria, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
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
        CreateObject(ContainerID, Elements, ObjectID, Result);
        pAction->setArgument<std::string>("ObjectID", ObjectID);
        pAction->setArgument<std::string>("Result", Result);
    }
    else if (actionName == "DestroyObject") {
        std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
        DestroyObject(ObjectID);
    }
    else if (actionName == "UpdateObject") {
        std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
        std::string CurrentTagValue = pAction->getArgument<std::string>("CurrentTagValue");
        std::string NewTagValue = pAction->getArgument<std::string>("NewTagValue");
        UpdateObject(ObjectID, CurrentTagValue, NewTagValue);
    }
    else if (actionName == "ImportResource") {
        uri SourceURI = pAction->getArgument<uri>("SourceURI");
        uri DestinationURI = pAction->getArgument<uri>("DestinationURI");
        ui4 TransferID;
        ImportResource(SourceURI, DestinationURI, TransferID);
        pAction->setArgument<ui4>("TransferID", TransferID);
    }
    else if (actionName == "GetTransferProgress") {
        ui4 TransferID = pAction->getArgument<ui4>("TransferID");
        std::string TransferStatus;
        std::string TransferLength;
        std::string TransferTotal;
        GetTransferProgress(TransferID, TransferStatus, TransferLength, TransferTotal);
        pAction->setArgument<std::string>("TransferStatus", TransferStatus);
        pAction->setArgument<std::string>("TransferLength", TransferLength);
        pAction->setArgument<std::string>("TransferTotal", TransferTotal);
    }
    else if (actionName == "DeleteResource") {
        uri ResourceURI = pAction->getArgument<uri>("ResourceURI");
        DeleteResource(ResourceURI);
    }
    else if (actionName == "CreateReference") {
        std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
        std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
        std::string NewID;
        CreateReference(ContainerID, ObjectID, NewID);
        pAction->setArgument<std::string>("NewID", NewID);
    }
}


void
DevContentDirectory::_setTransferIDs(const std::string& val)
{
    _pService->setStateVar<std::string>("TransferIDs", val);
}


std::string
DevContentDirectory::_getTransferIDs()
{
    return _pService->getStateVar<std::string>("TransferIDs");
}


void
DevContentDirectory::_setSearchCapabilities(const std::string& val)
{
    _pService->setStateVar<std::string>("SearchCapabilities", val);
}


std::string
DevContentDirectory::_getSearchCapabilities()
{
    return _pService->getStateVar<std::string>("SearchCapabilities");
}


void
DevContentDirectory::_setSortCapabilities(const std::string& val)
{
    _pService->setStateVar<std::string>("SortCapabilities", val);
}


std::string
DevContentDirectory::_getSortCapabilities()
{
    return _pService->getStateVar<std::string>("SortCapabilities");
}


void
DevContentDirectory::_setSystemUpdateID(const ui4& val)
{
    _pService->setStateVar<ui4>("SystemUpdateID", val);
}


ui4
DevContentDirectory::_getSystemUpdateID()
{
    return _pService->getStateVar<ui4>("SystemUpdateID");
}


void
DevContentDirectory::_setContainerUpdateIDs(const std::string& val)
{
    _pService->setStateVar<std::string>("ContainerUpdateIDs", val);
}


std::string
DevContentDirectory::_getContainerUpdateIDs()
{
    return _pService->getStateVar<std::string>("ContainerUpdateIDs");
}


} // namespace Av
} // namespace Omm
