#include "UpnpAvControllers.h"

using namespace Jamm;
using namespace Jamm::Av;

void
AVTransportController::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    Action* pAction = m_pService->getAction("SetAVTransportURI")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("CurrentURI", CurrentURI);
    pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
    m_pService->sendAction(pAction);
}

void
AVTransportController::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
    Action* pAction = m_pService->getAction("GetMediaInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
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
AVTransportController::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
    Action* pAction = m_pService->getAction("GetTransportInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentTransportState = pAction->getArgument<std::string>("CurrentTransportState");
    CurrentTransportStatus = pAction->getArgument<std::string>("CurrentTransportStatus");
    CurrentSpeed = pAction->getArgument<std::string>("CurrentSpeed");
}

void
AVTransportController::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
    Action* pAction = m_pService->getAction("GetPositionInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
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
AVTransportController::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
    Action* pAction = m_pService->getAction("GetDeviceCapabilities")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    PlayMedia = pAction->getArgument<std::string>("PlayMedia");
    RecMedia = pAction->getArgument<std::string>("RecMedia");
    RecQualityModes = pAction->getArgument<std::string>("RecQualityModes");
}

void
AVTransportController::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
    Action* pAction = m_pService->getAction("GetTransportSettings")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    PlayMode = pAction->getArgument<std::string>("PlayMode");
    RecQualityMode = pAction->getArgument<std::string>("RecQualityMode");
}

void
AVTransportController::Stop(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Stop")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
}

void
AVTransportController::Play(const ui4& InstanceID, const std::string& Speed)
{
    Action* pAction = m_pService->getAction("Play")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Speed", Speed);
    m_pService->sendAction(pAction);
}

void
AVTransportController::Pause(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Pause")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
}

void
AVTransportController::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    Action* pAction = m_pService->getAction("Seek")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Unit", Unit);
    pAction->setArgument<std::string>("Target", Target);
    m_pService->sendAction(pAction);
}

void
AVTransportController::Next(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Next")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
}

void
AVTransportController::Previous(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Previous")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
}

std::string
AVTransportController::_getLastChange()
{
    return m_pService->getStateVar<std::string>("LastChange");
}


void 
AVTransportController::_reqSetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
    Action* pAction = m_pService->getAction("SetAVTransportURI")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("CurrentURI", CurrentURI);
    pAction->setArgument<std::string>("CurrentURIMetaData", CurrentURIMetaData);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadSetAVTransportURI, pAction);
    thread.start();
}

void 
AVTransportController::_reqGetMediaInfo(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetMediaInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadGetMediaInfo, pAction);
    thread.start();
}

void 
AVTransportController::_reqGetTransportInfo(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetTransportInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadGetTransportInfo, pAction);
    thread.start();
}

void 
AVTransportController::_reqGetPositionInfo(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetPositionInfo")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadGetPositionInfo, pAction);
    thread.start();
}

void 
AVTransportController::_reqGetDeviceCapabilities(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetDeviceCapabilities")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadGetDeviceCapabilities, pAction);
    thread.start();
}

void 
AVTransportController::_reqGetTransportSettings(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetTransportSettings")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadGetTransportSettings, pAction);
    thread.start();
}

void 
AVTransportController::_reqStop(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Stop")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadStop, pAction);
    thread.start();
}

void 
AVTransportController::_reqPlay(const ui4& InstanceID, const std::string& Speed)
{
    Action* pAction = m_pService->getAction("Play")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Speed", Speed);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadPlay, pAction);
    thread.start();
}

void 
AVTransportController::_reqPause(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Pause")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadPause, pAction);
    thread.start();
}

void 
AVTransportController::_reqSeek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
    Action* pAction = m_pService->getAction("Seek")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Unit", Unit);
    pAction->setArgument<std::string>("Target", Target);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadSeek, pAction);
    thread.start();
}

void 
AVTransportController::_reqNext(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Next")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadNext, pAction);
    thread.start();
}

void 
AVTransportController::_reqPrevious(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("Previous")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<AVTransportController> thread(this, &AVTransportController::_threadPrevious, pAction);
    thread.start();
}

void 
AVTransportController::_threadSetAVTransportURI(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentURI = pAction->getArgument<std::string>("CurrentURI");
    std::string CurrentURIMetaData = pAction->getArgument<std::string>("CurrentURIMetaData");
    _ansSetAVTransportURI(InstanceID, CurrentURI, CurrentURIMetaData);
}

void 
AVTransportController::_threadGetMediaInfo(Action* pAction)
{
    m_pService->sendAction(pAction);
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
AVTransportController::_threadGetTransportInfo(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentTransportState = pAction->getArgument<std::string>("CurrentTransportState");
    std::string CurrentTransportStatus = pAction->getArgument<std::string>("CurrentTransportStatus");
    std::string CurrentSpeed = pAction->getArgument<std::string>("CurrentSpeed");
    _ansGetTransportInfo(InstanceID, CurrentTransportState, CurrentTransportStatus, CurrentSpeed);
}

void 
AVTransportController::_threadGetPositionInfo(Action* pAction)
{
    m_pService->sendAction(pAction);
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
AVTransportController::_threadGetDeviceCapabilities(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PlayMedia = pAction->getArgument<std::string>("PlayMedia");
    std::string RecMedia = pAction->getArgument<std::string>("RecMedia");
    std::string RecQualityModes = pAction->getArgument<std::string>("RecQualityModes");
    _ansGetDeviceCapabilities(InstanceID, PlayMedia, RecMedia, RecQualityModes);
}

void 
AVTransportController::_threadGetTransportSettings(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PlayMode = pAction->getArgument<std::string>("PlayMode");
    std::string RecQualityMode = pAction->getArgument<std::string>("RecQualityMode");
    _ansGetTransportSettings(InstanceID, PlayMode, RecQualityMode);
}

void 
AVTransportController::_threadStop(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansStop(InstanceID);
}

void 
AVTransportController::_threadPlay(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Speed = pAction->getArgument<std::string>("Speed");
    _ansPlay(InstanceID, Speed);
}

void 
AVTransportController::_threadPause(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansPause(InstanceID);
}

void 
AVTransportController::_threadSeek(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Unit = pAction->getArgument<std::string>("Unit");
    std::string Target = pAction->getArgument<std::string>("Target");
    _ansSeek(InstanceID, Unit, Target);
}

void 
AVTransportController::_threadNext(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansNext(InstanceID);
}

void 
AVTransportController::_threadPrevious(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    _ansPrevious(InstanceID);
}



void
ConnectionManagerController::GetProtocolInfo(std::string& Source, std::string& Sink)
{
    Action* pAction = m_pService->getAction("GetProtocolInfo")->clone();
    m_pService->sendAction(pAction);
    Source = pAction->getArgument<std::string>("Source");
    Sink = pAction->getArgument<std::string>("Sink");
}

void
ConnectionManagerController::ConnectionComplete(const i4& ConnectionID)
{
    Action* pAction = m_pService->getAction("ConnectionComplete")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    m_pService->sendAction(pAction);
}

void
ConnectionManagerController::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
    Action* pAction = m_pService->getAction("GetCurrentConnectionIDs")->clone();
    m_pService->sendAction(pAction);
    ConnectionIDs = pAction->getArgument<std::string>("ConnectionIDs");
}

void
ConnectionManagerController::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
    Action* pAction = m_pService->getAction("GetCurrentConnectionInfo")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    m_pService->sendAction(pAction);
    RcsID = pAction->getArgument<i4>("RcsID");
    AVTransportID = pAction->getArgument<i4>("AVTransportID");
    ProtocolInfo = pAction->getArgument<std::string>("ProtocolInfo");
    PeerConnectionManager = pAction->getArgument<std::string>("PeerConnectionManager");
    PeerConnectionID = pAction->getArgument<i4>("PeerConnectionID");
    Direction = pAction->getArgument<std::string>("Direction");
    Status = pAction->getArgument<std::string>("Status");
}

std::string
ConnectionManagerController::_getSourceProtocolInfo()
{
    return m_pService->getStateVar<std::string>("SourceProtocolInfo");
}

std::string
ConnectionManagerController::_getSinkProtocolInfo()
{
    return m_pService->getStateVar<std::string>("SinkProtocolInfo");
}

std::string
ConnectionManagerController::_getCurrentConnectionIDs()
{
    return m_pService->getStateVar<std::string>("CurrentConnectionIDs");
}


void 
ConnectionManagerController::_reqGetProtocolInfo()
{
    Action* pAction = m_pService->getAction("GetProtocolInfo")->clone();
    ActionThread<ConnectionManagerController> thread(this, &ConnectionManagerController::_threadGetProtocolInfo, pAction);
    thread.start();
}

void 
ConnectionManagerController::_reqConnectionComplete(const i4& ConnectionID)
{
    Action* pAction = m_pService->getAction("ConnectionComplete")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    ActionThread<ConnectionManagerController> thread(this, &ConnectionManagerController::_threadConnectionComplete, pAction);
    thread.start();
}

void 
ConnectionManagerController::_reqGetCurrentConnectionIDs()
{
    Action* pAction = m_pService->getAction("GetCurrentConnectionIDs")->clone();
    ActionThread<ConnectionManagerController> thread(this, &ConnectionManagerController::_threadGetCurrentConnectionIDs, pAction);
    thread.start();
}

void 
ConnectionManagerController::_reqGetCurrentConnectionInfo(const i4& ConnectionID)
{
    Action* pAction = m_pService->getAction("GetCurrentConnectionInfo")->clone();
    pAction->setArgument<i4>("ConnectionID", ConnectionID);
    ActionThread<ConnectionManagerController> thread(this, &ConnectionManagerController::_threadGetCurrentConnectionInfo, pAction);
    thread.start();
}

void 
ConnectionManagerController::_threadGetProtocolInfo(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string Source = pAction->getArgument<std::string>("Source");
    std::string Sink = pAction->getArgument<std::string>("Sink");
    _ansGetProtocolInfo(Source, Sink);
}

void 
ConnectionManagerController::_threadConnectionComplete(Action* pAction)
{
    m_pService->sendAction(pAction);
    i4 ConnectionID = pAction->getArgument<i4>("ConnectionID");
    _ansConnectionComplete(ConnectionID);
}

void 
ConnectionManagerController::_threadGetCurrentConnectionIDs(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string ConnectionIDs = pAction->getArgument<std::string>("ConnectionIDs");
    _ansGetCurrentConnectionIDs(ConnectionIDs);
}

void 
ConnectionManagerController::_threadGetCurrentConnectionInfo(Action* pAction)
{
    m_pService->sendAction(pAction);
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
RenderingControlController::ListPresets(const ui4& InstanceID, std::string& CurrentPresetNameList)
{
    Action* pAction = m_pService->getAction("ListPresets")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentPresetNameList = pAction->getArgument<std::string>("CurrentPresetNameList");
}

void
RenderingControlController::SelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
    Action* pAction = m_pService->getAction("SelectPreset")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("PresetName", PresetName);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetBrightness(const ui4& InstanceID, ui2& CurrentBrightness)
{
    Action* pAction = m_pService->getAction("GetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentBrightness = pAction->getArgument<ui2>("CurrentBrightness");
}

void
RenderingControlController::SetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
    Action* pAction = m_pService->getAction("SetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBrightness", DesiredBrightness);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetContrast(const ui4& InstanceID, ui2& CurrentContrast)
{
    Action* pAction = m_pService->getAction("GetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentContrast = pAction->getArgument<ui2>("CurrentContrast");
}

void
RenderingControlController::SetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
    Action* pAction = m_pService->getAction("SetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredContrast", DesiredContrast);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetSharpness(const ui4& InstanceID, ui2& CurrentSharpness)
{
    Action* pAction = m_pService->getAction("GetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentSharpness = pAction->getArgument<ui2>("CurrentSharpness");
}

void
RenderingControlController::SetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
    Action* pAction = m_pService->getAction("SetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredSharpness", DesiredSharpness);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetRedVideoGain(const ui4& InstanceID, ui2& CurrentRedVideoGain)
{
    Action* pAction = m_pService->getAction("GetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentRedVideoGain = pAction->getArgument<ui2>("CurrentRedVideoGain");
}

void
RenderingControlController::SetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
    Action* pAction = m_pService->getAction("SetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoGain", DesiredRedVideoGain);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetGreenVideoGain(const ui4& InstanceID, ui2& CurrentGreenVideoGain)
{
    Action* pAction = m_pService->getAction("GetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentGreenVideoGain = pAction->getArgument<ui2>("CurrentGreenVideoGain");
}

void
RenderingControlController::SetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
    Action* pAction = m_pService->getAction("SetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoGain", DesiredGreenVideoGain);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetBlueVideoGain(const ui4& InstanceID, ui2& CurrentBlueVideoGain)
{
    Action* pAction = m_pService->getAction("GetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentBlueVideoGain = pAction->getArgument<ui2>("CurrentBlueVideoGain");
}

void
RenderingControlController::SetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
    Action* pAction = m_pService->getAction("SetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoGain", DesiredBlueVideoGain);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetRedVideoBlackLevel(const ui4& InstanceID, ui2& CurrentRedVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("GetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentRedVideoBlackLevel = pAction->getArgument<ui2>("CurrentRedVideoBlackLevel");
}

void
RenderingControlController::SetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoBlackLevel", DesiredRedVideoBlackLevel);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetGreenVideoBlackLevel(const ui4& InstanceID, ui2& CurrentGreenVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("GetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentGreenVideoBlackLevel = pAction->getArgument<ui2>("CurrentGreenVideoBlackLevel");
}

void
RenderingControlController::SetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoBlackLevel", DesiredGreenVideoBlackLevel);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetBlueVideoBlackLevel(const ui4& InstanceID, ui2& CurrentBlueVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("GetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentBlueVideoBlackLevel = pAction->getArgument<ui2>("CurrentBlueVideoBlackLevel");
}

void
RenderingControlController::SetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoBlackLevel", DesiredBlueVideoBlackLevel);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetColorTemperature (const ui4& InstanceID, ui2& CurrentColorTemperature)
{
    Action* pAction = m_pService->getAction("GetColorTemperature ")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentColorTemperature = pAction->getArgument<ui2>("CurrentColorTemperature");
}

void
RenderingControlController::SetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
    Action* pAction = m_pService->getAction("SetColorTemperature")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredColorTemperature", DesiredColorTemperature);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetHorizontalKeystone(const ui4& InstanceID, i2& CurrentHorizontalKeystone)
{
    Action* pAction = m_pService->getAction("GetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentHorizontalKeystone = pAction->getArgument<i2>("CurrentHorizontalKeystone");
}

void
RenderingControlController::SetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
    Action* pAction = m_pService->getAction("SetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredHorizontalKeystone", DesiredHorizontalKeystone);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetVerticalKeystone(const ui4& InstanceID, i2& CurrentVerticalKeystone)
{
    Action* pAction = m_pService->getAction("GetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    m_pService->sendAction(pAction);
    CurrentVerticalKeystone = pAction->getArgument<i2>("CurrentVerticalKeystone");
}

void
RenderingControlController::SetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
    Action* pAction = m_pService->getAction("SetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredVerticalKeystone", DesiredVerticalKeystone);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetMute(const ui4& InstanceID, const std::string& Channel, bool& CurrentMute)
{
    Action* pAction = m_pService->getAction("GetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    m_pService->sendAction(pAction);
    CurrentMute = pAction->getArgument<bool>("CurrentMute");
}

void
RenderingControlController::SetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
    Action* pAction = m_pService->getAction("SetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredMute", DesiredMute);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetVolume(const ui4& InstanceID, const std::string& Channel, ui2& CurrentVolume)
{
    Action* pAction = m_pService->getAction("GetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    m_pService->sendAction(pAction);
    CurrentVolume = pAction->getArgument<ui2>("CurrentVolume");
}

void
RenderingControlController::SetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Action* pAction = m_pService->getAction("SetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<ui2>("DesiredVolume", DesiredVolume);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetVolumeDB(const ui4& InstanceID, const std::string& Channel, i2& CurrentVolume)
{
    Action* pAction = m_pService->getAction("GetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    m_pService->sendAction(pAction);
    CurrentVolume = pAction->getArgument<i2>("CurrentVolume");
}

void
RenderingControlController::SetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
    Action* pAction = m_pService->getAction("SetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<i2>("DesiredVolume", DesiredVolume);
    m_pService->sendAction(pAction);
}

void
RenderingControlController::GetVolumeDBRange(const ui4& InstanceID, const std::string& Channel, i2& MinValue, i2& MaxValue)
{
    Action* pAction = m_pService->getAction("GetVolumeDBRange")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    m_pService->sendAction(pAction);
    MinValue = pAction->getArgument<i2>("MinValue");
    MaxValue = pAction->getArgument<i2>("MaxValue");
}

void
RenderingControlController::GetLoudness(const ui4& InstanceID, const std::string& Channel, bool& CurrentLoudness)
{
    Action* pAction = m_pService->getAction("GetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    m_pService->sendAction(pAction);
    CurrentLoudness = pAction->getArgument<bool>("CurrentLoudness");
}

void
RenderingControlController::SetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
    Action* pAction = m_pService->getAction("SetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredLoudness", DesiredLoudness);
    m_pService->sendAction(pAction);
}

std::string
RenderingControlController::_getLastChange()
{
    return m_pService->getStateVar<std::string>("LastChange");
}


void 
RenderingControlController::_reqListPresets(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("ListPresets")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadListPresets, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSelectPreset(const ui4& InstanceID, const std::string& PresetName)
{
    Action* pAction = m_pService->getAction("SelectPreset")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("PresetName", PresetName);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSelectPreset, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetBrightness(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetBrightness, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetBrightness(const ui4& InstanceID, const ui2& DesiredBrightness)
{
    Action* pAction = m_pService->getAction("SetBrightness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBrightness", DesiredBrightness);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetBrightness, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetContrast(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetContrast, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetContrast(const ui4& InstanceID, const ui2& DesiredContrast)
{
    Action* pAction = m_pService->getAction("SetContrast")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredContrast", DesiredContrast);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetContrast, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetSharpness(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetSharpness, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetSharpness(const ui4& InstanceID, const ui2& DesiredSharpness)
{
    Action* pAction = m_pService->getAction("SetSharpness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredSharpness", DesiredSharpness);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetSharpness, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetRedVideoGain(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetRedVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetRedVideoGain(const ui4& InstanceID, const ui2& DesiredRedVideoGain)
{
    Action* pAction = m_pService->getAction("SetRedVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoGain", DesiredRedVideoGain);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetRedVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetGreenVideoGain(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetGreenVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetGreenVideoGain(const ui4& InstanceID, const ui2& DesiredGreenVideoGain)
{
    Action* pAction = m_pService->getAction("SetGreenVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoGain", DesiredGreenVideoGain);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetGreenVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetBlueVideoGain(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetBlueVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetBlueVideoGain(const ui4& InstanceID, const ui2& DesiredBlueVideoGain)
{
    Action* pAction = m_pService->getAction("SetBlueVideoGain")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoGain", DesiredBlueVideoGain);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetBlueVideoGain, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetRedVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetRedVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetRedVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredRedVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetRedVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredRedVideoBlackLevel", DesiredRedVideoBlackLevel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetRedVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetGreenVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetGreenVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetGreenVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredGreenVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetGreenVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredGreenVideoBlackLevel", DesiredGreenVideoBlackLevel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetGreenVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetBlueVideoBlackLevel(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetBlueVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetBlueVideoBlackLevel(const ui4& InstanceID, const ui2& DesiredBlueVideoBlackLevel)
{
    Action* pAction = m_pService->getAction("SetBlueVideoBlackLevel")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredBlueVideoBlackLevel", DesiredBlueVideoBlackLevel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetBlueVideoBlackLevel, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetColorTemperature (const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetColorTemperature ")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetColorTemperature , pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetColorTemperature(const ui4& InstanceID, const ui2& DesiredColorTemperature)
{
    Action* pAction = m_pService->getAction("SetColorTemperature")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<ui2>("DesiredColorTemperature", DesiredColorTemperature);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetColorTemperature, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetHorizontalKeystone(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetHorizontalKeystone, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetHorizontalKeystone(const ui4& InstanceID, const i2& DesiredHorizontalKeystone)
{
    Action* pAction = m_pService->getAction("SetHorizontalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredHorizontalKeystone", DesiredHorizontalKeystone);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetHorizontalKeystone, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetVerticalKeystone(const ui4& InstanceID)
{
    Action* pAction = m_pService->getAction("GetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetVerticalKeystone, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetVerticalKeystone(const ui4& InstanceID, const i2& DesiredVerticalKeystone)
{
    Action* pAction = m_pService->getAction("SetVerticalKeystone")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<i2>("DesiredVerticalKeystone", DesiredVerticalKeystone);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetVerticalKeystone, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetMute(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = m_pService->getAction("GetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetMute, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetMute(const ui4& InstanceID, const std::string& Channel, const bool& DesiredMute)
{
    Action* pAction = m_pService->getAction("SetMute")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredMute", DesiredMute);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetMute, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetVolume(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = m_pService->getAction("GetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetVolume, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetVolume(const ui4& InstanceID, const std::string& Channel, const ui2& DesiredVolume)
{
    Action* pAction = m_pService->getAction("SetVolume")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<ui2>("DesiredVolume", DesiredVolume);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetVolume, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetVolumeDB(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = m_pService->getAction("GetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetVolumeDB, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetVolumeDB(const ui4& InstanceID, const std::string& Channel, const i2& DesiredVolume)
{
    Action* pAction = m_pService->getAction("SetVolumeDB")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<i2>("DesiredVolume", DesiredVolume);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetVolumeDB, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetVolumeDBRange(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = m_pService->getAction("GetVolumeDBRange")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetVolumeDBRange, pAction);
    thread.start();
}

void 
RenderingControlController::_reqGetLoudness(const ui4& InstanceID, const std::string& Channel)
{
    Action* pAction = m_pService->getAction("GetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadGetLoudness, pAction);
    thread.start();
}

void 
RenderingControlController::_reqSetLoudness(const ui4& InstanceID, const std::string& Channel, const bool& DesiredLoudness)
{
    Action* pAction = m_pService->getAction("SetLoudness")->clone();
    pAction->setArgument<ui4>("InstanceID", InstanceID);
    pAction->setArgument<std::string>("Channel", Channel);
    pAction->setArgument<bool>("DesiredLoudness", DesiredLoudness);
    ActionThread<RenderingControlController> thread(this, &RenderingControlController::_threadSetLoudness, pAction);
    thread.start();
}

void 
RenderingControlController::_threadListPresets(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string CurrentPresetNameList = pAction->getArgument<std::string>("CurrentPresetNameList");
    _ansListPresets(InstanceID, CurrentPresetNameList);
}

void 
RenderingControlController::_threadSelectPreset(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string PresetName = pAction->getArgument<std::string>("PresetName");
    _ansSelectPreset(InstanceID, PresetName);
}

void 
RenderingControlController::_threadGetBrightness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBrightness = pAction->getArgument<ui2>("CurrentBrightness");
    _ansGetBrightness(InstanceID, CurrentBrightness);
}

void 
RenderingControlController::_threadSetBrightness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBrightness = pAction->getArgument<ui2>("DesiredBrightness");
    _ansSetBrightness(InstanceID, DesiredBrightness);
}

void 
RenderingControlController::_threadGetContrast(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentContrast = pAction->getArgument<ui2>("CurrentContrast");
    _ansGetContrast(InstanceID, CurrentContrast);
}

void 
RenderingControlController::_threadSetContrast(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredContrast = pAction->getArgument<ui2>("DesiredContrast");
    _ansSetContrast(InstanceID, DesiredContrast);
}

void 
RenderingControlController::_threadGetSharpness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentSharpness = pAction->getArgument<ui2>("CurrentSharpness");
    _ansGetSharpness(InstanceID, CurrentSharpness);
}

void 
RenderingControlController::_threadSetSharpness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredSharpness = pAction->getArgument<ui2>("DesiredSharpness");
    _ansSetSharpness(InstanceID, DesiredSharpness);
}

void 
RenderingControlController::_threadGetRedVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentRedVideoGain = pAction->getArgument<ui2>("CurrentRedVideoGain");
    _ansGetRedVideoGain(InstanceID, CurrentRedVideoGain);
}

void 
RenderingControlController::_threadSetRedVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredRedVideoGain = pAction->getArgument<ui2>("DesiredRedVideoGain");
    _ansSetRedVideoGain(InstanceID, DesiredRedVideoGain);
}

void 
RenderingControlController::_threadGetGreenVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentGreenVideoGain = pAction->getArgument<ui2>("CurrentGreenVideoGain");
    _ansGetGreenVideoGain(InstanceID, CurrentGreenVideoGain);
}

void 
RenderingControlController::_threadSetGreenVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredGreenVideoGain = pAction->getArgument<ui2>("DesiredGreenVideoGain");
    _ansSetGreenVideoGain(InstanceID, DesiredGreenVideoGain);
}

void 
RenderingControlController::_threadGetBlueVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBlueVideoGain = pAction->getArgument<ui2>("CurrentBlueVideoGain");
    _ansGetBlueVideoGain(InstanceID, CurrentBlueVideoGain);
}

void 
RenderingControlController::_threadSetBlueVideoGain(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBlueVideoGain = pAction->getArgument<ui2>("DesiredBlueVideoGain");
    _ansSetBlueVideoGain(InstanceID, DesiredBlueVideoGain);
}

void 
RenderingControlController::_threadGetRedVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentRedVideoBlackLevel = pAction->getArgument<ui2>("CurrentRedVideoBlackLevel");
    _ansGetRedVideoBlackLevel(InstanceID, CurrentRedVideoBlackLevel);
}

void 
RenderingControlController::_threadSetRedVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredRedVideoBlackLevel = pAction->getArgument<ui2>("DesiredRedVideoBlackLevel");
    _ansSetRedVideoBlackLevel(InstanceID, DesiredRedVideoBlackLevel);
}

void 
RenderingControlController::_threadGetGreenVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentGreenVideoBlackLevel = pAction->getArgument<ui2>("CurrentGreenVideoBlackLevel");
    _ansGetGreenVideoBlackLevel(InstanceID, CurrentGreenVideoBlackLevel);
}

void 
RenderingControlController::_threadSetGreenVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredGreenVideoBlackLevel = pAction->getArgument<ui2>("DesiredGreenVideoBlackLevel");
    _ansSetGreenVideoBlackLevel(InstanceID, DesiredGreenVideoBlackLevel);
}

void 
RenderingControlController::_threadGetBlueVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentBlueVideoBlackLevel = pAction->getArgument<ui2>("CurrentBlueVideoBlackLevel");
    _ansGetBlueVideoBlackLevel(InstanceID, CurrentBlueVideoBlackLevel);
}

void 
RenderingControlController::_threadSetBlueVideoBlackLevel(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredBlueVideoBlackLevel = pAction->getArgument<ui2>("DesiredBlueVideoBlackLevel");
    _ansSetBlueVideoBlackLevel(InstanceID, DesiredBlueVideoBlackLevel);
}

void 
RenderingControlController::_threadGetColorTemperature (Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 CurrentColorTemperature = pAction->getArgument<ui2>("CurrentColorTemperature");
    _ansGetColorTemperature (InstanceID, CurrentColorTemperature);
}

void 
RenderingControlController::_threadSetColorTemperature(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    ui2 DesiredColorTemperature = pAction->getArgument<ui2>("DesiredColorTemperature");
    _ansSetColorTemperature(InstanceID, DesiredColorTemperature);
}

void 
RenderingControlController::_threadGetHorizontalKeystone(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 CurrentHorizontalKeystone = pAction->getArgument<i2>("CurrentHorizontalKeystone");
    _ansGetHorizontalKeystone(InstanceID, CurrentHorizontalKeystone);
}

void 
RenderingControlController::_threadSetHorizontalKeystone(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 DesiredHorizontalKeystone = pAction->getArgument<i2>("DesiredHorizontalKeystone");
    _ansSetHorizontalKeystone(InstanceID, DesiredHorizontalKeystone);
}

void 
RenderingControlController::_threadGetVerticalKeystone(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 CurrentVerticalKeystone = pAction->getArgument<i2>("CurrentVerticalKeystone");
    _ansGetVerticalKeystone(InstanceID, CurrentVerticalKeystone);
}

void 
RenderingControlController::_threadSetVerticalKeystone(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    i2 DesiredVerticalKeystone = pAction->getArgument<i2>("DesiredVerticalKeystone");
    _ansSetVerticalKeystone(InstanceID, DesiredVerticalKeystone);
}

void 
RenderingControlController::_threadGetMute(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool CurrentMute = pAction->getArgument<bool>("CurrentMute");
    _ansGetMute(InstanceID, Channel, CurrentMute);
}

void 
RenderingControlController::_threadSetMute(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool DesiredMute = pAction->getArgument<bool>("DesiredMute");
    _ansSetMute(InstanceID, Channel, DesiredMute);
}

void 
RenderingControlController::_threadGetVolume(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    ui2 CurrentVolume = pAction->getArgument<ui2>("CurrentVolume");
    _ansGetVolume(InstanceID, Channel, CurrentVolume);
}

void 
RenderingControlController::_threadSetVolume(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    ui2 DesiredVolume = pAction->getArgument<ui2>("DesiredVolume");
    _ansSetVolume(InstanceID, Channel, DesiredVolume);
}

void 
RenderingControlController::_threadGetVolumeDB(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 CurrentVolume = pAction->getArgument<i2>("CurrentVolume");
    _ansGetVolumeDB(InstanceID, Channel, CurrentVolume);
}

void 
RenderingControlController::_threadSetVolumeDB(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 DesiredVolume = pAction->getArgument<i2>("DesiredVolume");
    _ansSetVolumeDB(InstanceID, Channel, DesiredVolume);
}

void 
RenderingControlController::_threadGetVolumeDBRange(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    i2 MinValue = pAction->getArgument<i2>("MinValue");
    i2 MaxValue = pAction->getArgument<i2>("MaxValue");
    _ansGetVolumeDBRange(InstanceID, Channel, MinValue, MaxValue);
}

void 
RenderingControlController::_threadGetLoudness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool CurrentLoudness = pAction->getArgument<bool>("CurrentLoudness");
    _ansGetLoudness(InstanceID, Channel, CurrentLoudness);
}

void 
RenderingControlController::_threadSetLoudness(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 InstanceID = pAction->getArgument<ui4>("InstanceID");
    std::string Channel = pAction->getArgument<std::string>("Channel");
    bool DesiredLoudness = pAction->getArgument<bool>("DesiredLoudness");
    _ansSetLoudness(InstanceID, Channel, DesiredLoudness);
}



void
MediaRendererController::eventHandler(StateVar* pStateVar)
{
    if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        m_pAVTransportController->_changedLastChange(val);
    }
    else if (pStateVar->getName() == "SourceProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedSourceProtocolInfo(val);
    }
    else if (pStateVar->getName() == "SinkProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedSinkProtocolInfo(val);
    }
    else if (pStateVar->getName() == "CurrentConnectionIDs") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedCurrentConnectionIDs(val);
    }
    else if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        m_pRenderingControlController->_changedLastChange(val);
    }
}


MediaRendererController::MediaRendererController(Device* pDevice, RenderingControlController* pRenderingControlController, ConnectionManagerController* pConnectionManagerController, AVTransportController* pAVTransportController) :
ControllerImplAdapter(pDevice),
m_pDevice(pDevice),
m_pRenderingControlController(pRenderingControlController), 
m_pConnectionManagerController(pConnectionManagerController), 
m_pAVTransportController(pAVTransportController)
{
    m_pRenderingControlController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:RenderingControl:1");
    m_pConnectionManagerController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:ConnectionManager:1");
    m_pAVTransportController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:AVTransport:1");

    init();
}


void
ContentDirectoryController::GetSearchCapabilities(std::string& SearchCaps)
{
    Action* pAction = m_pService->getAction("GetSearchCapabilities")->clone();
    m_pService->sendAction(pAction);
    SearchCaps = pAction->getArgument<std::string>("SearchCaps");
}

void
ContentDirectoryController::GetSortCapabilities(std::string& SortCaps)
{
    Action* pAction = m_pService->getAction("GetSortCapabilities")->clone();
    m_pService->sendAction(pAction);
    SortCaps = pAction->getArgument<std::string>("SortCaps");
}

void
ContentDirectoryController::GetSystemUpdateID(ui4& Id)
{
    Action* pAction = m_pService->getAction("GetSystemUpdateID")->clone();
    m_pService->sendAction(pAction);
    Id = pAction->getArgument<ui4>("Id");
}

void
ContentDirectoryController::Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    Action* pAction = m_pService->getAction("Browse")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("BrowseFlag", BrowseFlag);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    m_pService->sendAction(pAction);
    Result = pAction->getArgument<std::string>("Result");
    NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    UpdateID = pAction->getArgument<ui4>("UpdateID");
}

void
ContentDirectoryController::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    Action* pAction = m_pService->getAction("Search")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("SearchCriteria", SearchCriteria);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    m_pService->sendAction(pAction);
    Result = pAction->getArgument<std::string>("Result");
    NumberReturned = pAction->getArgument<ui4>("NumberReturned");
    TotalMatches = pAction->getArgument<ui4>("TotalMatches");
    UpdateID = pAction->getArgument<ui4>("UpdateID");
}

void
ContentDirectoryController::CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result)
{
    Action* pAction = m_pService->getAction("CreateObject")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("Elements", Elements);
    m_pService->sendAction(pAction);
    ObjectID = pAction->getArgument<std::string>("ObjectID");
    Result = pAction->getArgument<std::string>("Result");
}

void
ContentDirectoryController::DestroyObject(const std::string& ObjectID)
{
    Action* pAction = m_pService->getAction("DestroyObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    m_pService->sendAction(pAction);
}

void
ContentDirectoryController::UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
    Action* pAction = m_pService->getAction("UpdateObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("CurrentTagValue", CurrentTagValue);
    pAction->setArgument<std::string>("NewTagValue", NewTagValue);
    m_pService->sendAction(pAction);
}

void
ContentDirectoryController::ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID)
{
    Action* pAction = m_pService->getAction("ImportResource")->clone();
    pAction->setArgument<uri>("SourceURI", SourceURI);
    pAction->setArgument<uri>("DestinationURI", DestinationURI);
    m_pService->sendAction(pAction);
    TransferID = pAction->getArgument<ui4>("TransferID");
}

void
ContentDirectoryController::GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal)
{
    Action* pAction = m_pService->getAction("GetTransferProgress")->clone();
    pAction->setArgument<ui4>("TransferID", TransferID);
    m_pService->sendAction(pAction);
    TransferStatus = pAction->getArgument<std::string>("TransferStatus");
    TransferLength = pAction->getArgument<std::string>("TransferLength");
    TransferTotal = pAction->getArgument<std::string>("TransferTotal");
}

void
ContentDirectoryController::DeleteResource(const uri& ResourceURI)
{
    Action* pAction = m_pService->getAction("DeleteResource")->clone();
    pAction->setArgument<uri>("ResourceURI", ResourceURI);
    m_pService->sendAction(pAction);
}

void
ContentDirectoryController::CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID)
{
    Action* pAction = m_pService->getAction("CreateReference")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    m_pService->sendAction(pAction);
    NewID = pAction->getArgument<std::string>("NewID");
}

std::string
ContentDirectoryController::_getTransferIDs()
{
    return m_pService->getStateVar<std::string>("TransferIDs");
}

ui4
ContentDirectoryController::_getSystemUpdateID()
{
    return m_pService->getStateVar<ui4>("SystemUpdateID");
}

std::string
ContentDirectoryController::_getContainerUpdateIDs()
{
    return m_pService->getStateVar<std::string>("ContainerUpdateIDs");
}


void 
ContentDirectoryController::_reqGetSearchCapabilities()
{
    Action* pAction = m_pService->getAction("GetSearchCapabilities")->clone();
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadGetSearchCapabilities, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqGetSortCapabilities()
{
    Action* pAction = m_pService->getAction("GetSortCapabilities")->clone();
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadGetSortCapabilities, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqGetSystemUpdateID()
{
    Action* pAction = m_pService->getAction("GetSystemUpdateID")->clone();
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadGetSystemUpdateID, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqBrowse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria)
{
    Action* pAction = m_pService->getAction("Browse")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("BrowseFlag", BrowseFlag);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadBrowse, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqSearch(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria)
{
    Action* pAction = m_pService->getAction("Search")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("SearchCriteria", SearchCriteria);
    pAction->setArgument<std::string>("Filter", Filter);
    pAction->setArgument<ui4>("StartingIndex", StartingIndex);
    pAction->setArgument<ui4>("RequestedCount", RequestedCount);
    pAction->setArgument<std::string>("SortCriteria", SortCriteria);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadSearch, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqCreateObject(const std::string& ContainerID, const std::string& Elements)
{
    Action* pAction = m_pService->getAction("CreateObject")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("Elements", Elements);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadCreateObject, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqDestroyObject(const std::string& ObjectID)
{
    Action* pAction = m_pService->getAction("DestroyObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadDestroyObject, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqUpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
    Action* pAction = m_pService->getAction("UpdateObject")->clone();
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    pAction->setArgument<std::string>("CurrentTagValue", CurrentTagValue);
    pAction->setArgument<std::string>("NewTagValue", NewTagValue);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadUpdateObject, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqImportResource(const uri& SourceURI, const uri& DestinationURI)
{
    Action* pAction = m_pService->getAction("ImportResource")->clone();
    pAction->setArgument<uri>("SourceURI", SourceURI);
    pAction->setArgument<uri>("DestinationURI", DestinationURI);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadImportResource, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqGetTransferProgress(const ui4& TransferID)
{
    Action* pAction = m_pService->getAction("GetTransferProgress")->clone();
    pAction->setArgument<ui4>("TransferID", TransferID);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadGetTransferProgress, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqDeleteResource(const uri& ResourceURI)
{
    Action* pAction = m_pService->getAction("DeleteResource")->clone();
    pAction->setArgument<uri>("ResourceURI", ResourceURI);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadDeleteResource, pAction);
    thread.start();
}

void 
ContentDirectoryController::_reqCreateReference(const std::string& ContainerID, const std::string& ObjectID)
{
    Action* pAction = m_pService->getAction("CreateReference")->clone();
    pAction->setArgument<std::string>("ContainerID", ContainerID);
    pAction->setArgument<std::string>("ObjectID", ObjectID);
    ActionThread<ContentDirectoryController> thread(this, &ContentDirectoryController::_threadCreateReference, pAction);
    thread.start();
}

void 
ContentDirectoryController::_threadGetSearchCapabilities(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string SearchCaps = pAction->getArgument<std::string>("SearchCaps");
    _ansGetSearchCapabilities(SearchCaps);
}

void 
ContentDirectoryController::_threadGetSortCapabilities(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string SortCaps = pAction->getArgument<std::string>("SortCaps");
    _ansGetSortCapabilities(SortCaps);
}

void 
ContentDirectoryController::_threadGetSystemUpdateID(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 Id = pAction->getArgument<ui4>("Id");
    _ansGetSystemUpdateID(Id);
}

void 
ContentDirectoryController::_threadBrowse(Action* pAction)
{
    m_pService->sendAction(pAction);
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
ContentDirectoryController::_threadSearch(Action* pAction)
{
    m_pService->sendAction(pAction);
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
ContentDirectoryController::_threadCreateObject(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
    std::string Elements = pAction->getArgument<std::string>("Elements");
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string Result = pAction->getArgument<std::string>("Result");
    _ansCreateObject(ContainerID, Elements, ObjectID, Result);
}

void 
ContentDirectoryController::_threadDestroyObject(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    _ansDestroyObject(ObjectID);
}

void 
ContentDirectoryController::_threadUpdateObject(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string CurrentTagValue = pAction->getArgument<std::string>("CurrentTagValue");
    std::string NewTagValue = pAction->getArgument<std::string>("NewTagValue");
    _ansUpdateObject(ObjectID, CurrentTagValue, NewTagValue);
}

void 
ContentDirectoryController::_threadImportResource(Action* pAction)
{
    m_pService->sendAction(pAction);
    uri SourceURI = pAction->getArgument<uri>("SourceURI");
    uri DestinationURI = pAction->getArgument<uri>("DestinationURI");
    ui4 TransferID = pAction->getArgument<ui4>("TransferID");
    _ansImportResource(SourceURI, DestinationURI, TransferID);
}

void 
ContentDirectoryController::_threadGetTransferProgress(Action* pAction)
{
    m_pService->sendAction(pAction);
    ui4 TransferID = pAction->getArgument<ui4>("TransferID");
    std::string TransferStatus = pAction->getArgument<std::string>("TransferStatus");
    std::string TransferLength = pAction->getArgument<std::string>("TransferLength");
    std::string TransferTotal = pAction->getArgument<std::string>("TransferTotal");
    _ansGetTransferProgress(TransferID, TransferStatus, TransferLength, TransferTotal);
}

void 
ContentDirectoryController::_threadDeleteResource(Action* pAction)
{
    m_pService->sendAction(pAction);
    uri ResourceURI = pAction->getArgument<uri>("ResourceURI");
    _ansDeleteResource(ResourceURI);
}

void 
ContentDirectoryController::_threadCreateReference(Action* pAction)
{
    m_pService->sendAction(pAction);
    std::string ContainerID = pAction->getArgument<std::string>("ContainerID");
    std::string ObjectID = pAction->getArgument<std::string>("ObjectID");
    std::string NewID = pAction->getArgument<std::string>("NewID");
    _ansCreateReference(ContainerID, ObjectID, NewID);
}



void
MediaServerController::eventHandler(StateVar* pStateVar)
{
    if (pStateVar->getName() == "LastChange") {
        std::string val;
        pStateVar->getValue(val);
        m_pAVTransportController->_changedLastChange(val);
    }
    else if (pStateVar->getName() == "SourceProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedSourceProtocolInfo(val);
    }
    else if (pStateVar->getName() == "SinkProtocolInfo") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedSinkProtocolInfo(val);
    }
    else if (pStateVar->getName() == "CurrentConnectionIDs") {
        std::string val;
        pStateVar->getValue(val);
        m_pConnectionManagerController->_changedCurrentConnectionIDs(val);
    }
    else if (pStateVar->getName() == "TransferIDs") {
        std::string val;
        pStateVar->getValue(val);
        m_pContentDirectoryController->_changedTransferIDs(val);
    }
    else if (pStateVar->getName() == "SystemUpdateID") {
        ui4 val;
        pStateVar->getValue(val);
        m_pContentDirectoryController->_changedSystemUpdateID(val);
    }
    else if (pStateVar->getName() == "ContainerUpdateIDs") {
        std::string val;
        pStateVar->getValue(val);
        m_pContentDirectoryController->_changedContainerUpdateIDs(val);
    }
}


MediaServerController::MediaServerController(Device* pDevice, ContentDirectoryController* pContentDirectoryController, ConnectionManagerController* pConnectionManagerController, AVTransportController* pAVTransportController) :
ControllerImplAdapter(pDevice),
m_pDevice(pDevice),
m_pContentDirectoryController(pContentDirectoryController), 
m_pConnectionManagerController(pConnectionManagerController), 
m_pAVTransportController(pAVTransportController)
{
    m_pContentDirectoryController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:ContentDirectory:1");
    m_pConnectionManagerController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:ConnectionManager:1");
    m_pAVTransportController->m_pService = m_pDevice->getService("urn:schemas-upnp-org:service:AVTransport:1");

    init();
}

