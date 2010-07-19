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

#ifndef Filesystem_INCLUDED
#define Filesystem_INCLUDED

#include <Omm/UpnpAvServer.h>

class FileServer : public Omm::Av::MediaServerContainer
{
public:
    FileServer();
    
    virtual AbstractMediaObject* getChild(const std::string& objectId) { return 0; }
    virtual AbstractMediaObject* getObject(const std::string& objectId) { return 0; }        // server object, cds browse

    virtual ui4 getChildCount() { return 0; }                                                // server object, cds browse / write meta data
                                                                                // controller object, browse
    virtual bool isContainer() { return true; }                                                 // server object, write meta data
                                                                                // controller object, browse
    virtual AbstractMediaObject* getChild(ui4 numChild) { return 0; }                        // server object, write meta data
                                                                                // controller object, browse

    virtual std::string getObjectId() { return ""; }                                          // server object, write meta data
    virtual bool isRestricted() { return false; }                                                // server object, write meta data
    virtual int getPropertyCount(const std::string& name = "") { return 0; }
    virtual AbstractProperty* getProperty(int index) { return 0; }
    virtual AbstractProperty* getProperty(const std::string& name, int index = 0) { return 0; }             // server object, write meta data
    virtual AbstractProperty* getProperty(const std::string& name, const std::string& value) { return 0; }  // server object, write meta data
};

#endif
