/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
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

#ifndef VDRMEDIASERVER_H
#define VDRMEDIASERVER_H

#include <Jamm/UpnpAvTypes.h>
#include <Jamm/UpnpAvServer.h>

/**
	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/


class VdrChannels : public Jamm::Av::MediaContainer
{
public:
    VdrChannels();
    
private:
    Jamm::Av::MediaItemServer*  m_pItemServer;
};


class VdrRecordings : public Jamm::Av::MediaContainer
{
public:
    VdrRecordings();
    
private:
    Jamm::Av::MediaItemServer*  m_pItemServer;
};



// class MultiFileInputStream : public NPT_InputStream
// {
// public:
//     MultiFileInputStream(string path);
//     virtual ~MultiFileInputStream();
//     
//     virtual NPT_Result Read(void*     buffer, 
//                             NPT_Size  bytes_to_read, 
//                             NPT_Size* bytes_read = NULL);
//     virtual NPT_Result Seek(NPT_Position offset);
//     virtual NPT_Result Tell(NPT_Position& offset);
//     virtual NPT_Result GetSize(NPT_LargeSize& size);
//     virtual NPT_Result GetAvailable(NPT_LargeSize& available);
//     
// private:
//     map<long long, FILE*>           m_streams;
//     map<long long, FILE*>::iterator m_currentStream;
//     long long                       m_totalSize;
// };

#endif
