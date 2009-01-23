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
#ifndef JAMMIODEVICE_H
#define JAMMIODEVICE_H

#include <sys/time.h>

#include <string>
using namespace std;


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

    static const int   m_timeout = 5; // timeout in seconds.
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

#endif
