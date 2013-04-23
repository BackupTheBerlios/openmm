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

#ifndef IPhoneCameraModel_INCLUDED
#define IPhoneCameraModel_INCLUDED

#include <Omm/UpnpAvObject.h>
#include <Omm/UpnpAvServer.h>
#include <Omm/AvStream.h>


//class IPhoneCameraItem;

class IPhoneCameraModel : public Omm::Av::AbstractDataModel
{
public:
    IPhoneCameraModel();
    ~IPhoneCameraModel();

    virtual std::string getModelClass();
    virtual void scan();
    virtual bool useObjectCache() { return false; }

    virtual Omm::Av::ServerObject* getMediaObject(const std::string& path);
    virtual std::streamsize getSize(const std::string& path);
    virtual bool isSeekable(const std::string& path, const std::string& resourcePath = "");
    virtual std::istream* getStream(const std::string& path, const std::string& resourcePath = "");
    virtual void freeStream(std::istream* pIstream);

private:
    void*                               _pCaptureSession;
};


#endif
