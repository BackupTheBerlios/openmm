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
DevConnectionManagerServerImpl::ConnectionComplete(const i4& ConnectionID)
{
// begin of your own code

// end of your own code
}


void
DevConnectionManagerServerImpl::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
// begin of your own code

// end of your own code
}


void
DevConnectionManagerServerImpl::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
// begin of your own code

// end of your own code
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
    AbstractMediaObject* object;
    if (ObjectID == "0") {
        object = _pRoot;
    }
    else {
        // get object via object id and index
        object = _pRoot->getDescendant(ObjectID.substr(2));
    }

    MediaObjectWriter2 writer(object);
    if (BrowseFlag == "BrowseMetadata") {
        writer.write(Result);
        NumberReturned = 1;
        TotalMatches = 1;
    }
    else if (BrowseFlag == "BrowseDirectChildren") {
        // get child objects via row
        NumberReturned = writer.writeChildren(StartingIndex, RequestedCount, Result);
        TotalMatches = object->getTotalChildCount();
    }
    else {
        Log::instance()->upnpav().error("Error in Browse: unkown BrowseFlag");
    }
    UpdateID = 0;
}


void
DevContentDirectoryServerImpl::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
// begin of your own code

// end of your own code
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
// begin of your own code

// end of your own code
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
