#include "UpnpAvServerImpl.h"

void
AVTransportImplementation::initStateVars()
{
// you may set state variables here with the _set<state_variable_name>() methods
// default values are already set, if defined by the service
}


void
AVTransportImplementation::SetAVTransportURI(const Jamm::ui4& InstanceID, const std::string& CurrentURI, const std::string& CurrentURIMetaData)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetMediaInfo(const Jamm::ui4& InstanceID, Jamm::ui4& NrTracks, std::string& MediaDuration, std::string& CurrentURI, std::string& CurrentURIMetaData, std::string& NextURI, std::string& NextURIMetaData, std::string& PlayMedium, std::string& RecordMedium, std::string& WriteStatus)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportInfo(const Jamm::ui4& InstanceID, std::string& CurrentTransportState, std::string& CurrentTransportStatus, std::string& CurrentSpeed)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetPositionInfo(const Jamm::ui4& InstanceID, Jamm::ui4& Track, std::string& TrackDuration, std::string& TrackMetaData, std::string& TrackURI, std::string& RelTime, std::string& AbsTime, Jamm::i4& RelCount, Jamm::i4& AbsCount)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetDeviceCapabilities(const Jamm::ui4& InstanceID, std::string& PlayMedia, std::string& RecMedia, std::string& RecQualityModes)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::GetTransportSettings(const Jamm::ui4& InstanceID, std::string& PlayMode, std::string& RecQualityMode)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Stop(const Jamm::ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Play(const Jamm::ui4& InstanceID, const std::string& Speed)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Pause(const Jamm::ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Seek(const Jamm::ui4& InstanceID, const std::string& Unit, const std::string& Target)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Next(const Jamm::ui4& InstanceID)
{
// begin of your own code

// end of your own code
}


void
AVTransportImplementation::Previous(const Jamm::ui4& InstanceID)
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
ConnectionManagerImplementation::ConnectionComplete(const Jamm::i4& ConnectionID)
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
ConnectionManagerImplementation::GetCurrentConnectionInfo(const Jamm::i4& ConnectionID, Jamm::i4& RcsID, Jamm::i4& AVTransportID, std::string& ProtocolInfo, std::string& PeerConnectionManager, Jamm::i4& PeerConnectionID, std::string& Direction, std::string& Status)
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
ContentDirectoryImplementation::GetSystemUpdateID(Jamm::ui4& Id)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::Browse(const std::string& ObjectID, const std::string& BrowseFlag, const std::string& Filter, const Jamm::ui4& StartingIndex, const Jamm::ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, Jamm::ui4& NumberReturned, Jamm::ui4& TotalMatches, Jamm::ui4& UpdateID)
{
    Jamm::Av::MediaObject* object;
    if (ObjectID == "0") {
        object = m_pRoot;
    }
    else {
        object = m_pRoot->getObject(ObjectID);
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
    
}


void
ContentDirectoryImplementation::Search(const std::string& ContainerID, const std::string& SearchCriteria, const std::string& Filter, const Jamm::ui4& StartingIndex, const Jamm::ui4& RequestedCount, const std::string& SortCriteria, std::string& Result, Jamm::ui4& NumberReturned, Jamm::ui4& TotalMatches, Jamm::ui4& UpdateID)
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
ContentDirectoryImplementation::ImportResource(const Jamm::uri& SourceURI, const Jamm::uri& DestinationURI, Jamm::ui4& TransferID)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::GetTransferProgress(const Jamm::ui4& TransferID, std::string& TransferStatus, std::string& TransferLength, std::string& TransferTotal)
{
// begin of your own code

// end of your own code
}


void
ContentDirectoryImplementation::DeleteResource(const Jamm::uri& ResourceURI)
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


