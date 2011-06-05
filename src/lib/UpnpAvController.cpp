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

#include "UpnpAvServer.h"
#include "UpnpAvRenderer.h"
#include "UpnpAvTypes.h"
#include "UpnpAvCtrlImpl.h"
#include "UpnpAvControllers.h"

#include "UpnpAvController.h"
#include "UpnpAvControllerPrivate.h"


namespace Omm {
namespace Av {

ControllerObject::ControllerObject() :
Omm::Av::MediaObject(),
_childCount(0),
_fetchedAllChildren(false)
{
}


void
ControllerObject::addResource(Resource* pResource)
{
//     std::clog << "ControllerObject::addResource() with uri: " << pResource->getUri() << std::endl;
    _resources.push_back(pResource);
}


void
ControllerObject::readChildren(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    while (pObjectNode)
    {
        if (pObjectNode->hasChildNodes()) {
            ControllerObject* pObject = new ControllerObject;
            pObject->readNode(pObjectNode);
            pObject->_parent = this;
            pObject->_server = _server;
            _children.push_back(pObject);
        }
        pObjectNode = pObjectNode->nextSibling();
    }
}


void
ControllerObject::readMetaData(const std::string& metaData)
{
    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pObjectNode = pDoc->documentElement()->firstChild();
    readNode(pObjectNode);
}


void
ControllerObject::readNode(Poco::XML::Node* pNode)
{
    Poco::XML::NamedNodeMap* attr = 0;
    if (pNode->hasAttributes()) {
        attr = pNode->attributes();
        _objectId = attr->getNamedItem("id")->nodeValue();
//         _parentId = attr->getNamedItem("parentID")->nodeValue();
        
    }
    if (pNode->nodeName() == AvClass::CONTAINER) {
        _isContainer = true;
        if (attr != 0) {
            _childCount = Poco::NumberParser::parse(attr->getNamedItem(AvProperty::CHILD_COUNT)->nodeValue());
        }
    }
    if (attr != 0) {
        attr->release();
    }
    
//     std::clog << "isContainer: " << (_isContainer ? "1" : "0") << std::endl;
//     std::clog << "id: " << _objectId << std::endl;
// //     std::clog << "parentId: " << _parentId << std::endl;
//     std::clog << "childCount: " << _childCount << std::endl;
    
    if (pNode->hasChildNodes()) {
        Poco::XML::Node* childNode = pNode->firstChild();
        while (childNode)
        {
//             std::clog << childNode->nodeName() << ": " << childNode->innerText() << std::endl;
            
            if (childNode->nodeName() == AvProperty::RES) {
                Poco::XML::NamedNodeMap* attr = 0;
                std::string protInfo = "";
                ui4 size = 0;
                if (childNode->hasAttributes()) {
                    attr = childNode->attributes();
                    Poco::XML::Node* attrNode = attr->getNamedItem(AvProperty::PROTOCOL_INFO);
                    if (attrNode) {
                        protInfo = attrNode->nodeValue();
                    }
                    attrNode = attr->getNamedItem(AvProperty::SIZE);
                    if (attrNode) {
                        size = Poco::NumberParser::parseUnsigned(attrNode->nodeValue());
                    }
                }
                addResource(new Resource(childNode->innerText(), protInfo, size));
//                 addResource(childNode->innerText(), protInfo, size);
                if (attr != 0) {
                    attr->release();
                }
            }
            else {
//                 _properties.append(childNode->nodeName(), new Variant(childNode->innerText()));
                _properties[childNode->nodeName()] =  childNode->innerText();
            }
            childNode = childNode->nextSibling();
        }
    }
}


int
ControllerObject::fetchChildren()
{
    // TODO: browse meta data for the root object with id "0"
//     std::clog << "MediaObject::fetchChildren() objectId: " << _objectId << std::endl;
    if (_server && !_fetchedAllChildren) {
        std::string result;
        Omm::ui4 numberReturned;
        Omm::ui4 totalMatches;
        Omm::ui4 updateId;
        try {
            _server->ContentDirectory()->Browse(_objectId, "BrowseDirectChildren", "*", _children.size(), 10, "", result, numberReturned, totalMatches, updateId);
        }
        catch (Poco::Exception& e){
//             error("");
// FIXME: if no children are fetched (network error) and _fetchedAllChildren remains false, controller doesn't stop fetching children
//             _fetchedAllChildren = true;
            Log::instance()->upnpav().error("could not fetch children: " + e.displayText());
            return 0;
        }
        readChildren(result);
//         UpnpBrowseResult res = _server->browseChildren(this, _children.size(), UpnpServer::_sliceSize);
        // _totalMatches is the number of items in the browse result, that matches
        // the filter criterion (see examples, 2.8.2, 2.8.3 in AV-CD 1.0)
        if (_children.size() >= totalMatches) {
            _fetchedAllChildren = true;
        }
        _childCount = totalMatches;
    }
    return _childCount;
}


bool
ControllerObject::fetchedAllChildren()
{
    return _fetchedAllChildren;
}


ControllerObject*
ControllerObject::parent()
{
    return static_cast<ControllerObject*>(_parent);
}


std::string
ControllerObject::getProperty(const std::string& name)
{
//     std::clog << "MediaObject::getProperty() name: " << name << std::endl;
//     std::clog << "MediaObject::getProperty() number of properties: " << _properties.size() << std::endl;
//     std::clog << "MediaObject::getProperty() value: " << _properties[name] << std::endl;
    
//     return _properties.getValue<std::string>(name);
    return _properties[name];
}


Icon*
ControllerObject::getIcon()
{
    // icon property is a lower resolution thumb nail of the original picture (but should be displayable on a handheld device).
    return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getProperty(AvProperty::ICON));
}


Icon*
ControllerObject::getImageRepresentation()
{
    std::string prop = getProperty(AvProperty::CLASS);
    if (AvClass::matchClass(prop, AvClass::ITEM, AvClass::IMAGE_ITEM))
    {
        return new Icon(0, 0, 0, Mime::IMAGE_JPEG, getResource()->getUri());
    }
    else {
        // for any other object type, we currently don't supply any icon.
        return 0;
    }
}


Resource*
ControllerObject::getResource(int num)
{
//     std::clog << "ControllerObject::getResource() number: " << num << std::endl;
//     std::clog << "ControllerObject::getResource() with uri: " << _resources[num]->getUri() << std::endl;
    return _resources[num];
}


ui4
ControllerObject::childCount()
{
    return _childCount;
}


void
ControllerObject::setFetchedAllChildren(bool fetchedAllChildren)
{
    _fetchedAllChildren = fetchedAllChildren;
}


void
ControllerObject::setServerController(CtlMediaServer* _pServer)
{
    _server = _pServer;
}


ServerController::ServerController(CtlMediaServer* pServerController) :
_pServerController(pServerController)
{
}


void
ServerController::browseRootObject()
{
  _pRoot = new ControllerObject;
    try {
        std::string rootMeta;
        ui4 numberReturned;
        ui4 totalMatches;
        ui4 updateId;
        _pServerController->ContentDirectory()->Browse("0", "BrowseMetadata", "*", 0, 0, "", rootMeta, numberReturned, totalMatches, updateId);
        _pRoot->readMetaData(rootMeta);
        Log::instance()->upnpav().debug("controller fetched root object with title: " + _pRoot->getTitle() + ", class: " + _pRoot->getProperty(AvProperty::CLASS));
    }
    catch (Poco::Exception& e) {
        Log::instance()->upnpav().error("controller could not fetch root object, setting default replacement object: " + e.displayText());
        _pRoot->setObjectId("0");
        _pRoot->setIsContainer(true);
    }
    _pRoot->setTitle(_pServerController->getDevice()->getFriendlyName());
    _pRoot->setServerController(_pServerController);
    _pRoot->setFetchedAllChildren(false);
}


RendererView::RendererView(CtlMediaRenderer* rendererController) :
_pRendererController(rendererController)
{
}


const std::string
RendererView::getName()
{
    return _pRendererController->getDevice()->getFriendlyName();
}


const std::string
RendererView::getUuid()
{
    return _pRendererController->getDevice()->getUuid();
}


void
AvController::setUserInterface(AvUserInterface* pUserInterface)
{
    Controller::setUserInterface(pUserInterface);
    pUserInterface->_pAvController = this;
}


int
AvController::rendererCount()
{
    return _renderers.size();
}


RendererView*
AvController::rendererView(int numRenderer)
{
    return &_renderers.get(numRenderer);
}


int
AvController::serverCount()
{
    return _servers.size();
}


ControllerObject*
AvController::serverRootObject(int numServer)
{
    return _servers.get(numServer).root();
}


Device*
AvController::serverDevice(int numServer)
{
    return _servers.get(numServer).controller()->getDevice();
}


void
AvController::addDeviceContainer(DeviceContainer* pDeviceContainer)
{
    // FIXME: need a list of ControllerImplAdapters for calling eventHandler(StateVar*)
    // FIXME: ownership of ControllerImplAdapters ...
    // FIXME: add all devices, not only root device

    Controller::addDeviceContainer(pDeviceContainer);

    AvUserInterface* pUserInterface = static_cast<AvUserInterface*>(_pUserInterface);
    Device* pDevice = pDeviceContainer->getRootDevice();
    Log::instance()->upnpav().information("AV controller add root device, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        CtlMediaRenderer* pRenderer = new CtlMediaRenderer(
            pDevice,
            new CtlRenderingControlImpl,
            new CtlConnectionManagerImpl,
            new CtlAVTransportImpl);
        pUserInterface->beginAddRenderer(_renderers.size());
        _renderers.append(pDevice->getUuid(), new RendererView(pRenderer));
        pUserInterface->endAddRenderer(_renderers.size() - 1);
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        ServerController* pServer = new ServerController(new Omm::Av::CtlMediaServer(
            pDevice,
            new CtlContentDirectoryImpl,
            new CtlConnectionManagerImpl,
            new CtlAVTransportImpl));
        pServer->browseRootObject();
        pUserInterface->beginAddServer(_servers.size());
        _servers.append(pDevice->getUuid(), pServer);
        pUserInterface->endAddServer(_servers.size() - 1);
    }
    Log::instance()->upnpav().information("AV controller add root device finished, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
}


void
AvController::removeDeviceContainer(DeviceContainer* pDeviceContainer)
{
    AvUserInterface* pUserInterface = static_cast<AvUserInterface*>(_pUserInterface);
    Device* pDevice = pDeviceContainer->getRootDevice();
    Log::instance()->upnpav().information("av controller removed root device, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        // TODO: delete renderer controller
        int position = _renderers.position(pDevice->getUuid());
        pUserInterface->beginRemoveRenderer(position);
        _renderers.remove(pDevice->getUuid());
        pUserInterface->endRemoveRenderer(position);
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        // TODO: delete server controller
        int position = _servers.position(pDevice->getUuid());
        pUserInterface->beginRemoveServer(position);
        _servers.remove(pDevice->getUuid());
        pUserInterface->endRemoveServer(position);
    }

    Controller::removeDeviceContainer(pDeviceContainer);

    Log::instance()->upnpav().information("av controller removed root device finished, friendly name: " + pDevice->getFriendlyName() + ", uuid: " + pDevice->getUuid());
}


AvUserInterface::AvUserInterface() :
_pAvController(0),
_pSelectedRenderer(0),
_pSelectedObject(0),
_pEngine(0)
{
    _positionInfoTimer.setPeriodicInterval(1000);
    Poco::TimerCallback<AvUserInterface> callback(*this, &AvUserInterface::pollPositionInfo);
    // NOTE: position info polling disabled for now. Reason: engine throws exceptions
    // when stream is not seekable. We should check if stream is seekable somewhere
    // and avoid reading the position (in Engine?)
//    _positionInfoTimer.start(callback);
}


AvUserInterface::~AvUserInterface()
{
    _positionInfoTimer.stop();
}


int
AvUserInterface::rendererCount()
{
    return _pAvController->rendererCount();
}


RendererView*
AvUserInterface::rendererView(int numRenderer)
{
    return _pAvController->rendererView(numRenderer);
}


bool
AvUserInterface::isPlaying(RendererView* pRenderer)
{
    std::string transportState;
    std::string transportStatus;
    std::string speed;
    pRenderer->_pRendererController->AVTransport()->GetTransportInfo(0, transportState, transportStatus, speed);
    if (transportState == AvTransportArgument::TRANSPORT_STATE_PLAYING) {
        return true;
    }
    else {
        return false;
    }
}


int
AvUserInterface::serverCount()
{
    return _pAvController->serverCount();
}


ControllerObject*
AvUserInterface::serverRootObject(int numServer)
{
    return _pAvController->serverRootObject(numServer);
}


const std::string
AvUserInterface::serverUuid(int numServer)
{
    return _pAvController->serverDevice(numServer)->getUuid();
}


void
AvUserInterface::rendererSelected(RendererView* pRenderer)
{
    _pSelectedRenderer = pRenderer->_pRendererController;
    std::string sourceInfo;
    std::string sinkInfo;
    _pSelectedRenderer->ConnectionManager()->GetProtocolInfo(sourceInfo, sinkInfo);
}


void
AvUserInterface::mediaObjectSelected(ControllerObject* pObject)
{
    _pSelectedObject = pObject;
}


void
AvUserInterface::playPressed()
{
    if (_pSelectedRenderer == 0 || _pSelectedObject == 0) {
        return;
    }
    Resource* pRes = _pSelectedObject->getResource();
    if (pRes) {
        std::string metaData;
        MediaObjectWriter writer(_pSelectedObject);
        writer.write(metaData);
//         _pSelectedObject->writeMetaData(metaData);
        try {
            _pSelectedRenderer->AVTransport()->SetAVTransportURI(0, pRes->getUri(), metaData);
            _pSelectedRenderer->AVTransport()->Play(0, "1");
        }
        catch (Poco::Exception e) {
            error(e.message());
            return;
        }
        Log::instance()->upnpav().debug("playing " + _pSelectedObject->getTitle());
    }
}


void
AvUserInterface::stopPressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Stop(0);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
    newPosition(0, 0);
}


void
AvUserInterface::pausePressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Pause(0);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
}


void
AvUserInterface::positionMoved(int position)
{
    Log::instance()->upnpav().debug("position moved to: " + Poco::NumberFormatter::format(position));
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->AVTransport()->Seek(0, AvTransportArgument::SEEK_MODE_ABS_TIME, AvTypeConverter::writeTime(position * 1000000));
    }
    catch (Poco::Exception& e){
        error(e.message());
    }
}


void
AvUserInterface::volumeChanged(int value)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    try {
        _pSelectedRenderer->RenderingControl()->SetVolume(0, "Master", value);
    }
    catch (Poco::Exception e){
        error(e.message());
    }
}


void
AvUserInterface::startLocalServers()
{
    for (std::vector<AvServer*>::iterator it = _localServers.begin(); it != _localServers.end(); ++it) {
        // TODO: start DeviceContainer instead of single devices
//        (*it)->startThreaded();
    }
}


void
AvUserInterface::stopLocalServers()
{
    for (std::vector<AvServer*>::iterator it = _localServers.begin(); it != _localServers.end(); ++it) {
        // TODO: start DeviceContainer instead of single devices
//        (*it)->stopThreaded();
    }
}


void
AvUserInterface::startLocalRenderer()
{
    if (_pEngine) {
        _pEngine->createPlayer();
        _pRenderer = new Omm::Av::AvRenderer(_pEngine);
//        _pRenderer->setFriendlyName("Local Player");
        //_pEngine->setParentView(parentView);
        // TODO: start device container of renderer
//        _pRenderer->start();
    }
    else {
        Log::instance()->upnpav().warning("no engine set in user interface, local renderer not started.");
    }
}


void
AvUserInterface::stopLocalRenderer()
{
    // TODO: stop device container of renderer
//    _pRenderer->stop();
}


void
AvUserInterface::setLocalEngine(Engine* pEngine)
{
    _pEngine = pEngine;
}


void
AvUserInterface::addLocalServer(AvServer* pServer)
{
    _localServers.push_back(pServer);
}


//void
//AvUserInterface::startFileServers()
//{
//    ThreadedServer* pMusic = new ThreadedServer("mp3", "Music (on iPhone)");
//    _FileServers.push_back(pMusic);
//    ThreadedServer* pNeuseeland = new ThreadedServer("Neuseeland", "Neuseeland (on iPhone)");
//    _FileServers.push_back(pNeuseeland);
//    pMusic->start();
//    pNeuseeland->start();
//}


//void
//AvUserInterface::startWebradio()
//{
//    std::string home = Poco::Environment::get("HOME");
//
//    // TODO: load webradio server plugin?
//    //_pWebradioContainer = new WebradioServer;
//    std::string webradioName = "Web Radio";
//    std::string webradioConf = home + "/.omm/webradio.conf";
////    std::string webradioConf = "/var/root/.omm/webradio.conf";
////    std::string webradioConf = "/Users/jb/.omm/webradio.conf";
//    _pWebradioContainer->setOption("basePath", webradioConf);
//    _pWebradioContainer->setTitle(webradioName);
//    _pWebradioServer = new Omm::Av::AvServer;
//    _pWebradioServer->setRoot(_pWebradioContainer);
//    _pWebradioServer->setFriendlyName(webradioName);
//    _pWebradioIcon = new Omm::Icon(22, 22, 8, "image/png", "device.png");
//    _pWebradioServer->addIcon(_pWebradioIcon);
//    _pWebradioServer->start();
//}


void
AvUserInterface::pollPositionInfo(Poco::Timer& timer)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
//    Log::instance()->upnpav().debug("poll position info ...");
    // TODO: get TransportState and poll position info only, if it is PLAYING, RECORDING or TRANSITIONING
//     _pSelectedRenderer->AVTransport()->
    ui4 Track;
    std::string TrackDuration;
    std::string TrackMetaData;
    std::string TrackURI;
    std::string RelTime;
    std::string AbsTime;
    i4 RelCount;
    i4 AbsCount;
    _pSelectedRenderer->AVTransport()->GetPositionInfo(0, Track, TrackDuration, TrackMetaData, TrackURI, RelTime, AbsTime, RelCount, AbsCount);
//    Log::instance()->upnpav().debug("TrackDuration: " + TrackDuration + ", TrackMetaData: " + TrackMetaData + ", TrackURI: " + TrackURI + ", RelTime: " + RelTime + ", AbsTime: " + AbsTime + ", RelCount: " + Poco::NumberFormatter::format(RelCount) + ", AbsCount: " + Poco::NumberFormatter::format(AbsCount));
    
    try {
        r8 trackDuration = AvTypeConverter::readDuration(TrackDuration);
        r8 absTime = AvTypeConverter::readDuration(AbsTime);
        newPosition(trackDuration, absTime);
    }
    catch (Poco::Exception& e) {
        //Log::instance()->upnpav().warning("could not read current track position: " + e.displayText());
    }
    
    if (TrackMetaData == "") {
        newTrack("", "", "");
    }
    else {
        ControllerObject object;
        try {
            object.readMetaData(TrackMetaData);
//            Log::instance()->upnpav().debug("new track title: " + object.getTitle());
//            Log::instance()->upnpav().debug("new track artist: " + object.getProperty(AvProperty::ARTIST));
//            Log::instance()->upnpav().debug("new track album: " + object.getProperty(AvProperty::ALBUM));
            newTrack(object.getTitle(), object.getProperty(AvProperty::ARTIST), object.getProperty(AvProperty::ALBUM));
        }
        catch (Poco::Exception& e) {
            newTrack("", "", "");
            Log::instance()->upnpav().error("could not read current track meta data: " + e.displayText());
        }
    }
    
    // FIXME: don't poll volume, use eventing through LastChange state variable.
    ui2 vol;
    _pSelectedRenderer->RenderingControl()->GetVolume(0, "Master", vol);
//    Log::instance()->upnpav().debug("volume of renderer is: " + Poco::NumberFormatter::format(vol));
    newVolume(vol);
}

} // namespace Av
} // namespace Omm

