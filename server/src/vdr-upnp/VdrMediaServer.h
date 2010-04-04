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

#ifndef VDRMEDIASERVER_H
#define VDRMEDIASERVER_H

#include <Omm/UpnpAvTypes.h>
#include <Omm/UpnpAvServer.h>

/**
	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/


class VdrChannels : public Omm::Av::MediaContainer
{
public:
    VdrChannels();
    
private:
    Omm::Av::MediaItemServer*  m_pItemServer;
};


class VdrRecordings : public Omm::Av::MediaContainer
{
public:
    VdrRecordings();
    
private:
    Omm::Av::MediaItemServer*  m_pItemServer;
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
