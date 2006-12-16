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
#include "netsocket.h"
#include "debug.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <cstdio>
#include <cerrno>


NetSocket::NetSocket()
{
    TRACE("NetSocket::NetSocket()");
    if ((m_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        TRACE("NetSocket::NetSocket() socket creation failed with error: %s", strerror(errno));
    }
    else {
        TRACE("NetSocket::NetSocket() socket created");
    }
}


NetSocket::~NetSocket()
{
}


bool
NetSocket::open(string server, unsigned int port)
{
    TRACE("NetSocket::open() connection to server: %s, port: %i", server.c_str(), port);
//     m_bytesRead = 0;
//     m_bytesScanned = 0;
//     m_lineEndFound = false;

    sockaddr_in sin;
    hostent *host = gethostbyname(server.c_str());
    memcpy(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (connect(m_socket, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        TRACE("NetSocket::open() connect failed with error: %s", strerror(errno));
        return false;
    }
    else {
        TRACE("NetSocket::open() connect succeeded");
    }
    m_socketIn = fdopen(m_socket, "r");
    m_socketOut = fdopen(m_socket, "w");
    return true;
}


string
NetSocket::readLine()
{
    char buf[10240];
    char newline[2];
    int itemsRead;
    if ((itemsRead = fscanf(m_socketIn, "[^\r\n][\r\n]", buf, newline)) == EOF) {
        TRACE("NetSocket::readLine() read failed with error: %s", strerror(errno));
    }
    else {
        TRACE("NetSocket::readLine() read %i items", itemsRead);
    }
    return string(buf);
}


void
NetSocket::writeLine(string data)
{
    // write out the data.
    int bytesWritten;
    if ((bytesWritten = fprintf(m_socketOut, "%s\r\n", data.c_str())) < 0) {
        TRACE("NetSocket::writeLine() write failed with error: %s", strerror(errno));
    }
    else {
        TRACE("NetSocket::writeLine() written %i bytes", bytesWritten);
    }
    // flush stream buffers (C library internal).
    if (fflush(m_socketOut) != 0) {
        TRACE("NetSocket::writeLine() flush failed with error: %s", strerror(errno));
    }
    else {
        TRACE("NetSocket::writeLine() stream flushed");
    }
}


// string
// NetSocket::readLine()
// {
// //     TRACE("NetSocket::readLine()");
//     bool eof = false;
//     bool eol = false;
//     while (!eof && !eol) {
//         if (m_bytesScanned >= m_bytesRead) {
// //             TRACE("NetSocket::readLine() fetching new buffer");
//             // we need to fetch new data.
// //             m_bytesRead = read(m_socket, m_buf, m_bufSize);
//             m_bytesRead = recv(m_socket, m_buf, m_bufSize, MSG_WAITALL);
// //             usleep(100); // TODO: wait 0.1 milisec after each write. This must be fixed!
//             m_bytesScanned = 0;
//             m_strBuf = string(m_buf);
//         }
//         if (m_bytesRead < 0) {
// //             TRACE("NetSocket::readLine() failed with error: %s", strerror(errno));
//             break;
//         }
//         else if (m_bytesRead == 0) {
// //             TRACE("NetSocket::readLine() EOF reached");
//             m_bytesScanned = 0;
//             eof = true;
//         }
//         else {
//             // search for EOL in the new buffer
//             int m_bytesScannedOld = m_bytesScanned;
//             m_bytesScanned = m_strBuf.find_first_of("\r\n", m_bytesScannedOld);
//             if (m_bytesScanned == string::npos) {
//                 // no carriage return in the buffer
//                 m_bytesScanned = m_bytesRead;
//                 m_line.append(m_strBuf.substr(m_bytesScannedOld));
//             }
//             else {
//                 eol = true;
//                 // jump over carriage return
//                 m_bytesScanned++;
//                 // check if there's also a newline
//                 int newline = 0;
//                 if (m_bytesScanned <= m_bytesRead && 
//                     (m_strBuf[m_bytesScanned] == '\n') || (m_strBuf[m_bytesScanned] == '\r')) {
//                     // jump over newline
//                     m_bytesScanned++;
//                     newline = 1;
//                 }
//                 m_line.append(m_strBuf.substr(m_bytesScannedOld, m_bytesScanned - m_bytesScannedOld - newline));
//             }
// //             TRACE("NetSocket::readLine() read: %i, scanned: %i bytes", m_bytesRead, m_bytesScanned);
//             if (m_bytesScanned == m_bytesRead) {
//                 m_bytesScanned = 0;
//                 m_bytesRead = 0;
//             }
//         }
//     }
//     // get rid of a newline at the beginning of m_line, copy the result and return
//     string res = ((m_line[0] == '\n') || (m_line[0] == '\r'))?string(m_line, 1):m_line;
//     m_line = "";
// //     TRACE("NetSocket::readLine() returns: %s", res.c_str());
//     return res;
// }


// void
// NetSocket::writeLine(string data)
// {
//     TRACE("NetSocket::writeLine() data: %s", data.c_str());
//     int bytesWritten = write(m_socket, (data + "\r\n").c_str(), data.length() + 2);
// //     usleep(100000);  // TODO: wait 100 milisec after sending the request. This must be fixed!
//     if (bytesWritten < 0) {
//         TRACE("NetSocket::writeLine() failed with error: %s", strerror(errno));
//     }
//     else {
//         TRACE("NetSocket::writeLine() written %i bytes", bytesWritten);
//     }
// }


void
NetSocket::close()
{
    ::fclose(m_socketIn);
    ::fclose(m_socketOut);
    ::close(m_socket);
}


void
NetSocket::setBlocking(bool enable)
{
    int tmp = fcntl(m_socket, F_GETFL, 0);
    if (tmp >= 0) {
        TRACE("NetSocket::setBlocking() setting to %s", enable ? "blocking" : "not blocking");
        tmp = fcntl(m_socket, F_SETFL, enable ? (tmp&~O_NDELAY) : (tmp|O_NDELAY));
    }
}
