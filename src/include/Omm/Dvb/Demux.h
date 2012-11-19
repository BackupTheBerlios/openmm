/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011, 2012                                     |
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

#ifndef Demux_INCLUDED
#define Demux_INCLUDED

#include <map>


namespace Omm {
namespace Dvb {

class Adapter;
class TransportStreamPacket;
class Multiplex;
class Remux;

class Demux
{
    friend class Adapter;
    friend class Device;

public:
    enum Target { TargetDemux, TargetDvr };

    Demux(Adapter* pAdapter, int num);
    ~Demux();


    bool selectService(Service* pService, Target target, bool blocking = true);
    bool unselectService(Service* pService);
    bool runService(Service* pService, bool run = true);

    bool selectStream(Stream* pStream, Target target, bool blocking = true);
    bool unselectStream(Stream* pStream);
    bool runStream(Stream* pStream, bool run = true);
    bool setSectionFilter(Stream* pStream, Poco::UInt8 tableId);

    bool readSection(Section* pSection);
    bool readTable(Table* pTable);

private:
    int pidRefCount(Poco::UInt16 pid);
    bool incPidRefCount(Poco::UInt16 pid);
    bool decPidRefCount(Poco::UInt16 pid);

    Adapter*                            _pAdapter;
    std::string                         _deviceName;
    int                                 _num;
    std::map<Poco::UInt16, int>         _pidRefCount;

    // currently not used:
//    Multiplex*                          _pMultiplex;
//    Remux*                              _pRemux;
};

}  // namespace Omm
}  // namespace Dvb

#endif
