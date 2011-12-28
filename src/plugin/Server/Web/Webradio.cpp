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

#include "Webradio.h"


class WebradioDataModel : public Omm::Av::SimpleDataModel
{
public:
    WebradioDataModel(const std::string& stationConfig);

    virtual std::string getClass(const std::string& path);
    virtual std::string getTitle(const std::string& path);

    virtual std::string getMime(const std::string& path);
    virtual std::string getDlna(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");

private:
    void scanStationConfig(const std::string& stationConfig);

    std::map<std::string, std::string>     _stationNames;
};


WebradioDataModel::WebradioDataModel(const std::string& stationConfig)
{
    scanStationConfig(stationConfig);
}


std::string
WebradioDataModel::getClass(const std::string& path)
{
    return Omm::Av::AvClass::className(Omm::Av::AvClass::ITEM, Omm::Av::AvClass::AUDIO_BROADCAST);
}


std::string
WebradioDataModel::getTitle(const std::string& path)
{
    return _stationNames[path];
}


bool
WebradioDataModel::isSeekable(const std::string& path, const std::string& resourcePath)
{
    return false;
}


std::istream*
WebradioDataModel::getStream(const std::string& path, const std::string& resourcePath)
{
    Poco::URI uri(path);

    Poco::Net::HTTPClientSession* pSession = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
    Poco::Net::HTTPRequest proxyRequest("GET", uri.getPath());
    pSession->sendRequest(proxyRequest);
    std::stringstream requestHeader;
    proxyRequest.write(requestHeader);
    Omm::Av::Log::instance()->upnpav().debug("proxy request header:\n" + requestHeader.str());

    Poco::Net::HTTPResponse proxyResponse;
    std::istream& istr = pSession->receiveResponse(proxyResponse);
    if (proxyResponse.getStatus() == Poco::Net::HTTPResponse::HTTP_NOT_FOUND) {
        Omm::Av::Log::instance()->upnpav().error("error web radio resource not available");
        return 0;
    }

    if (istr.peek() == EOF) {
        Omm::Av::Log::instance()->upnpav().error("error web radio reading data from web resource");
        return 0;
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("web radio success reading data from web resource");
    }

    Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(proxyResponse.getStatus()) + " " + proxyResponse.getReason());
    std::stringstream responseHeader;
    proxyResponse.write(responseHeader);
    Omm::Av::Log::instance()->upnpav().debug("proxy response header:\n" + responseHeader.str());

    std::streamsize bytes;
    if (proxyResponse.getContentType() == "audio/mpeg" || proxyResponse.getContentType() == "application/ogg") {
        Omm::Av::Log::instance()->upnpav().debug("web radio detected audio content, streaming directly ...");
        return new Poco::Net::HTTPResponseStream(istr, pSession);
    }
    else {
        std::vector<std::string> uris;

        // look for streamable URIs in the downloaded playlist
        Omm::Av::Log::instance()->upnpav().debug("web radio detected playlist, analyzing ...");
        std::string line;
        while (getline(istr, line)) {
            Omm::Av::Log::instance()->upnpav().debug(line);
            std::string::size_type uriPos = line.find("http://");
            if (uriPos != std::string::npos) {
                std::string uri = Poco::trimRight(line.substr(uriPos));
                Omm::Av::Log::instance()->upnpav().debug("web radio found stream uri: " + uri);
                uris.push_back(uri);
            }
        }
        // try to stream one of the URIs in the downloaded playlist
        for (std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
            Poco::URI streamUri(*it);

            Poco::Net::HTTPClientSession session(streamUri.getHost(), streamUri.getPort());
            std::string path = streamUri.getPath();
            if (path == "") {
                path = "/";
            }
            Poco::Net::HTTPRequest request("GET", path);
            pSession->sendRequest(request);
            std::stringstream requestHeader;
            request.write(requestHeader);
            Omm::Av::Log::instance()->upnpav().debug("proxy request header:\n" + requestHeader.str());

            Poco::Net::HTTPResponse response;
            std::istream& istr = pSession->receiveResponse(response);

            if (istr.peek() == EOF) {
                Omm::Av::Log::instance()->upnpav().error("web radio failed reading data from stream uri: " + *it);
                continue;
            }
            else {
                Omm::Av::Log::instance()->upnpav().debug("web radio success reading data from stream uri: " + *it);
            }

            Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason());
            std::stringstream responseHeader;
            response.write(responseHeader);
            Omm::Av::Log::instance()->upnpav().debug("proxy response header:\n" + responseHeader.str());
            return new Poco::Net::HTTPResponseStream(istr, pSession);
        }
    }
}


std::string
WebradioDataModel::getMime(const std::string& path)
{
    return "audio/mpeg";
}


std::string
WebradioDataModel::getDlna(const std::string& path)
{
    return "DLNA.ORG_PN=MP3;DLNA.ORG_OP=01";
}


void
WebradioDataModel::scanStationConfig(const std::string& stationConfig)
{
    Omm::Av::Log::instance()->upnpav().debug("web radio, start scanning station config file: " + stationConfig + " ...");
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
        Omm::Av::Log::instance()->upnpav().error("webradio config not found and scanning of stations not yet supported, giving up.");
        return;
    }

    Poco::XML::Node* pStationList = pDoc->firstChild();
    if (pStationList->nodeName() != "stationlist") {
        Omm::Av::Log::instance()->upnpav().error("error reading webradio station list, wrong file format");
        return;
    }

    std::string currentStationName;
    if (pStationList->hasChildNodes()) {
        Poco::XML::Node* pStation = pStationList->firstChild();
        Omm::Av::Log::instance()->upnpav().debug("stationlist first child: " + pStation->nodeName());
        while (pStation) {
            if (pStation->nodeName() != "station") {
                Omm::Av::Log::instance()->upnpav().error("error reading webradio station list, no station found.");
                return;
            }
            Poco::XML::Node* pProp = pStation->firstChild();
            while (pProp) {
                if (pProp->nodeName() == "name") {
                    currentStationName = pProp->innerText();
                    Omm::Av::Log::instance()->upnpav().debug("added web radio station with name: " + pProp->innerText());
                }
                else if (pProp->nodeName() == "uri") {
                    addPath(pProp->innerText());
                    _stationNames[pProp->innerText()] = currentStationName;
                    Omm::Av::Log::instance()->upnpav().debug("added web radio station with uri: " + pProp->innerText());
                }
                else {
                    Omm::Av::Log::instance()->upnpav().warning("webradio station entry in config file has unknown property");
                }
                pProp = pProp->nextSibling();
            }
            pStation = pStation->nextSibling();
        }
    }
    Omm::Av::Log::instance()->upnpav().debug("web radio, finished scanning station config file.");
}


void
WebradioServer::setBasePath(const std::string& basePath)
{
    ServerContainer::setBasePath(basePath);
    setDataModel(new WebradioDataModel(basePath));
    setClass(Omm::Av::AvClass::className(Omm::Av::AvClass::CONTAINER, Omm::Av::AvClass::AUDIO_BROADCAST));
}



#ifdef OMMPLUGIN
POCO_BEGIN_MANIFEST(Omm::Av::ServerContainer)
POCO_EXPORT_CLASS(WebradioServer)
POCO_END_MANIFEST
#endif
