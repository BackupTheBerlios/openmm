/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg<at>hakker<dot>de   						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2 (not   *
 *   v2.2 or v3.x or other) as published by the Free Software Foundation.  *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "iodevice.h"
#include "debug.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <cerrno>


JIoDevice::JIoDevice(bool blocking)
: m_blocking(blocking)
{
/*    TRACE("JIoDevice::JIoDevice()");
    if ((m_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        TRACE("JIoDevice::JIoDevice() socket creation failed with error: %s", strerror(errno));
    }
    else {
        TRACE("JIoDevice::JIoDevice() socket created");*/
//     }
//     setBlocking(blocking);
    m_tv.tv_sec = m_timeout;
    m_tv.tv_usec = 0;
}


JIoDevice::~JIoDevice()
{
}


bool
JIoDevice::open(const char* pathname)
{
//     TRACE("JIoDevice::open() connection to server: %s, port: %i", server.c_str(), port);
    m_bytesRead = 0;
    m_bytesScanned = 0;
    m_lineEndFound = false;

/*    sockaddr_in sin;
    hostent *host = gethostbyname(server.c_str());
    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (connect(m_socket, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        // 
        if (errno == EINPROGRESS) {
            TRACE("JIoDevice::open() %s", strerror(errno));
            // TODO: do a select and wait until connection is established
            //       or print an error if not succesfull.
        }
        else {
            TRACE("JIoDevice::open() connect failed with error: %s", strerror(errno));
            ::close(m_socket);
            return false;
        }
    }
    else {
        TRACE("JIoDevice::open() connect succeeded");
    }*/
    
    m_device = ::open(pathname, m_blocking?(O_RDWR):(O_RDWR | O_NONBLOCK));
    
    return true;
}

// TODO: maybe try getline(stream, string, delimiter) of the standard C++ library?
//       fgets() from the standard C library seemed to block.

int
JIoDevice::readLine(string& res)
{
    TRACE("JIoDevice::readLine()");
    bool eof = false;
    bool eol = false;
    while (!eof && !eol) {
        if (m_bytesScanned >= m_bytesRead) {
            TRACE("JIoDevice::readLine() fetching new buffer");
            // we need to fetch new data.
            m_bytesRead = readBuf();
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
            TRACE("JIoDevice::readLine() read: %i, scanned: %i bytes", m_bytesRead, m_bytesScanned);
            if (m_bytesScanned == m_bytesRead) {
                m_bytesScanned = 0;
                m_bytesRead = 0;
            }
        }
    }
    // get rid of a newline at the beginning of m_line, copy the result and return
    /*string*/ res = ((m_line[0] == '\n') || (m_line[0] == '\r'))?string(m_line, 1):m_line;
    m_line = "";
//     TRACE("JIoDevice::readLine() returns: %s", res.c_str());
//     return res;
    return res.length();
}


int
JIoDevice::readBuf()
{
    TRACE("JIoDevice::readBuf()");
    int bytesRead = 0;
    int bytesReadSum = 0;
    int bytesLeft = m_bufSize;
    m_strBuf = "";
    while (bytesLeft) {
        if (isReadable()) {
           bytesRead = read(m_device, m_buf, bytesLeft);
        }
        else {
            TRACE("JIoDevice::readBuf() timed out");
            return bytesReadSum;
        }
        TRACE("JIoDevice::readBuf() bytesRead: %i", bytesRead);
        bytesReadSum += bytesRead;
        bytesLeft = m_bufSize - bytesReadSum;
        if (bytesRead == -1) { // read failure
            return bytesRead;
        }
        else if (bytesRead == 0) { // EOF  
            return bytesReadSum;
        }
        m_strBuf.append(string(m_buf).substr(0, bytesRead));
        // if the last byte read is a newline or carriage return, we don't read any further
        // because this could be the end of the reply (EOF).
        if (m_strBuf.find_last_of("\r\n") == m_strBuf.length()-1) {
            break;
        }
    }
    return bytesReadSum;
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


// void
// JIoDevice::setBlocking(bool enable)
// {
//     int tmp = fcntl(m_socket, F_GETFL, 0);
//     if (tmp >= 0) {
//         TRACE("JIoDevice::setBlocking() setting to %s", enable ? "blocking" : "not blocking");
//         tmp = fcntl(m_socket, F_SETFL, enable ? (tmp&~O_NDELAY) : (tmp|O_NDELAY));
//     }
//     m_blocking = enable;
// }


bool
JIoDevice::isReadable()
{
//     TRACE("JIoDevice::isReadable()");
    if (m_blocking) {
        return true;
    }
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
