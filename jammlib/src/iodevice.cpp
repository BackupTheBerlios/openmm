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

#include "iodevice.h"
#include "debug.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <cerrno>

using namespace Jamm;

JIoDevice::JIoDevice(bool blocking)
: m_blocking(blocking)
{
}


JIoDevice::~JIoDevice()
{
}


bool
JIoDevice::open(const char* pathname)
{
    m_bytesRead = 0;
    m_bytesScanned = 0;
    m_lineEndFound = false;
    
    m_device = ::open(pathname, m_blocking?(O_RDWR):(O_RDWR | O_NONBLOCK));
    
    return true;
}


bool
JIoDevice::readLine(string& res, int milliSec)
{
//     TRACE("JIoDevice::readLine()");
    bool eof = false;
    bool eol = false;
    bool noTimeout = true;
    while (!eof && !eol) {
        if (m_bytesScanned >= m_bytesRead) {
//             TRACE("JIoDevice::readLine() fetching new buffer");
            // need to fetch new data.
            noTimeout = readBuf(m_bytesRead, milliSec);
            m_bytesScanned = 0;
        }
        if (m_bytesRead < 0) {
            TRACE("JIoDevice::readLine() failed with error: %s", strerror(errno));
            break;
        }
        else if (m_bytesRead == 0) {
            TRACE("JIoDevice::readLine() EOF reached");
            m_bytesScanned = 0;
            eof = true;
        }
        else {
            // search for EOL in the new buffer
            int m_bytesScannedOld = m_bytesScanned;
            m_bytesScanned = m_strBuf.find_first_of("\r\n", m_bytesScannedOld);
            if (m_bytesScanned == string::npos) {
                // no carriage return in the buffer
                m_bytesScanned = m_bytesRead;
                m_line.append(m_strBuf.substr(m_bytesScannedOld));
            }
            else {
                eol = true;
                // jump over carriage return
                m_bytesScanned++;
                // check if there's also a newline
                int newline = 0;
                if ((m_bytesScanned <= m_bytesRead) && 
                    (m_strBuf[m_bytesScanned] == '\n') || (m_strBuf[m_bytesScanned] == '\r')) {
                    // jump over newline
                    m_bytesScanned++;
                    newline = 1;
                }
                m_line.append(m_strBuf.substr(m_bytesScannedOld, m_bytesScanned - m_bytesScannedOld - newline));
            }
//             TRACE("JIoDevice::readLine() read: %i, scanned: %i bytes", m_bytesRead, m_bytesScanned);
            if (m_bytesScanned == m_bytesRead) {
                m_bytesScanned = 0;
                m_bytesRead = 0;
            }
        }
    }
    // get rid of a newline at the beginning of m_line, copy the result and return
    /*string*/ res = ((m_line[0] == '\n') || (m_line[0] == '\r'))?string(m_line, 1):m_line;
    m_line = "";
//     TRACE("JIoDevice::readLine() returns %i bytes: %s", res.length(), res.c_str());
    return noTimeout;
}


bool
JIoDevice::readBuf(int& bytesReadSum, int milliSec)
{
//     TRACE("JIoDevice::readBuf()");
    int bytesRead = 0;
    int bytesLeft = m_bufSize;
    bytesReadSum = 0;
    m_strBuf = "";
    while (bytesLeft) {
        if (isReadable(milliSec)) {
            bytesRead = read(m_device, m_buf, bytesLeft);
//             TRACE("JIoDevice::readBuf() bytesRead: %i", bytesRead);
        }
        else {
//             TRACE("JIoDevice::readBuf() timed out");
            return false;
        }
        bytesReadSum += bytesRead;
        bytesLeft = m_bufSize - bytesReadSum;
        if (bytesRead == -1) { // read failure
            return false;
        }
        else if (bytesRead == 0) { // EOF  
            return true;
        }
        m_strBuf.append(string(m_buf).substr(0, bytesRead));
        // if the last byte read is a newline or carriage return, we don't read any further
        // because this could be the end of the reply (EOF).
        if (m_strBuf.find_last_of("\r\n") == m_strBuf.length()-1) {
            break;
        }
    }
    return true;
}


// void
// JIoDevice::writeLine(string data)
// {
// //     TRACE("JIoDevice::writeLine() data: %s", data.c_str());
//     string outString = data + "\r\n";
//     int bytesWritten = 0;
//     int bytesWrittenSum = 0;
//     int bytesLeft = outString.length();
//     while (bytesLeft) {
//         if (isWriteable()) {
//             bytesWritten = write(m_socket, outString.substr(bytesWritten).c_str(), bytesLeft);
//         }
//         else {
//             TRACE("JIoDevice::writeLine() timed out");
//             return;
//         }
//         bytesWrittenSum += bytesWritten;
//         bytesLeft = outString.length() - bytesWrittenSum;
//         if (bytesWritten == -1) {
//             TRACE("JIoDevice::writeLine() failed with error: %s", strerror(errno));
//             return;
//         }
//         else {
// //             TRACE("JIoDevice::writeLine() written %i bytes", bytesWritten);
//         }
//     }
// }


void
JIoDevice::close()
{
    ::close(m_device);
}


bool
JIoDevice::isReadable(int milliSec)
{
//     TRACE("JIoDevice::isReadable()");
    if (m_blocking) {
        return true;
    }
    m_tv.tv_sec = milliSec/1000;
    m_tv.tv_usec = (milliSec%1000)*1000;
    FD_ZERO(&m_readfds);
    FD_SET(m_device, &m_readfds);
    if (select(m_device+1, &m_readfds, NULL, NULL, &m_tv) == -1) {
        TRACE("JIoDevice::isReadable() failed with error: %s", strerror(errno));
    }
    return FD_ISSET(m_device, &m_readfds);
}


// bool
// JIoDevice::isWriteable()
// {
// //     TRACE("JIoDevice::isWriteable()");
//     if (m_blocking) {
//         return true;
//     }
//     FD_ZERO(&m_writefds);
//     FD_SET(m_socket, &m_writefds);
//     if (select(m_socket+1, NULL, &m_writefds, NULL, &m_tv) == -1) {
//         TRACE("JIoDevice::isWriteable() failed with error: %s", strerror(errno));
//     }
//     return FD_ISSET(m_socket, &m_writefds);
// }
