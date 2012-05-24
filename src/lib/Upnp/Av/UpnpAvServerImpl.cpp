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
#include <Poco/URI.h>

#include "UpnpAvServerImpl.h"

namespace Omm {
namespace Av {

void
DevAVTransportServerImpl::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
DevAVTransportServerImpl::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Stop(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Play(const ui4& InstanceID, const std::string& Speed)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Pause(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Next(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevAVTransportServerImpl::Previous(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
DevConnectionManagerServerImpl::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
DevConnectionManagerServerImpl::GetProtocolInfo(std::string& Source, std::string& Sink)
{
// begin of your own code

// end of your own code
}


void
DevConnectionManagerServerImpl::PrepareForConnection(const std::string& RemoteProtocolInfo, const std::string& PeerConnectionManager, const i4& PeerConnectionID, const std::string& Direction, i4& ConnectionID, i4& AVTransportID, i4& RcsID)
{
    ConnectionManagerId peerManagerId;
    try {
        peerManagerId.parseManagerIdString(PeerConnectionManager);
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("could not parse PeerConnectionManager string: " + e.displayText());
        return;
    }
    Connection* pConnection = new Connection(peerManagerId.getUuid(), _pThisDevice->getUuid());

    ConnectionPeer& thisPeer = pConnection->getRenderer();
    ConnectionPeer& remotePeer = pConnection->getServer();

    thisPeer._connectionId = getConnectionCount();
    thisPeer._AVTId = -1;
    thisPeer._RCId = -1;
    ConnectionID = thisPeer._connectionId;
    AVTransportID = thisPeer._AVTId;
    RcsID = thisPeer._RCId;

    remotePeer._connectionId = PeerConnectionID;

    addConnection(pConnection, RemoteProtocolInfo);

    _connectionIds.append(Poco::NumberFormatter::format(ConnectionID));
    _setCurrentConnectionIDs(_connectionIds.toString());
}


void
DevConnectionManagerServerImpl::ConnectionComplete(const i4& ConnectionID)
{
    removeConnection(ConnectionID);
    _connectionIds.remove(Poco::NumberFormatter::format(ConnectionID));
}


void
DevConnectionManagerServerImpl::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
    ConnectionIDs = _connectionIds.toString();
}


void
DevConnectionManagerServerImpl::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
    Connection* pConnection = getConnection(ConnectionID);
    if (pConnection) {
        RcsID = pConnection->getServer()._RCId;
        AVTransportID = pConnection->getServer()._AVTId;
//        ProtocolInfo = pConnection->getServer()._protInfo;

        PeerConnectionManager = pConnection->getRenderer()._managerId.toString();
        PeerConnectionID = pConnection->getRenderer()._connectionId;
    }
}


void
DevContentDirectoryServerImpl::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
DevContentDirectoryServerImpl::GetSearchCapabilities(std::string& SearchCaps)
{
    if (!_pRoot->getSearchCaps()) {
        SearchCaps = "";
    }
}


void
DevContentDirectoryServerImpl::GetSortCapabilities(std::string& SortCaps)
{
    if (!_pRoot->getSortCaps()) {
        SortCaps = "";
    }
}


void
DevContentDirectoryServerImpl::GetSystemUpdateID(ui4& Id)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    ServerObject* pObject;
    if (ObjectID == "0") {
        pObject = _pRoot;
    }
    else {
        pObject = _pRoot->getDescendant(ObjectID);
    }

    ServerObjectWriter writer;
    if (BrowseFlag == "BrowseMetadata") {
        writer.write(Result, pObject);
        NumberReturned = 1;
        TotalMatches = 1;
    }
    else if (BrowseFlag == "BrowseDirectChildren") {
        // get child objects via row
        Log::instance()->upnpav().debug("browse direct children of: " + pObject->getTitle());
        std::vector<ServerObject*> children;
        TotalMatches = pObject->getChildrenAtRowOffset(children, StartingIndex, RequestedCount, SortCriteria);
        NumberReturned = children.size();
        writer.writeChildren(Result, children);
    }
    else {
        Log::instance()->upnpav().error("Error in Browse: unkown BrowseFlag");
    }
    UpdateID = 0;
}


void
DevContentDirectoryServerImpl::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    ServerObject* pObject;
    if (ContainerID == "0") {
        pObject = _pRoot;
    }
    else {
        pObject = _pRoot->getDescendant(ContainerID);
    }

    ServerObjectWriter writer;
    // get child objects via row
    std::vector<ServerObject*> children;
    TotalMatches = pObject->getChildrenAtRowOffset(children, StartingIndex, RequestedCount, SortCriteria, SearchCriteria);
    NumberReturned = children.size();
    writer.writeChildren(Result, children);
    UpdateID = 0;
}


void
DevContentDirectoryServerImpl::CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::DestroyObject(const std::string& ObjectID)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID)
{
    // get resource of object in DestinationURI
    Poco::StringTokenizer uri(DestinationURI.getPath(), "$");
    std::string objectId = uri[0].substr(1);
    Log::instance()->upnpav().debug("import resource to objectId: " + objectId + ", resourceId: " + uri[1]);
    ServerObject* pObject = _pRoot->getDescendant(objectId);
    if (!pObject) {
        Log::instance()->upnpav().error("import resource, could not find object with id: " + objectId);
        return;
    }
    int resourceId = Poco::NumberParser::parse(uri[1]);
    ServerObjectResource* pResource = static_cast<ServerObjectResource*>(pObject->getResource(resourceId));

    if (pResource) {
        pResource->writeResource(SourceURI);
    }
    else {
        Log::instance()->upnpav().error("import resource, no resource for object with id: " + objectId);
    }
}


void
DevContentDirectoryServerImpl::GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::DeleteResource(const uri& ResourceURI)
{
// begin of your own code

// end of your own code
}


void
DevContentDirectoryServerImpl::CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID)
{
// begin of your own code

// end of your own code
}


} // namespace Av
} // namespace Omm
