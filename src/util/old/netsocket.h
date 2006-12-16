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
#ifndef NETSOCKET_H
#define NETSOCKET_H

#include <string>
using namespace std;


/**
Simple socket encapsulation.

	@author Jörg Bakker <joerg<at>hakker<dot>de>
*/

class NetSocket
{
public:
    NetSocket();
    ~NetSocket();

    bool open(string server, unsigned int port);
    string readLine();
    void writeLine(string);
    void close();
    void setBlocking(bool enable);

private:
    int                m_socket;
    FILE              *m_socketIn;
    FILE              *m_socketOut;
/*
    static const int   m_bufSize = 1024;
    char               m_buf[m_bufSize];
    int                m_bytesRead;
    int                m_bytesScanned;
    bool               m_lineEndFound;
    string             m_line;
    string             m_strBuf;*/
};

#endif
