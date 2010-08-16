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
#include <Poco/ClassLibrary.h>

#include "WebRadio.h"


class WebradioDataModel : public Omm::Av::AbstractDataModel
{
public:
    WebradioDataModel(const std::string& stationConfig);
    
    virtual Omm::ui4 getChildCount();
    virtual std::string getTitle(Omm::ui4 index);
    
    virtual Omm::ui4 getSize(Omm::ui4 index);
    virtual std::string getMime(Omm::ui4 index);
    virtual std::string getDlna(Omm::ui4 index);
    virtual bool isSeekable(Omm::ui4 index);
    virtual std::streamsize stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek);

private:
    void scanStationConfig(const std::string& stationConfig);
    
    std::vector<std::string>             _stationNames;
    std::vector<std::string>             _stationUris;
};


WebradioDataModel::WebradioDataModel(const std::string& stationConfig)
{
    scanStationConfig(stationConfig);
}


Omm::ui4
WebradioDataModel::getChildCount()
{
    return _stationNames.size();
}


std::string
WebradioDataModel::getTitle(Omm::ui4 index)
{
    return _stationNames[index];
}


bool
WebradioDataModel::isSeekable(Omm::ui4 index)
{
    return false;
}


std::streamsize
WebradioDataModel::stream(Omm::ui4 index, std::ostream& ostr, std::iostream::pos_type seek)
{
    Poco::URI uri(_stationUris[index]);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    session.setKeepAlive(true);
    session.setKeepAliveTimeout(Poco::Timespan(3, 0));
    Poco::Timespan timeout = session.getKeepAliveTimeout();
    Omm::Av::Log::instance()->upnpav().debug("web resource server proxy timeout is: " + Poco::NumberFormatter::format(timeout.seconds() + "sec"));
    Poco::Net::HTTPRequest proxyRequest("GET", uri.getPath());
    proxyRequest.setKeepAlive(true);
    session.sendRequest(proxyRequest);
    std::stringstream requestHeader;
    proxyRequest.write(requestHeader);
    Omm::Av::Log::instance()->upnpav().debug("proxy request header:\n" + requestHeader.str());
    
    Poco::Net::HTTPResponse proxyResponse;
    std::istream& istr = session.receiveResponse(proxyResponse);
    
    if (istr.peek() == EOF) {
        Omm::Av::Log::instance()->upnpav().error("error reading data from web resource");
    }
    else {
        Omm::Av::Log::instance()->upnpav().debug("success reading data from web resource");
    }
    
    Omm::Av::Log::instance()->upnpav().information("HTTP " + Poco::NumberFormatter::format(proxyResponse.getStatus()) + " " + proxyResponse.getReason());
    std::stringstream responseHeader;
    proxyResponse.write(responseHeader);
    Omm::Av::Log::instance()->upnpav().debug("proxy response header:\n" + responseHeader.str());
    
    std::streamsize bytes = Poco::StreamCopier::copyStream(istr, ostr);
    return bytes;
}


Omm::ui4
WebradioDataModel::getSize(Omm::ui4 index)
{
    return 0;
}


std::string
WebradioDataModel::getMime(Omm::ui4 index)
{
    return "audio/mpeg";
}


std::string
WebradioDataModel::getDlna(Omm::ui4 index)
{
    return "DLNA.ORG_PN=MP3;DLNA.ORG_OP=01";
}


void
WebradioDataModel::scanStationConfig(const std::string& stationConfig)
{
    Omm::Av::Log::instance()->upnpav().debug("web radio, start scanning station config file ...");
    Poco::XML::DOMParser parser;
    parser.setFeature(Poco::XML::DOMParser::FEATURE_WHITESPACE, false);
    Poco::XML::Document* pDoc = parser.parse(stationConfig);
    
    Poco::XML::Node* pStationList = pDoc->firstChild();
    if (pStationList->nodeName() != "stationlist") {
        Omm::Av::Log::instance()->upnpav().error("error reading webradio station list, wrong file format");
        return;
    }
    if (pStationList->hasChildNodes()) {
        Poco::XML::Node* pStation = pStationList->firstChild();
        std::clog << "stationlist first child: " << pStation->nodeName() << std::endl;
        while (pStation) {
            if (pStation->nodeName() != "station") {
                Omm::Av::Log::instance()->upnpav().error("error reading webradio station list, no station found.");
                return;
            }
            Poco::XML::Node* pProp = pStation->firstChild();
            while (pProp) {
                if (pProp->nodeName() == "name") {
                    _stationNames.push_back(pProp->innerText());
                    Omm::Av::Log::instance()->upnpav().debug("added web radio station with name: " + pProp->innerText());
                }
                else if (pProp->nodeName() == "uri") {
                    _stationUris.push_back(pProp->innerText());
                    Omm::Av::Log::instance()->upnpav().debug("added web radio station with uri: " + pProp->innerText());
                }
                else {
                    Omm::Av::Log::instance()->upnpav().warning("webradio station entry in config file has properties");
                }
                pProp = pProp->nextSibling();
            }
            pStation = pStation->nextSibling();
        }
    }
    Omm::Av::Log::instance()->upnpav().debug("web radio, finished scanning station config file.");
}


void
WebradioServer::setOption(const std::string& key, const std::string& value)
{
    if (key == "basePath") {
        setDataModel(new WebradioDataModel(value));
    }
}


POCO_BEGIN_MANIFEST(Omm::Av::AbstractMediaObject)
POCO_EXPORT_CLASS(WebradioServer)
POCO_END_MANIFEST
