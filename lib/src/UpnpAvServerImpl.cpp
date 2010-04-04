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

using namespace Omm;
using namespace Omm::Av;

void
AVTransportImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
AVTransportImplementation::SetAVTransportURI(const ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetMediaInfo(const ui4& InstanceID, ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportInfo(const ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetPositionInfo(const ui4& InstanceID, ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, i4& RelCount, i4& AbsCount)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetDeviceCapabilities(const ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportSettings(const ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Stop(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Play(const ui4& InstanceID, const std::string& Speed)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Pause(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Seek(const ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Next(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Previous(const ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
ConnectionManagerImplementation::GetProtocolInfo(std::string& Source, std::string& Sink)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::ConnectionComplete(const i4& ConnectionID)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::GetCurrentConnectionIDs(std::string& ConnectionIDs)
{
// begin of your own code

// end of your own code
}


void
ConnectionManagerImplementation::GetCurrentConnectionInfo(const i4& ConnectionID, i4& RcsID, i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, i4& PeerConnectionID, std::string& Direction, std::string& Status)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
ContentDirectoryImplementation::GetSearchCapabilities(std::string& SearchCaps)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::GetSortCapabilities(std::string& SortCaps)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::GetSystemUpdateID(ui4& Id)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
    MediaObject* object;
    if (ObjectID == "0") {
        object = _pRoot;
    }
    else {
        object = _pRoot->getObject(ObjectID.substr(2));
    }
    
    if (BrowseFlag == "BrowseMetadata") {
        object->writeMetaData(Result);
    }
    else if (BrowseFlag == "BrowseDirectChildren") {
        NumberReturned = object->writeChildren(StartingIndex, RequestedCount, Result);
        TotalMatches = object->getChildCount();
    }
    else {
        std::cerr << "Error in Browse: unkown BrowseFlag" << std::endl;
    }
    UpdateID = 0;
}


void
ContentDirectoryImplementation::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const ui4& StartingIndex, const ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, ui4& NumberReturned, ui4& TotalMatches, ui4& UpdateID)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::CreateObject(const std::string& ContainerID, const std::string& Elements, std::string& ObjectID, std::string& Result)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::DestroyObject(const std::string& ObjectID)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::UpdateObject(const std::string& ObjectID, const std::string& CurrentTagValue, const std::string& NewTagValue)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::ImportResource(const uri& SourceURI, const uri& DestinationURI, ui4& TransferID)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::GetTransferProgress(const ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::DeleteResource(const uri& ResourceURI)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::CreateReference(const std::string& ContainerID, const std::string& ObjectID, std::string& NewID)
{
// begin of your own code

// end of your own code
}


