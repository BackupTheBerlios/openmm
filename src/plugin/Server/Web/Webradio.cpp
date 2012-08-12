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
#include <fstream>
#include <sstream>

#include <Poco/ClassLibrary.h>
#include <Poco/Net/HTTPIOStream.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/Document.h>


#include <Omm/UpnpAvObject.h>
#include <Poco/File.h>

#include "Webradio.h"


WebradioModel::WebradioModel()
{
}


void
WebradioModel::init()
{
    scanStationConfig(getBasePath());
}


std::string
WebradioModel::getModelClass()
{
    return "WebradioModel";
}


Omm::ui4
WebradioModel::getSystemUpdateId(bool checkMod)
{
    // FIXME: if cache is current, xml config file is not parsed
    // and _stationNames not initialized. When using no database
    // cache, we get no meta data out of the model.
    return Poco::File(getBasePath()).getLastModified().epochTime();
}


void
WebradioModel::scan()
{
    scanStationConfig(getBasePath());
    for (std::map<std::string, std::string>::iterator it = _stationNames.begin(); it != _stationNames.end(); ++it) {
        addPath(it->first);
    }
}


std::string
WebradioModel::getClass(const std::string& path)
{
    return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_BROADCAST);
}


std::string
WebradioModel::getTitle(const std::string& path)
{
    return _stationNames[path];
}


bool
WebradioModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
WebradioModel::getStream(const std::string& uri, const std::string& resourcePath)
{
    Poco::URI streamUri(uri);

    Poco::Net::HTTPClientSession* pSession = new Poco::Net::HTTPClientSession(streamUri.getHost(), streamUri.getPort());
    std::string path = streamUri.getPath();
    if (path == "") {
        path = "/";
    }
    Poco::Net::HTTPRequest request("GET", path);
    pSession->sendRequest(request);
    std::stringstream requestHeader;
    request.write(requestHeader);
    LOGNS(Omm::Av, upnpav, debug, "proxy request header:\n" + requestHeader.str());

    Poco::Net::HTTPResponse response;
    std::istream& istr = pSession->receiveResponse(response);
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND ||
        response.getStatus() == Poco::Net::HTTPResponse::HTTP_NO_CONTENT) {
        LOGNS(Omm::Av, upnpav, error, "web radio stream not available");
        return 0;
    }
    else if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_SEE_OTHER ||
             response.getStatus() == Poco::Net::HTTPResponse::HTTP_FOUND ||
             response.getStatus() == Poco::Net::HTTPResponse::HTTP_MOVED_PERMANENTLY ||
             response.getStatus() == Poco::Net::HTTPResponse::HTTP_TEMPORARY_REDIRECT) {
        LOGNS(Omm::Av, upnpav, information, "web radio uri redirected to: " + response.get("Location"));
        delete pSession;
        return getStream(response.get("Location"));
    }

    if (istr.peek() == EOF) {
        LOGNS(Omm::Av, upnpav, error, "web radio failed to read data from stream uri: " + uri);
        return 0;
    }
    else {
        LOGNS(Omm::Av, upnpav, debug, "web radio success reading data from stream uri: " + uri);
    }

    LOGNS(Omm::Av, upnpav, information, "HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
    std::stringstream responseHeader;
    response.write(responseHeader);
    LOGNS(Omm::Av, upnpav, debug, "proxy response header:\n" + responseHeader.str());

    if (response.getContentType() == "audio/mpeg" || response.getContentType() == "application/ogg" || response.getContentType() == "audio/aac") {
        LOGNS(Omm::Av, upnpav, debug, "web radio detected audio content, streaming directly ...");
        return new Poco::Net::HTTPResponseStream(istr, pSession);
    }
    else if (response.getContentType() == "audio/x-scpls" || response.getContentType() == "audio/x-mpegurl") {
        std::vector<std::string> uris;
        // look for streamable URIs in the downloaded playlist
        LOGNS(Omm::Av, upnpav, debug, "web radio detected playlist, analyzing ...");
        std::string line;
        while (getline(istr, line)) {
            LOGNS(Omm::Av, upnpav, debug, line);
            std::string::size_type uriPos = line.find("http://");
            if (uriPos != std::string::npos) {
                std::string uri = Poco::trimRight(line.substr(uriPos));
                LOGNS(Omm::Av, upnpav, debug, "web radio found stream uri: " + uri);
                uris.push_back(uri);
            }
        }
        // try to stream one of the URIs in the downloaded playlist
        for (std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
            delete pSession;
            std::istream* pStream = getStream(*it);
            if (pStream) {
                return pStream;
            }
        }
    }
    else {
        LOGNS(Omm::Av, upnpav, error, "web radio unknown stream type (no audio or playlist)");
    }
    return 0;
}


void
WebradioModel::freeStream(std::istream* pIstream)
{
    LOGNS(Omm::Av, upnpav, debug, "deleting webradio stream");
    delete pIstream;
}


std::string
WebradioModel::getMime(const std::string& path)
{
    return "audio/mpeg";
}


std::string
WebradioModel::getDlna(const std::string& path)
{
    return "DLNA.ORG_PN=MP3;DLNA.ORG_OP=01";
}


void
WebradioModel::scanStationConfig(const std::string& stationConfig)
{
    LOGNS(Omm::Av, upnpav, debug, "web radio, start scanning station config file: " + stationConfig + " ...");

    _stationNames.clear();

    Poco::XML::DOMParser parser;
#if (POCO_VERSION & 0xFFFFFFFF) < 0x01040000
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
#else
    parser.setFeature(Poco::XML::DOMParser::FEATURE_FILTER_WHITESPACE, true);
#endif
    Poco::XML::Document* pDoc;
    try {
        pDoc = parser.parse(stationConfig);
    }
    catch (Poco::Exception& e) {
        LOGNS(Omm::Av, upnpav, error, "webradio config not found and scanning of stations not yet supported, giving up.");
        return;
    }

    Poco::XML::Node* pStationList = pDoc->firstChild();
    if (pStationList->nodeName() != "stationlist") {
        LOGNS(Omm::Av, upnpav, error, "error reading webradio station list, wrong file format");
        return;
    }

    std::string currentStationName;
    if (pStationList->hasChildNodes()) {
        Poco::XML::Node* pStation = pStationList->firstChild();
        LOGNS(Omm::Av, upnpav, debug, "stationlist first child: " + pStation->nodeName());
        while (pStation) {
            if (pStation->nodeName() != "station") {
                LOGNS(Omm::Av, upnpav, error, "error reading webradio station list, no station found.");
                return;
            }
            Poco::XML::Node* pProp = pStation->firstChild();
            while (pProp) {
                if (pProp->nodeName() == "name") {
                    currentStationName = pProp->innerText();
                    LOGNS(Omm::Av, upnpav, debug, "added web radio station with name: " + pProp->innerText());
                }
                else if (pProp->nodeName() == "uri") {
                    _stationNames[pProp->innerText()] = currentStationName;
                    LOGNS(Omm::Av, upnpav, debug, "added web radio station with uri: " + pProp->innerText());
                }
                else {
                    LOGNS(Omm::Av, upnpav, warning, "webradio station entry in config file has unknown property");
                }
                pProp = pProp->nextSibling();
            }
            pStation = pStation->nextSibling();
        }
    }
    LOGNS(Omm::Av, upnpav, debug, "web radio, finished scanning station config file.");
}


#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::AbstractDataModel)
POCO_EXPORT_CLASS(WebradioModel)
POCO_END_MANIFEST
#endif
