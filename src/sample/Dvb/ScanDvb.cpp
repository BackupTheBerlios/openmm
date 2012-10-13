/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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

#include <iostream>
#include <Poco/StreamCopier.h>
#include <sstream>
#include <Omm/Dvb/Device.h>
#include <Omm/Dvb/Frontend.h>


int
main(int argc, char** argv) {
    Omm::Dvb::Adapter* pAdapter = new Omm::Dvb::Adapter(0);
    Omm::Dvb::Frontend* pFrontend = new Omm::Dvb::TerrestrialFrontend(pAdapter, 0);
//    Omm::Dvb::Frontend* pFrontend = new Omm::Dvb::SatFrontend(pAdapter, 0);
    Omm::Dvb::Device::instance()->addAdapter(pAdapter);
    pAdapter->addFrontend(pFrontend);

//    pFrontend->listInitialTransponderData();

    pFrontend->scan("dvb-t/de-Baden-Wuerttemberg");
//    pFrontend->scan("dvb-s/Astra-19.2E");

    Omm::Dvb::Device::instance()->writeXml(std::cout);

//    std::ifstream xmlDevice("/home/jb/tmp/dvb.xml");
//    Omm::Dvb::Device::instance()->readXml(xmlDevice);

    return 0;
}
