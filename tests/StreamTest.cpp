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
#include <iostream>
#include <string>
#include <sstream>

#include <Poco/Environment.h>
#include <Poco/DirectoryIterator.h>
#include <Poco/Exception.h>
#include <Poco/Thread.h>
#include <Poco/Random.h>
#include <Poco/NumberFormatter.h>
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

// #include <AvStream.h>
// #include <UpnpAvController.h>

// const std::string liveStreamAddress = "http://anubis:8888/";
const std::string fileStreamAddress = "http://192.168.178.28:50057/";

void
downloadStream(std::vector<std::string>& uris)
{
    for(std::vector<std::string>::iterator it = uris.begin(); it != uris.end(); ++it) {
        std::clog << "downloading stream: " << *it << std::endl;
        Poco::URI uri(*it);
        Poco::Net::HTTPClientSession session;
        session.setHost(uri.getHost());
        session.setPort(uri.getPort());
        Poco::Net::HTTPRequest request("GET", uri.getPath());
        session.sendRequest(request);
        std::stringstream requestHeader;
        request.write(requestHeader);
        std::clog << "request header:\n" + requestHeader.str();
        
        Poco::Net::HTTPResponse response;
        std::istream& istr = session.receiveResponse(response);
        
//         if (istr.peek() == EOF) {
//             Omm::Av::Log::instance()->upnpav().error("error web radio reading data from web resource");
//         }
//         else {
//             Omm::Av::Log::instance()->upnpav().debug("web radio success reading data from web resource");
//         }
        
        std::clog << "HTTP " + Poco::NumberFormatter::format(response.getStatus()) + " " + response.getReason() << std::endl;
        std::stringstream responseHeader;
        response.write(responseHeader);
        std::clog << "response header:\n" + responseHeader.str();
        
        const int bufferSize = 1024 * 1024;
        char buffer[bufferSize];
        while (istr) {
//             istr.read(buffer, bufferSize);
//             std::clog << "read " << Poco::NumberFormatter::format(istr.gcount()) << " bytes" << std::endl;
            int bytes = istr.readsome(buffer, bufferSize);
            std::clog << "read " << Poco::NumberFormatter::format(bytes) << " bytes" << std::endl;
        }
    }
}


int
main(int argc, char** argv)
{
    std::vector<std::string> fileStreams;
    fileStreams.push_back(fileStreamAddress + Poco::NumberFormatter::format(0) + "$0");
    
    downloadStream(fileStreams);
}
