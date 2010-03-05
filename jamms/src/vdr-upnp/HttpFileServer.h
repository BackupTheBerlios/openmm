/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009 - 2010                                                |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
 ***************************************************************************/

#ifndef HttpFileServer_INCLUDED
#define HttpFileServer_INCLUDED

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>


class FileRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
//     DescriptionRequestHandler(std::string* pDescription);
    
//     DescriptionRequestHandler* create();
    
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    
private:
//     std::string*    m_pDescription;
};


class FileRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
//     FileRequestHandlerFactory(HttpSocket* pHttpSocket);
    
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
    
//     void registerRequestHandler(std::string Uri, UpnpRequestHandler* requestHandler);
    
private:
//     std::map<std::string,UpnpRequestHandler*> m_requestHandlerMap;
//     HttpSocket*                               m_pHttpSocket;
};


#endif
