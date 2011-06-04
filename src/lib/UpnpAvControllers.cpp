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

#include "UpnpAvControllers.h"
#include "UpnpInternal.h"

namespace Omm {
namespace Av {


CtlMediaRenderer::CtlMediaRenderer(Device* pDevice, CtlRenderingControl* pCtlRenderingControl, CtlConnectionManager* pCtlConnectionManager, CtlAVTransport* pCtlAVTransport) :
CtlDeviceCode(pDevice),
_pCtlRenderingControl(pCtlRenderingControl),
_pCtlConnectionManager(pCtlConnectionManager),
_pCtlAVTransport(pCtlAVTransport)
{
    _pCtlRenderingControl->_pService = _pDevice->getService("urn:schemas-upnp-org:service:RenderingControl:1");
    _pCtlConnectionManager->_pService = _pDevice->getService("urn:schemas-upnp-org:service:ConnectionManager:1");
    _pCtlAVTransport->_pService = _pDevice->getService("urn:schemas-upnp-org:service:AVTransport:1");

    init();
}


CtlMediaRenderer::~CtlMediaRenderer()
{
    delete _pCtlRenderingControl;
    delete _pCtlConnectionManager;
    delete _pCtlAVTransport;
}


void
CtlMediaRenderer::eventHandler(StateVar* pStateVar)
{
    if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlAVTransport->_changedLastChange(val);
    }
    else if (pStateVar->getName() == "SourceProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedSourceProtocolInfo(val);
    }
    else if (pStateVar->getName() == "SinkProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedSinkProtocolInfo(val);
    }
    else if (pStateVar->getName() == "CurrentConnectionIDs") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedCurrentConnectionIDs(val);
    }
    else if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlRenderingControl->_changedLastChange(val);
    }
}


CtlMediaServer::CtlMediaServer(Device* pDevice, CtlContentDirectory* pCtlContentDirectory, CtlConnectionManager* pCtlConnectionManager, CtlAVTransport* pCtlAVTransport) :
CtlDeviceCode(pDevice),
_pCtlContentDirectory(pCtlContentDirectory),
_pCtlConnectionManager(pCtlConnectionManager),
_pCtlAVTransport(pCtlAVTransport)
{
    _pCtlContentDirectory->_pService = _pDevice->getService("urn:schemas-upnp-org:service:ContentDirectory:1");
    _pCtlConnectionManager->_pService = _pDevice->getService("urn:schemas-upnp-org:service:ConnectionManager:1");
    _pCtlAVTransport->_pService = _pDevice->getService("urn:schemas-upnp-org:service:AVTransport:1");

    init();
}


CtlMediaServer::~CtlMediaServer()
{
    delete _pCtlContentDirectory;
    delete _pCtlConnectionManager;
    delete _pCtlAVTransport;
}


void
CtlMediaServer::eventHandler(StateVar* pStateVar)
{
    if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlAVTransport->_changedLastChange(val);
    }
    else if (pStateVar->getName() == "SourceProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedSourceProtocolInfo(val);
    }
    else if (pStateVar->getName() == "SinkProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedSinkProtocolInfo(val);
    }
    else if (pStateVar->getName() == "CurrentConnectionIDs") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlConnectionManager->_changedCurrentConnectionIDs(val);
    }
    else if (pStateVar->getName() == "TransferIDs") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlContentDirectory->_changedTransferIDs(val);
    }
    else if (pStateVar->getName() == "SystemUpdateID") {
        Omm::ui4 val;
        pStateVar->getValue(val);
        _pCtlContentDirectory->_changedSystemUpdateID(val);
    }
    else if (pStateVar->getName() == "ContainerUpdateIDs") {
        std::string val;
        pStateVar->getValue(val);
        _pCtlContentDirectory->_changedContainerUpdateIDs(val);
    }
}


void
CtlAVTransport::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    Action* pAction = _pService->getAction("SetAVTransportURI")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("CurrentURI", CurrentURI);
    pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
    Action* pAction = _pService->getAction("GetMediaInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    NrTracks = pAction->getArgument<ui4>("NrTracks");
    MediaDuration = pAction->getArgument<std::string>("MediaDuration");
    CurrentURI = pAction->getArgument<std::string>("CurrentURI");
    CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
    NextURI = pAction->getArgument<std::string>("NextURI");
    NextURIMetaData = pAction->getArgument<std::string>("NextURIMetaData");
    PlayMedium = pAction->getArgument<std::string>("PlayMedium");
    RecordMedium = pAction->getArgument<std::string>("RecordMedium");
    WriteStatus = pAction->getArgument<std::string>("WriteStatus");
}

void
CtlAVTransport::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
    Action* pAction = _pService->getAction("GetTransportInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentTransportState = pAction->getArgument<std::string>("CurrentTransportState");
    CurrentTransportStatus = pAction->getArgument<std::string>("CurrentTransportStatus");
    CurrentSpeed = pAction->getArgument<std::string>("CurrentSpeed");
}

void
CtlAVTransport::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
    Action* pAction = _pService->getAction("GetPositionInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    Track = pAction->getArgument<ui4>("Track");
    TrackDuration = pAction->getArgument<std::string>("TrackDuration");
    TrackMetaData = pAction->getArgument<std::string>("TrackMetaData");
    TrackURI = pAction->getArgument<std::string>("TrackURI");
    RelTime = pAction->getArgument<std::string>("RelTime");
    AbsTime = pAction->getArgument<std::string>("AbsTime");
    RelCount = pAction->getArgument<i4>("RelCount");
    AbsCount = pAction->getArgument<i4>("AbsCount");
}

void
CtlAVTransport::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
    Action* pAction = _pService->getAction("GetDeviceCapabilities")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    PlayMedia = pAction->getArgument<std::string>("PlayMedia");
    RecMedia = pAction->getArgument<std::string>("RecMedia");
    RecQualityModes = pAction->getArgument<std::string>("RecQualityModes");
}

void
CtlAVTransport::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
    Action* pAction = _pService->getAction("GetTransportSettings")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    PlayMode = pAction->getArgument<std::string>("PlayMode");
    RecQualityMode = pAction->getArgument<std::string>("RecQualityMode");
}

void
CtlAVTransport::Stop(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Stop")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::Play(const ui4& InstanceID, const std::string& Speed)
{
    Action* pAction = _pService->getAction("Play")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Speed", Speed);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::Pause(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Pause")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    Action* pAction = _pService->getAction("Seek")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Unit", Unit);
    pAction->setArgument<std::string>("Target", Target);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::Next(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Next")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
}

void
CtlAVTransport::Previous(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Previous")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
}

std::string
CtlAVTransport::_getLastChange()
{
    return _pService->getStateVar<std::string>("LastChange");
}


void 
CtlAVTransport::_reqSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    Action* pAction = _pService->getAction("SetAVTransportURI")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("CurrentURI", CurrentURI);
    pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadSetAVTransportURI, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqGetMediaInfo(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetMediaInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadGetMediaInfo, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqGetTransportInfo(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetTransportInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadGetTransportInfo, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqGetPositionInfo(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetPositionInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadGetPositionInfo, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqGetDeviceCapabilities(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetDeviceCapabilities")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadGetDeviceCapabilities, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqGetTransportSettings(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetTransportSettings")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadGetTransportSettings, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqStop(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Stop")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadStop, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqPlay(const ui4& InstanceID, const std::string& Speed)
{
    Action* pAction = _pService->getAction("Play")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Speed", Speed);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadPlay, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqPause(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Pause")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadPause, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    Action* pAction = _pService->getAction("Seek")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Unit", Unit);
    pAction->setArgument<std::string>("Target", Target);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadSeek, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqNext(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Next")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadNext, pAction);
    thread.start();
}

void 
CtlAVTransport::_reqPrevious(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("Previous")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlAVTransport> thread(this, &CtlAVTransport::_threadPrevious, pAction);
    thread.start();
}

void 
CtlAVTransport::_threadSetAVTransportURI(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
    std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
    _ansSetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
}

void 
CtlAVTransport::_threadGetMediaInfo(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui4 NrTracks = pAction->getArgument<ui4>("NrTracks");
    std::string MediaDuration = pAction->getArgument<std::string>("MediaDuration");
    std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
    std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
    std::string NextURI = pAction->getArgument<std::string>("NextURI");
    std::string NextURIMetaData = pAction->getArgument<std::string>("NextURIMetaData");
    std::string PlayMedium = pAction->getArgument<std::string>("PlayMedium");
    std::string RecordMedium = pAction->getArgument<std::string>("RecordMedium");
    std::string WriteStatus = pAction->getArgument<std::string>("WriteStatus");
    _ansGetMediaInfo(InstanceID, NrTracks, MediaDuration, CurrentURI, CurrentURIMetaData, NextURI, NextURIMetaData, PlayMedium, RecordMedium, WriteStatus);
}

void 
CtlAVTransport::_threadGetTransportInfo(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentTransportState = pAction->getArgument<std::string>("CurrentTransportState");
    std::string CurrentTransportStatus = pAction->getArgument<std::string>("CurrentTransportStatus");
    std::string CurrentSpeed = pAction->getArgument<std::string>("CurrentSpeed");
    _ansGetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
}

void 
CtlAVTransport::_threadGetPositionInfo(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui4 Track = pAction->getArgument<ui4>("Track");
    std::string TrackDuration = pAction->getArgument<std::string>("TrackDuration");
    std::string TrackMetaData = pAction->getArgument<std::string>("TrackMetaData");
    std::string TrackURI = pAction->getArgument<std::string>("TrackURI");
    std::string RelTime = pAction->getArgument<std::string>("RelTime");
    std::string AbsTime = pAction->getArgument<std::string>("AbsTime");
    i4 RelCount = pAction->getArgument<i4>("RelCount");
    i4 AbsCount = pAction->getArgument<i4>("AbsCount");
    _ansGetPositionInfo(InstanceID, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
}

void 
CtlAVTransport::_threadGetDeviceCapabilities(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PlayMedia = pAction->getArgument<std::string>("PlayMedia");
    std::string RecMedia = pAction->getArgument<std::string>("RecMedia");
    std::string RecQualityModes = pAction->getArgument<std::string>("RecQualityModes");
    _ansGetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
}

void 
CtlAVTransport::_threadGetTransportSettings(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PlayMode = pAction->getArgument<std::string>("PlayMode");
    std::string RecQualityMode = pAction->getArgument<std::string>("RecQualityMode");
    _ansGetTransportSettings(InstanceID, PlayMode, RecQualityMode);
}

void 
CtlAVTransport::_threadStop(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansStop(InstanceID);
}

void 
CtlAVTransport::_threadPlay(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Speed = pAction->getArgument<std::string>("Speed");
    _ansPlay(InstanceID, Speed);
}

void 
CtlAVTransport::_threadPause(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansPause(InstanceID);
}

void 
CtlAVTransport::_threadSeek(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Unit = pAction->getArgument<std::string>("Unit");
    std::string Target = pAction->getArgument<std::string>("Target");
    _ansSeek(InstanceID, Unit, Target);
}

void 
CtlAVTransport::_threadNext(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansNext(InstanceID);
}

void 
CtlAVTransport::_threadPrevious(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansPrevious(InstanceID);
}



void
CtlConnectionManager::GetProtocolInfo(std::string& Source, std::string& Sink)
{
    Action* pAction = _pService->getAction("GetProtocolInfo")->clone();
    _pService->sendAction(pAction);
    Source = pAction->getArgument<std::string>("Source");
    Sink = pAction->getArgument<std::string>("Sink");
}

void
CtlConnectionManager::ConnectionComplete(const i4& ConnectionID)
{
    Action* pAction = _pService->getAction("ConnectionComplete")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    _pService->sendAction(pAction);
}

void
CtlConnectionManager::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
    Action* pAction = _pService->getAction("GetCurrentConnectionIDs")->clone();
    _pService->sendAction(pAction);
    ConnectionIDs = pAction->getArgument<std::string>("ConnectionIDs");
}

void
CtlConnectionManager::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
    Action* pAction = _pService->getAction("GetCurrentConnectionInfo")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    _pService->sendAction(pAction);
    RcsID = pAction->getArgument<i4>("RcsID");
    AVTransportID = pAction->getArgument<i4>("AVTransportID");
    ProtocolInfo = pAction->getArgument<std::string>("ProtocolInfo");
    PeerConnectionManager = pAction->getArgument<std::string>("PeerConnectionManager");
    PeerConnectionID = pAction->getArgument<i4>("PeerConnectionID");
    Direction = pAction->getArgument<std::string>("Direction");
    Status = pAction->getArgument<std::string>("Status");
}

std::string
CtlConnectionManager::_getSourceProtocolInfo()
{
    return _pService->getStateVar<std::string>("SourceProtocolInfo");
}

std::string
CtlConnectionManager::_getSinkProtocolInfo()
{
    return _pService->getStateVar<std::string>("SinkProtocolInfo");
}

std::string
CtlConnectionManager::_getCurrentConnectionIDs()
{
    return _pService->getStateVar<std::string>("CurrentConnectionIDs");
}


void 
CtlConnectionManager::_reqGetProtocolInfo()
{
    Action* pAction = _pService->getAction("GetProtocolInfo")->clone();
    ActionThread<CtlConnectionManager> thread(this, &CtlConnectionManager::_threadGetProtocolInfo, pAction);
    thread.start();
}

void 
CtlConnectionManager::_reqConnectionComplete(const i4& ConnectionID)
{
    Action* pAction = _pService->getAction("ConnectionComplete")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    ActionThread<CtlConnectionManager> thread(this, &CtlConnectionManager::_threadConnectionComplete, pAction);
    thread.start();
}

void 
CtlConnectionManager::_reqGetCurrentConnectionIDs()
{
    Action* pAction = _pService->getAction("GetCurrentConnectionIDs")->clone();
    ActionThread<CtlConnectionManager> thread(this, &CtlConnectionManager::_threadGetCurrentConnectionIDs, pAction);
    thread.start();
}

void 
CtlConnectionManager::_reqGetCurrentConnectionInfo(const i4& ConnectionID)
{
    Action* pAction = _pService->getAction("GetCurrentConnectionInfo")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    ActionThread<CtlConnectionManager> thread(this, &CtlConnectionManager::_threadGetCurrentConnectionInfo, pAction);
    thread.start();
}

void 
CtlConnectionManager::_threadGetProtocolInfo(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string Source = pAction->getArgument<std::string>("Source");
    std::string Sink = pAction->getArgument<std::string>("Sink");
    _ansGetProtocolInfo(Source, Sink);
}

void 
CtlConnectionManager::_threadConnectionComplete(Action* pAction)
{
    _pService->sendAction(pAction);
    i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
    _ansConnectionComplete(ConnectionID);
}

void 
CtlConnectionManager::_threadGetCurrentConnectionIDs(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ConnectionIDs = pAction->getArgument<std::string>("ConnectionIDs");
    _ansGetCurrentConnectionIDs(ConnectionIDs);
}

void 
CtlConnectionManager::_threadGetCurrentConnectionInfo(Action* pAction)
{
    _pService->sendAction(pAction);
    i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
    i4 RcsID = pAction->getArgument<i4>("RcsID");
    i4 AVTransportID = pAction->getArgument<i4>("AVTransportID");
    std::string ProtocolInfo = pAction->getArgument<std::string>("ProtocolInfo");
    std::string PeerConnectionManager = pAction->getArgument<std::string>("PeerConnectionManager");
    i4 PeerConnectionID = pAction->getArgument<i4>("PeerConnectionID");
    std::string Direction = pAction->getArgument<std::string>("Direction");
    std::string Status = pAction->getArgument<std::string>("Status");
    _ansGetCurrentConnectionInfo(ConnectionID, RcsID, AVTransportID, ProtocolInfo, PeerConnectionManager, PeerConnectionID, Direction, Status);
}



void
CtlRenderingControl::ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList)
{
    Action* pAction = _pService->getAction("ListPresets")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentPresetNameList = pAction->getArgument<std::string>("CurrentPresetNameList");
}

void
CtlRenderingControl::SelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
    Action* pAction = _pService->getAction("SelectPreset")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("PresetName", PresetName);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness)
{
    Action* pAction = _pService->getAction("GetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentBrightness = pAction->getArgument<ui2>("CurrentBrightness");
}

void
CtlRenderingControl::SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
    Action* pAction = _pService->getAction("SetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBrightness", DesiredBrightness);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetContrast(const ui4& InstanceID, ui2& CurrentContrast)
{
    Action* pAction = _pService->getAction("GetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentContrast = pAction->getArgument<ui2>("CurrentContrast");
}

void
CtlRenderingControl::SetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
    Action* pAction = _pService->getAction("SetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredContrast", DesiredContrast);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness)
{
    Action* pAction = _pService->getAction("GetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentSharpness = pAction->getArgument<ui2>("CurrentSharpness");
}

void
CtlRenderingControl::SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
    Action* pAction = _pService->getAction("SetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredSharpness", DesiredSharpness);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain)
{
    Action* pAction = _pService->getAction("GetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentRedVideoGain = pAction->getArgument<ui2>("CurrentRedVideoGain");
}

void
CtlRenderingControl::SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
    Action* pAction = _pService->getAction("SetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoGain", DesiredRedVideoGain);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain)
{
    Action* pAction = _pService->getAction("GetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentGreenVideoGain = pAction->getArgument<ui2>("CurrentGreenVideoGain");
}

void
CtlRenderingControl::SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
    Action* pAction = _pService->getAction("SetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoGain", DesiredGreenVideoGain);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain)
{
    Action* pAction = _pService->getAction("GetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentBlueVideoGain = pAction->getArgument<ui2>("CurrentBlueVideoGain");
}

void
CtlRenderingControl::SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
    Action* pAction = _pService->getAction("SetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoGain", DesiredBlueVideoGain);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel)
{
    Action* pAction = _pService->getAction("GetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentRedVideoBlackLevel = pAction->getArgument<ui2>("CurrentRedVideoBlackLevel");
}

void
CtlRenderingControl::SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoBlackLevel", DesiredRedVideoBlackLevel);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel)
{
    Action* pAction = _pService->getAction("GetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentGreenVideoBlackLevel = pAction->getArgument<ui2>("CurrentGreenVideoBlackLevel");
}

void
CtlRenderingControl::SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoBlackLevel", DesiredGreenVideoBlackLevel);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel)
{
    Action* pAction = _pService->getAction("GetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentBlueVideoBlackLevel = pAction->getArgument<ui2>("CurrentBlueVideoBlackLevel");
}

void
CtlRenderingControl::SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoBlackLevel", DesiredBlueVideoBlackLevel);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature)
{
    Action* pAction = _pService->getAction("GetColorTemperature ")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentColorTemperature = pAction->getArgument<ui2>("CurrentColorTemperature");
}

void
CtlRenderingControl::SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
    Action* pAction = _pService->getAction("SetColorTemperature")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredColorTemperature", DesiredColorTemperature);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone)
{
    Action* pAction = _pService->getAction("GetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentHorizontalKeystone = pAction->getArgument<i2>("CurrentHorizontalKeystone");
}

void
CtlRenderingControl::SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
    Action* pAction = _pService->getAction("SetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredHorizontalKeystone", DesiredHorizontalKeystone);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone)
{
    Action* pAction = _pService->getAction("GetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    _pService->sendAction(pAction);
    CurrentVerticalKeystone = pAction->getArgument<i2>("CurrentVerticalKeystone");
}

void
CtlRenderingControl::SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
    Action* pAction = _pService->getAction("SetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredVerticalKeystone", DesiredVerticalKeystone);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute)
{
    Action* pAction = _pService->getAction("GetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    _pService->sendAction(pAction);
    CurrentMute = pAction->getArgument<bool>("CurrentMute");
}

void
CtlRenderingControl::SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
    Action* pAction = _pService->getAction("SetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredMute", DesiredMute);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume)
{
    Action* pAction = _pService->getAction("GetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    _pService->sendAction(pAction);
    CurrentVolume = pAction->getArgument<ui2>("CurrentVolume");
}

void
CtlRenderingControl::SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Action* pAction = _pService->getAction("SetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<ui2>("DesiredVolume", DesiredVolume);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume)
{
    Action* pAction = _pService->getAction("GetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    _pService->sendAction(pAction);
    CurrentVolume = pAction->getArgument<i2>("CurrentVolume");
}

void
CtlRenderingControl::SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
    Action* pAction = _pService->getAction("SetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<i2>("DesiredVolume", DesiredVolume);
    _pService->sendAction(pAction);
}

void
CtlRenderingControl::GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue)
{
    Action* pAction = _pService->getAction("GetVolumeDBRange")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    _pService->sendAction(pAction);
    MinValue = pAction->getArgument<i2>("MinValue");
    MaxValue = pAction->getArgument<i2>("MaxValue");
}

void
CtlRenderingControl::GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness)
{
    Action* pAction = _pService->getAction("GetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    _pService->sendAction(pAction);
    CurrentLoudness = pAction->getArgument<bool>("CurrentLoudness");
}

void
CtlRenderingControl::SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
    Action* pAction = _pService->getAction("SetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredLoudness", DesiredLoudness);
    _pService->sendAction(pAction);
}

std::string
CtlRenderingControl::_getLastChange()
{
    return _pService->getStateVar<std::string>("LastChange");
}


void 
CtlRenderingControl::_reqListPresets(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("ListPresets")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadListPresets, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
    Action* pAction = _pService->getAction("SelectPreset")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("PresetName", PresetName);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSelectPreset, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetBrightness(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetBrightness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
    Action* pAction = _pService->getAction("SetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBrightness", DesiredBrightness);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetBrightness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetContrast(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetContrast, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
    Action* pAction = _pService->getAction("SetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredContrast", DesiredContrast);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetContrast, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetSharpness(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetSharpness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
    Action* pAction = _pService->getAction("SetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredSharpness", DesiredSharpness);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetSharpness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetRedVideoGain(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetRedVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
    Action* pAction = _pService->getAction("SetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoGain", DesiredRedVideoGain);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetRedVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetGreenVideoGain(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetGreenVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
    Action* pAction = _pService->getAction("SetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoGain", DesiredGreenVideoGain);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetGreenVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetBlueVideoGain(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetBlueVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
    Action* pAction = _pService->getAction("SetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoGain", DesiredBlueVideoGain);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetBlueVideoGain, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetRedVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetRedVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoBlackLevel", DesiredRedVideoBlackLevel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetRedVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetGreenVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetGreenVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoBlackLevel", DesiredGreenVideoBlackLevel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetGreenVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetBlueVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetBlueVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
    Action* pAction = _pService->getAction("SetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoBlackLevel", DesiredBlueVideoBlackLevel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetBlueVideoBlackLevel, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetColorTemperature (const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetColorTemperature ")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetColorTemperature , pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
    Action* pAction = _pService->getAction("SetColorTemperature")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredColorTemperature", DesiredColorTemperature);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetColorTemperature, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetHorizontalKeystone(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetHorizontalKeystone, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
    Action* pAction = _pService->getAction("SetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredHorizontalKeystone", DesiredHorizontalKeystone);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetHorizontalKeystone, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetVerticalKeystone(const ui4& InstanceID)
{
    Action* pAction = _pService->getAction("GetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetVerticalKeystone, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
    Action* pAction = _pService->getAction("SetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredVerticalKeystone", DesiredVerticalKeystone);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetVerticalKeystone, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetMute(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = _pService->getAction("GetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetMute, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
    Action* pAction = _pService->getAction("SetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredMute", DesiredMute);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetMute, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetVolume(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = _pService->getAction("GetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetVolume, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Action* pAction = _pService->getAction("SetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<ui2>("DesiredVolume", DesiredVolume);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetVolume, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetVolumeDB(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = _pService->getAction("GetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetVolumeDB, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
    Action* pAction = _pService->getAction("SetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<i2>("DesiredVolume", DesiredVolume);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetVolumeDB, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = _pService->getAction("GetVolumeDBRange")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetVolumeDBRange, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqGetLoudness(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = _pService->getAction("GetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadGetLoudness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_reqSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
    Action* pAction = _pService->getAction("SetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredLoudness", DesiredLoudness);
    ActionThread<CtlRenderingControl> thread(this, &CtlRenderingControl::_threadSetLoudness, pAction);
    thread.start();
}

void 
CtlRenderingControl::_threadListPresets(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentPresetNameList = pAction->getArgument<std::string>("CurrentPresetNameList");
    _ansListPresets(InstanceID, CurrentPresetNameList);
}

void 
CtlRenderingControl::_threadSelectPreset(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PresetName = pAction->getArgument<std::string>("PresetName");
    _ansSelectPreset(InstanceID, PresetName);
}

void 
CtlRenderingControl::_threadGetBrightness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBrightness = pAction->getArgument<ui2>("CurrentBrightness");
    _ansGetBrightness(InstanceID, CurrentBrightness);
}

void 
CtlRenderingControl::_threadSetBrightness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBrightness = pAction->getArgument<ui2>("DesiredBrightness");
    _ansSetBrightness(InstanceID, DesiredBrightness);
}

void 
CtlRenderingControl::_threadGetContrast(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentContrast = pAction->getArgument<ui2>("CurrentContrast");
    _ansGetContrast(InstanceID, CurrentContrast);
}

void 
CtlRenderingControl::_threadSetContrast(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredContrast = pAction->getArgument<ui2>("DesiredContrast");
    _ansSetContrast(InstanceID, DesiredContrast);
}

void 
CtlRenderingControl::_threadGetSharpness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentSharpness = pAction->getArgument<ui2>("CurrentSharpness");
    _ansGetSharpness(InstanceID, CurrentSharpness);
}

void 
CtlRenderingControl::_threadSetSharpness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredSharpness = pAction->getArgument<ui2>("DesiredSharpness");
    _ansSetSharpness(InstanceID, DesiredSharpness);
}

void 
CtlRenderingControl::_threadGetRedVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentRedVideoGain = pAction->getArgument<ui2>("CurrentRedVideoGain");
    _ansGetRedVideoGain(InstanceID, CurrentRedVideoGain);
}

void 
CtlRenderingControl::_threadSetRedVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredRedVideoGain = pAction->getArgument<ui2>("DesiredRedVideoGain");
    _ansSetRedVideoGain(InstanceID, DesiredRedVideoGain);
}

void 
CtlRenderingControl::_threadGetGreenVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentGreenVideoGain = pAction->getArgument<ui2>("CurrentGreenVideoGain");
    _ansGetGreenVideoGain(InstanceID, CurrentGreenVideoGain);
}

void 
CtlRenderingControl::_threadSetGreenVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredGreenVideoGain = pAction->getArgument<ui2>("DesiredGreenVideoGain");
    _ansSetGreenVideoGain(InstanceID, DesiredGreenVideoGain);
}

void 
CtlRenderingControl::_threadGetBlueVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBlueVideoGain = pAction->getArgument<ui2>("CurrentBlueVideoGain");
    _ansGetBlueVideoGain(InstanceID, CurrentBlueVideoGain);
}

void 
CtlRenderingControl::_threadSetBlueVideoGain(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBlueVideoGain = pAction->getArgument<ui2>("DesiredBlueVideoGain");
    _ansSetBlueVideoGain(InstanceID, DesiredBlueVideoGain);
}

void 
CtlRenderingControl::_threadGetRedVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentRedVideoBlackLevel = pAction->getArgument<ui2>("CurrentRedVideoBlackLevel");
    _ansGetRedVideoBlackLevel(InstanceID, CurrentRedVideoBlackLevel);
}

void 
CtlRenderingControl::_threadSetRedVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredRedVideoBlackLevel = pAction->getArgument<ui2>("DesiredRedVideoBlackLevel");
    _ansSetRedVideoBlackLevel(InstanceID, DesiredRedVideoBlackLevel);
}

void 
CtlRenderingControl::_threadGetGreenVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentGreenVideoBlackLevel = pAction->getArgument<ui2>("CurrentGreenVideoBlackLevel");
    _ansGetGreenVideoBlackLevel(InstanceID, CurrentGreenVideoBlackLevel);
}

void 
CtlRenderingControl::_threadSetGreenVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredGreenVideoBlackLevel = pAction->getArgument<ui2>("DesiredGreenVideoBlackLevel");
    _ansSetGreenVideoBlackLevel(InstanceID, DesiredGreenVideoBlackLevel);
}

void 
CtlRenderingControl::_threadGetBlueVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBlueVideoBlackLevel = pAction->getArgument<ui2>("CurrentBlueVideoBlackLevel");
    _ansGetBlueVideoBlackLevel(InstanceID, CurrentBlueVideoBlackLevel);
}

void 
CtlRenderingControl::_threadSetBlueVideoBlackLevel(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBlueVideoBlackLevel = pAction->getArgument<ui2>("DesiredBlueVideoBlackLevel");
    _ansSetBlueVideoBlackLevel(InstanceID, DesiredBlueVideoBlackLevel);
}

void 
CtlRenderingControl::_threadGetColorTemperature (Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentColorTemperature = pAction->getArgument<ui2>("CurrentColorTemperature");
    _ansGetColorTemperature (InstanceID, CurrentColorTemperature);
}

void 
CtlRenderingControl::_threadSetColorTemperature(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredColorTemperature = pAction->getArgument<ui2>("DesiredColorTemperature");
    _ansSetColorTemperature(InstanceID, DesiredColorTemperature);
}

void 
CtlRenderingControl::_threadGetHorizontalKeystone(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 CurrentHorizontalKeystone = pAction->getArgument<i2>("CurrentHorizontalKeystone");
    _ansGetHorizontalKeystone(InstanceID, CurrentHorizontalKeystone);
}

void 
CtlRenderingControl::_threadSetHorizontalKeystone(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 DesiredHorizontalKeystone = pAction->getArgument<i2>("DesiredHorizontalKeystone");
    _ansSetHorizontalKeystone(InstanceID, DesiredHorizontalKeystone);
}

void 
CtlRenderingControl::_threadGetVerticalKeystone(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 CurrentVerticalKeystone = pAction->getArgument<i2>("CurrentVerticalKeystone");
    _ansGetVerticalKeystone(InstanceID, CurrentVerticalKeystone);
}

void 
CtlRenderingControl::_threadSetVerticalKeystone(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 DesiredVerticalKeystone = pAction->getArgument<i2>("DesiredVerticalKeystone");
    _ansSetVerticalKeystone(InstanceID, DesiredVerticalKeystone);
}

void 
CtlRenderingControl::_threadGetMute(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool CurrentMute = pAction->getArgument<bool>("CurrentMute");
    _ansGetMute(InstanceID, Channel, CurrentMute);
}

void 
CtlRenderingControl::_threadSetMute(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool DesiredMute = pAction->getArgument<bool>("DesiredMute");
    _ansSetMute(InstanceID, Channel, DesiredMute);
}

void 
CtlRenderingControl::_threadGetVolume(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    ui2 CurrentVolume = pAction->getArgument<ui2>("CurrentVolume");
    _ansGetVolume(InstanceID, Channel, CurrentVolume);
}

void 
CtlRenderingControl::_threadSetVolume(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    ui2 DesiredVolume = pAction->getArgument<ui2>("DesiredVolume");
    _ansSetVolume(InstanceID, Channel, DesiredVolume);
}

void 
CtlRenderingControl::_threadGetVolumeDB(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 CurrentVolume = pAction->getArgument<i2>("CurrentVolume");
    _ansGetVolumeDB(InstanceID, Channel, CurrentVolume);
}

void 
CtlRenderingControl::_threadSetVolumeDB(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 DesiredVolume = pAction->getArgument<i2>("DesiredVolume");
    _ansSetVolumeDB(InstanceID, Channel, DesiredVolume);
}

void 
CtlRenderingControl::_threadGetVolumeDBRange(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 MinValue = pAction->getArgument<i2>("MinValue");
    i2 MaxValue = pAction->getArgument<i2>("MaxValue");
    _ansGetVolumeDBRange(InstanceID, Channel, MinValue, MaxValue);
}

void 
CtlRenderingControl::_threadGetLoudness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool CurrentLoudness = pAction->getArgument<bool>("CurrentLoudness");
    _ansGetLoudness(InstanceID, Channel, CurrentLoudness);
}

void 
CtlRenderingControl::_threadSetLoudness(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool DesiredLoudness = pAction->getArgument<bool>("DesiredLoudness");
    _ansSetLoudness(InstanceID, Channel, DesiredLoudness);
}


void
CtlContentDirectory::GetSearchCapabilities(std::string& SearchCaps)
{
    Action* pAction = _pService->getAction("GetSearchCapabilities")->clone();
    _pService->sendAction(pAction);
    SearchCaps = pAction->getArgument<std::string>("SearchCaps");
}

void
CtlContentDirectory::GetSortCapabilities(std::string& SortCaps)
{
    Action* pAction = _pService->getAction("GetSortCapabilities")->clone();
    _pService->sendAction(pAction);
    SortCaps = pAction->getArgument<std::string>("SortCaps");
}

void
CtlContentDirectory::GetSystemUpdateID(ui4& Id)
{
    Action* pAction = _pService->getAction("GetSystemUpdateID")->clone();
    _pService->sendAction(pAction);
    Id = pAction->getArgument<ui4>("Id");
}

void
CtlContentDirectory::Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    Action* pAction = _pService->getAction("Browse")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("BrowseFlag", BrowseFlag);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    _pService->sendAction(pAction);
    Result = pAction->getArgument<std::string>("Result");
    NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    UpdateID = pAction->getArgument<ui4>("UpdateID");
}

void
CtlContentDirectory::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    Action* pAction = _pService->getAction("Search")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("SearchCriteria", SearchCriteria);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    _pService->sendAction(pAction);
    Result = pAction->getArgument<std::string>("Result");
    NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    UpdateID = pAction->getArgument<ui4>("UpdateID");
}

void
CtlContentDirectory::CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result)
{
    Action* pAction = _pService->getAction("CreateObject")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("Elements", Elements);
    _pService->sendAction(pAction);
    ObjectID = pAction->getArgument<std::string>("ObjectID");
    Result = pAction->getArgument<std::string>("Result");
}

void
CtlContentDirectory::DestroyObject(const std::string& ObjectID)
{
    Action* pAction = _pService->getAction("DestroyObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    _pService->sendAction(pAction);
}

void
CtlContentDirectory::UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
    Action* pAction = _pService->getAction("UpdateObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("CurrentTagValue", CurrentTagValue);
    pAction->setArgument<std::string>("NewTagValue", NewTagValue);
    _pService->sendAction(pAction);
}

void
CtlContentDirectory::ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID)
{
    Action* pAction = _pService->getAction("ImportResource")->clone();
    pAction->setArgument<uri>("SourceURI", SourceURI);
    pAction->setArgument<uri>("DestinationURI", DestinationURI);
    _pService->sendAction(pAction);
    TransferID = pAction->getArgument<ui4>("TransferID");
}

void
CtlContentDirectory::GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal)
{
    Action* pAction = _pService->getAction("GetTransferProgress")->clone();
    pAction->setArgument<ui4>("TransferID", TransferID);
    _pService->sendAction(pAction);
    TransferStatus = pAction->getArgument<std::string>("TransferStatus");
    TransferLength = pAction->getArgument<std::string>("TransferLength");
    TransferTotal = pAction->getArgument<std::string>("TransferTotal");
}

void
CtlContentDirectory::DeleteResource(const uri& ResourceURI)
{
    Action* pAction = _pService->getAction("DeleteResource")->clone();
    pAction->setArgument<uri>("ResourceURI", ResourceURI);
    _pService->sendAction(pAction);
}

void
CtlContentDirectory::CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID)
{
    Action* pAction = _pService->getAction("CreateReference")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    _pService->sendAction(pAction);
    NewID = pAction->getArgument<std::string>("NewID");
}

std::string
CtlContentDirectory::_getTransferIDs()
{
    return _pService->getStateVar<std::string>("TransferIDs");
}

ui4
CtlContentDirectory::_getSystemUpdateID()
{
    return _pService->getStateVar<ui4>("SystemUpdateID");
}

std::string
CtlContentDirectory::_getContainerUpdateIDs()
{
    return _pService->getStateVar<std::string>("ContainerUpdateIDs");
}


void 
CtlContentDirectory::_reqGetSearchCapabilities()
{
    Action* pAction = _pService->getAction("GetSearchCapabilities")->clone();
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadGetSearchCapabilities, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqGetSortCapabilities()
{
    Action* pAction = _pService->getAction("GetSortCapabilities")->clone();
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadGetSortCapabilities, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqGetSystemUpdateID()
{
    Action* pAction = _pService->getAction("GetSystemUpdateID")->clone();
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadGetSystemUpdateID, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria)
{
    Action* pAction = _pService->getAction("Browse")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("BrowseFlag", BrowseFlag);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadBrowse, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria)
{
    Action* pAction = _pService->getAction("Search")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("SearchCriteria", SearchCriteria);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadSearch, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqCreateObject(const std::string& ContainerID, const std::string& Elements)
{
    Action* pAction = _pService->getAction("CreateObject")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("Elements", Elements);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadCreateObject, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqDestroyObject(const std::string& ObjectID)
{
    Action* pAction = _pService->getAction("DestroyObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadDestroyObject, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
    Action* pAction = _pService->getAction("UpdateObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("CurrentTagValue", CurrentTagValue);
    pAction->setArgument<std::string>("NewTagValue", NewTagValue);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadUpdateObject, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqImportResource(const uri& SourceURI, const uri& DestinationURI)
{
    Action* pAction = _pService->getAction("ImportResource")->clone();
    pAction->setArgument<uri>("SourceURI", SourceURI);
    pAction->setArgument<uri>("DestinationURI", DestinationURI);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadImportResource, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqGetTransferProgress(const ui4& TransferID)
{
    Action* pAction = _pService->getAction("GetTransferProgress")->clone();
    pAction->setArgument<ui4>("TransferID", TransferID);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadGetTransferProgress, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqDeleteResource(const uri& ResourceURI)
{
    Action* pAction = _pService->getAction("DeleteResource")->clone();
    pAction->setArgument<uri>("ResourceURI", ResourceURI);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadDeleteResource, pAction);
    thread.start();
}

void 
CtlContentDirectory::_reqCreateReference(const std::string& ContainerID, const std::string& ObjectID)
{
    Action* pAction = _pService->getAction("CreateReference")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    ActionThread<CtlContentDirectory> thread(this, &CtlContentDirectory::_threadCreateReference, pAction);
    thread.start();
}

void 
CtlContentDirectory::_threadGetSearchCapabilities(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string SearchCaps = pAction->getArgument<std::string>("SearchCaps");
    _ansGetSearchCapabilities(SearchCaps);
}

void 
CtlContentDirectory::_threadGetSortCapabilities(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string SortCaps = pAction->getArgument<std::string>("SortCaps");
    _ansGetSortCapabilities(SortCaps);
}

void 
CtlContentDirectory::_threadGetSystemUpdateID(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 Id = pAction->getArgument<ui4>("Id");
    _ansGetSystemUpdateID(Id);
}

void 
CtlContentDirectory::_threadBrowse(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string BrowseFlag = pAction->getArgument<std::string>("BrowseFlag");
    std::string Filter = pAction->getArgument<std::string>("Filter");
    ui4 StartingIndex = pAction->getArgument<ui4>("StartingIndex");
    ui4 RequestedCount = pAction->getArgument<ui4>("RequestedCount");
    std::string SortCriteria = pAction->getArgument<std::string>("SortCriteria");
    std::string Result = pAction->getArgument<std::string>("Result");
    ui4 NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    ui4 TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    ui4 UpdateID = pAction->getArgument<ui4>("UpdateID");
    _ansBrowse(ObjectID, BrowseFlag, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
}

void 
CtlContentDirectory::_threadSearch(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
    std::string SearchCriteria = pAction->getArgument<std::string>("SearchCriteria");
    std::string Filter = pAction->getArgument<std::string>("Filter");
    ui4 StartingIndex = pAction->getArgument<ui4>("StartingIndex");
    ui4 RequestedCount = pAction->getArgument<ui4>("RequestedCount");
    std::string SortCriteria = pAction->getArgument<std::string>("SortCriteria");
    std::string Result = pAction->getArgument<std::string>("Result");
    ui4 NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    ui4 TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    ui4 UpdateID = pAction->getArgument<ui4>("UpdateID");
    _ansSearch(ContainerID, SearchCriteria, Filter, StartingIndex, RequestedCount, SortCriteria, Result, NumberReturned, TotalMatches, UpdateID);
}

void 
CtlContentDirectory::_threadCreateObject(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
    std::string Elements = pAction->getArgument<std::string>("Elements");
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string Result = pAction->getArgument<std::string>("Result");
    _ansCreateObject(ContainerID, Elements, ObjectID, Result);
}

void 
CtlContentDirectory::_threadDestroyObject(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    _ansDestroyObject(ObjectID);
}

void 
CtlContentDirectory::_threadUpdateObject(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string CurrentTagValue = pAction->getArgument<std::string>("CurrentTagValue");
    std::string NewTagValue = pAction->getArgument<std::string>("NewTagValue");
    _ansUpdateObject(ObjectID, CurrentTagValue, NewTagValue);
}

void 
CtlContentDirectory::_threadImportResource(Action* pAction)
{
    _pService->sendAction(pAction);
    uri SourceURI = pAction->getArgument<uri>("SourceURI");
    uri DestinationURI = pAction->getArgument<uri>("DestinationURI");
    ui4 TransferID = pAction->getArgument<ui4>("TransferID");
    _ansImportResource(SourceURI, DestinationURI, TransferID);
}

void 
CtlContentDirectory::_threadGetTransferProgress(Action* pAction)
{
    _pService->sendAction(pAction);
    ui4 TransferID = pAction->getArgument<ui4>("TransferID");
    std::string TransferStatus = pAction->getArgument<std::string>("TransferStatus");
    std::string TransferLength = pAction->getArgument<std::string>("TransferLength");
    std::string TransferTotal = pAction->getArgument<std::string>("TransferTotal");
    _ansGetTransferProgress(TransferID, TransferStatus, TransferLength, TransferTotal);
}

void 
CtlContentDirectory::_threadDeleteResource(Action* pAction)
{
    _pService->sendAction(pAction);
    uri ResourceURI = pAction->getArgument<uri>("ResourceURI");
    _ansDeleteResource(ResourceURI);
}

void 
CtlContentDirectory::_threadCreateReference(Action* pAction)
{
    _pService->sendAction(pAction);
    std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string NewID = pAction->getArgument<std::string>("NewID");
    _ansCreateReference(ContainerID, ObjectID, NewID);
}


} // namespace Av
} // namespace Omm
