/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker   				   *
 *   joerg@hakker.de   							   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
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
#ifndef MRL_H
#define MRL_H

// #include <qobject.h>
#include <vector>
#include <string>

using namespace std;


/**
An Mrl is a unique indentifier for a Title. It specifies the type (protocol), server and path to the media resource.

	@author Jörg Bakker <joerg@hakker.de>
*/
class Mrl //: public QObject
{
//     Q_OBJECT

public:
    enum MrlT {None, TvVdr};
    Mrl(string protocol, string path, string server = "", MrlT type = None);
    ~Mrl();

//    void setProtocol(string protocol) { m_protocol = protocol; }
//    void setServer(string server) { m_server = server; }
//    void setPath(string path) { m_path.append(path); }
//    void setType(MrlT type) { m_type = type; }

    string getProtocol() { return m_protocol; }
    string getServer() { return m_server; }  // complete mrl is assembled in StreamPlayer
    string getPath() { return m_path; }     // from elements protocol, server, and path
    MrlT    getType() { return m_type; }

    void setFiles(vector<string> files) { m_files = files; }
    vector<string> getFiles() { return m_files; }

protected:
    string m_path;
    /**
    m_files is a needed for resource that points to several data files, where the media is stored (split into).
    Example: vdr recordings in a filesystem.
    */
    vector<string> m_files;

private:
    string m_protocol;  // internal protocol naming: e.g. tvchan://, tvprog://, vdrchan://
                         // this is set only by the constructor of the derived class, e.g. TvChannel, TvProgram
    string m_server;  // encode the type of server? e.g. 192.168.1.10(vdr) or better store it in m_protocol?
    MrlT m_type;
};

#endif
