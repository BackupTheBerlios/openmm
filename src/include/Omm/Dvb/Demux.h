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

#include <linux/dvb/dmx.h>
#include <sys/poll.h>

namespace Omm {
namespace Dvb {

class Adapter;

class Demux
{
    friend class Adapter;

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

private:
    bool setPid(int fileDesc, unsigned int pid, dmx_pes_type_t pesType);
    bool setSectionFilter(Stream* pStream, uint8_t filter[18], uint8_t mask[18]);

    Adapter*                    _pAdapter;
    std::string                 _deviceName;
    int                         _num;
};

}  // namespace Omm
}  // namespace Dvb

#endif
