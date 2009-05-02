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

#ifndef JAMMIODEVICE_H
#define JAMMIODEVICE_H

#include <sys/time.h>

#include <string>
using namespace std;

namespace Jamm {

/**
Simple socket encapsulation with methods for handling a line-based protocol.
readLine() is buffered and attempts to read blocks of 1024 bytes from the network.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/

class JIoDevice
{
public:
    JIoDevice(bool blocking = false);
    ~JIoDevice();

    bool open(const char* pathname);
    bool readLine(string& line, int milliSec);
//     void writeLine(string);
    void close();

private:
    bool readBuf(int& bytesRead, int milliSec);
//     void setBlocking(bool enable);
    bool isReadable(int milliSec);
//     bool isWriteable();

    int                m_device;
    bool               m_blocking;

    timeval            m_tv;
    fd_set             m_readfds;
    fd_set             m_writefds;

    static const int   m_bufSize = 1024;
    char               m_buf[m_bufSize];
//     string::size_type  m_bytesRead;
//     string::size_type  m_bytesScanned;
    int  m_bytesRead;
    int  m_bytesScanned;
    bool               m_lineEndFound;
    string             m_line;
    string             m_strBuf;
};

} // namespace Jamm

#endif
