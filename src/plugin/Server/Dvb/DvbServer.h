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

#ifndef DvbServer_INCLUDED
#define DvbServer_INCLUDED

#include <Poco/DateTime.h>

#include <Omm/UpnpAvServer.h>
#include <Omm/Dvb/Device.h>


class DvbModel : public Omm::Av::SimpleDataModel
{
    friend class DvbServer;

public:
    DvbModel();

    virtual void init();
    virtual void deInit();
    virtual std::string getModelClass();
    virtual Omm::ui4 getSystemUpdateId(bool checkMod);
    virtual void scan();

    virtual std::string getClass(const std::string& path);
    virtual std::string getTitle(const std::string& path);

    virtual std::string getMime(const std::string& path);
    virtual std::string getDlna(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");
    virtual void freeStream(std::istream* pIstream);
};


#endif
