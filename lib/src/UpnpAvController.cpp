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

#include "UpnpAvController.h"
#include "UpnpAvControllerPrivate.h"
#include "UpnpAvCtrlImpl.h"

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
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
    Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(metaData);
    Poco::XML::Node* pDidl = pDoc->documentElement()->firstChild();
    readNode(pDidl);
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
    if (pNode->nodeName() == "container") {
        _isContainer = true;
        if (attr != 0) {
            _childCount = Poco::NumberParser::parse(attr->getNamedItem("childCount")->nodeValue());
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
            
            if (childNode->nodeName() == "res") {
                Poco::XML::NamedNodeMap* attr = 0;
                std::string protInfo = "";
                ui4 size = 0;
                if (childNode->hasAttributes()) {
                    attr = childNode->attributes();
                    Poco::XML::Node* attrNode = attr->getNamedItem("protocolInfo");
                    if (attrNode) {
                        protInfo = attrNode->nodeValue();
                    }
                    attrNode = attr->getNamedItem("size");
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
        _server->ContentDirectory()->Browse(_objectId, "BrowseDirectChildren", "*", _children.size(), 10, "", result, numberReturned, totalMatches, updateId);
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
ControllerObject::setServerController(MediaServerController* _pServer)
{
    _server = _pServer;
}


ServerController::ServerController(MediaServerController* pServerController) :
_pServerController(pServerController)
{
    _pRoot = new ControllerObject();
    _pRoot->setObjectId("0");
    _pRoot->setTitle(_pServerController->getDevice()->getFriendlyName());
    _pRoot->setServerController(_pServerController);
    _pRoot->setFetchedAllChildren(false);
    // TODO: browse root object "0" here
    // TODO: this should depend on the browse result for root object "0"
    _pRoot->setIsContainer(true);
}


RendererView::RendererView(MediaRendererController* rendererController) :
_pRendererController(rendererController)
{
}


const std::string&
RendererView::getName()
{
    return _pRendererController->getDevice()->getFriendlyName();
}


void
UpnpAvController::setUserInterface(UpnpAvUserInterface* pUserInterface)
{
    _pAvUserInterface = pUserInterface;
    _pAvUserInterface->_pRenderers = &_renderers;
    _pAvUserInterface->_pServers = &_servers;
}


void
UpnpAvController::deviceAdded(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information("device added, friendly name: " + pDevice->getFriendlyName() + " ,uuid: " + pDevice->getUuid());
//     std::clog << "UpnpAvController::deviceAdded()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        MediaRendererController* pRenderer = new MediaRendererController(
            pDevice,
            new RenderingControlControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl);
        _pAvUserInterface->beginAddRenderer(_renderers.size());
//         std::clog << "UpnpAvController::deviceAdded() number of renderers: " << _renderers.size() << std::endl;
        _renderers.append(pDevice->getUuid(), new RendererView(pRenderer));
//         std::clog << "UpnpAvController::deviceAdded() number of renderers: " << _renderers.size() << std::endl;
        _pAvUserInterface->endAddRenderer(_renderers.size() - 1);
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        ServerController* pServer = new ServerController(new Omm::Av::MediaServerController(
            pDevice,
            new ContentDirectoryControllerImpl,
            new ConnectionManagerControllerImpl,
            new AVTransportControllerImpl));
        _pAvUserInterface->beginAddServer(_servers.size());
//         std::clog << "UpnpAvController::deviceAdded() number of servers: " << _servers.size() << std::endl;
        _servers.append(pDevice->getUuid(), pServer);
//         std::clog << "UpnpAvController::deviceAdded() number of servers: " << _servers.size() << std::endl;
        _pAvUserInterface->endAddServer(_servers.size() - 1);
    }
}


void
UpnpAvController::deviceRemoved(DeviceRoot* pDeviceRoot)
{
    Device* pDevice = pDeviceRoot->getRootDevice();
    Log::instance()->upnpav().information("device removed, friendly name: " + pDevice->getFriendlyName() + " ,uuid: " + pDevice->getUuid());
//     std::clog << "UpnpAvController::deviceRemoved()" << std::endl;
//     std::clog << "uuid: " << pDevice->getUuid() << std::endl;
//     std::clog << "type: " << pDevice->getDeviceType() << std::endl;
//     std::clog << "friendly name: " << pDevice->getFriendlyName() << std::endl;
    
    if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaRenderer:1") {
        // TODO: delete renderer controller
        _pAvUserInterface->beginRemoveRenderer(_renderers.position(pDevice->getUuid()));
//         std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << _renderers.size() << std::endl;
        _renderers.remove(pDevice->getUuid());
//         std::clog << "UpnpAvController::deviceRemoved() number of renderers: " << _renderers.size() << std::endl;
        _pAvUserInterface->endRemoveRenderer(_renderers.position(pDevice->getUuid()));
    }
    else if (pDevice->getDeviceType() == "urn:schemas-upnp-org:device:MediaServer:1") {
        // TODO: delete server controller
        _pAvUserInterface->beginRemoveServer(_servers.position(pDevice->getUuid()));
//         std::clog << "UpnpAvController::deviceRemoved() number of servers: " << _servers.size() << std::endl;
        _servers.remove(pDevice->getUuid());
//         std::clog << "UpnpAvController::deviceRemoved() number of servers: " << _servers.size() << std::endl;
        _pAvUserInterface->endRemoveServer(_servers.position(pDevice->getUuid()));
    }
}


UpnpAvUserInterface::UpnpAvUserInterface() :
_pRenderers(NULL),
_pServers(NULL),
_pSelectedRenderer(NULL),
_pSelectedObject(NULL)
{
}


int
UpnpAvUserInterface::rendererCount()
{
    return _pRenderers->size();
}


RendererView*
UpnpAvUserInterface::rendererView(int numRenderer)
{
    return &_pRenderers->get(numRenderer);
}


int
UpnpAvUserInterface::serverCount()
{
    return _pServers->size();
}


ControllerObject*
UpnpAvUserInterface::serverRootObject(int numServer)
{
    return _pServers->get(numServer).root();
}


void
UpnpAvUserInterface::rendererSelected(RendererView* pRenderer)
{
    _pSelectedRenderer = pRenderer->_pRendererController;
    std::string sourceInfo;
    std::string sinkInfo;
    _pSelectedRenderer->ConnectionManager()->GetProtocolInfo(sourceInfo, sinkInfo);
}


void
UpnpAvUserInterface::mediaObjectSelected(ControllerObject* pObject)
{
    _pSelectedObject = pObject;
}


void
UpnpAvUserInterface::playPressed()
{
    if (_pSelectedRenderer == 0 || _pSelectedObject == 0) {
        return;
    }
//     std::clog << "UpnpAvUserInterface::playPressed() _pSelectedObject: " << _pSelectedObject << std::endl;
    Resource* pRes = _pSelectedObject->getResource();
    if (pRes) {
        std::string metaData;
        Omm::Av::MediaObjectWriter writer(_pSelectedObject);
        writer.write(metaData);
//         _pSelectedObject->writeMetaData(metaData);
        _pSelectedRenderer->AVTransport()->SetAVTransportURI(0, pRes->getUri(), metaData);
        _pSelectedRenderer->AVTransport()->Play(0, "1");
    }
}


void
UpnpAvUserInterface::stopPressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->AVTransport()->Stop(0);
}


void
UpnpAvUserInterface::pausePressed()
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->AVTransport()->Pause(0);
}


void
UpnpAvUserInterface::positionMoved(int position)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
        // TODO: need to support UPnP time format in Variant
//     _pSelectedRenderer->AVTransport()->Seek(0, "ABS_TIME", );
}


void
UpnpAvUserInterface::volumeChanged(int value)
{
    if (_pSelectedRenderer == 0) {
        return;
    }
    _pSelectedRenderer->RenderingControl()->SetVolume(0, "Master", value);
}


// 
// void
// UpnpController::sliderMoved(int position)
// {
//     std::clog << "UpnpController::sliderMoved() to:" << position;
//     if (_curMediaRenderer.IsNull()) {
//         return;
//     }
//     
//     NPT_String timestamp;
//     PLT_Didl::FormatTimeStamp(timestamp, position);
//     _mediaController->Seek(_curMediaRenderer, 0, "ABS_TIME", timestamp, NULL);
// }
// 
// 
// 
// 
// void
// UpnpController::OnGetPositionInfoResult(
//                                       NPT_Result               /* res */,
//                                       PLT_DeviceDataReference& /*device*/,
//                                       PLT_PositionInfo*        info,
//                                       void*                    /* userdata */)
// {
//     std::clog << "UpnpController::OnGetPositionInfoResult() duration:" << info->track_duration << "position:" << info->abs_time;
//     emit setSlider(info->track_duration, info->abs_time);
// }


// void
// UpnpController::onSignalReceived()
// {
// //     std::clog << "UpnpController::pollPositionInfo()";
//     if (!_curMediaRenderer.IsNull()) {
//         _mediaController->GetPositionInfo(_curMediaRenderer, 0, NULL);
//     }
// }

} // namespace Av
} // namespace Omm

