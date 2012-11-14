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

#include <Omm/Util.h>
#include <Omm/Dvb/DvbLogger.h>
#include <Omm/Dvb/Device.h>
#include <Omm/Dvb/Frontend.h>
#include <Omm/Dvb/Transponder.h>
#include <Omm/Dvb/Service.h>


int
main(int argc, char** argv) {
    const unsigned int streamBufPacketCount = 1000;

    // record 10 MiB of full multiplex
//    const std::streamsize bufSize = 6 * 1024 * 1024;
//    unsigned int serviceStreamPacketCount = 2;
    // record 2 MiB of each stream (video)
//    const std::streamsize bufSize = 2 * 1024 * 1024;
    unsigned int serviceStreamPacketCount = 40; // get 10,000 packets of service stream (not full multiplex)
    // record 100 KiB of each stream (audio)
//    const std::streamsize bufSize = 100 * 1024;
//    unsigned int serviceStreamPacketCount = 10;

    Omm::Dvb::Adapter* pAdapter = new Omm::Dvb::Adapter(0);
    Omm::Dvb::Frontend* pFrontend = new Omm::Dvb::TerrestrialFrontend(pAdapter, 0);
//    Omm::Dvb::Frontend* pFrontend = new Omm::Dvb::SatFrontend(pAdapter, 0);
    pAdapter->addFrontend(pFrontend);
    Omm::Dvb::Device* pDevice = Omm::Dvb::Device::instance();
    pDevice->addAdapter(pAdapter);

    std::ifstream dvbXml(Omm::Util::Home::instance()->getConfigDirPath("dvb.xml").c_str());
    pDevice->readXml(dvbXml);
    pDevice->open();

//    for (Omm::Dvb::Device::ServiceIterator it = pDevice->serviceBegin(); it != pDevice->serviceEnd(); ++it) {
//        Omm::Dvb::Device::ServiceIterator it = pDevice->serviceBegin();
//        std::string serviceName = it->first;
//        std::string serviceName("3sat");
        std::string serviceName("EinsPlus");

        Omm::Dvb::Transponder* pTransponder = pDevice->getTransponder(serviceName);
        if (pTransponder) {
            Omm::Dvb::Service* pService = pTransponder->getService(serviceName);
            if (pService && pService->getStatus() == Omm::Dvb::Service::StatusRunning && !pService->getScrambled()
                    && (pService->isAudio() || pService->isSdVideo())) {
                LOGNS(Omm::Dvb, dvb, debug, "recording service: " + serviceName);
                // only get elementary streams of service muxed to a transport stream
                std::istream* pDvbStream = pDevice->getStream(serviceName);
                if (pDvbStream) {
                    std::ofstream serviceStream((serviceName + std::string(".ts")).c_str());
                    const std::streamsize bufSize = 188 * streamBufPacketCount;
                    char buf[bufSize];
                    LOGNS(Omm::Dvb, dvb, debug, "reading stream packets total: " + Poco::NumberFormatter::format(serviceStreamPacketCount * streamBufPacketCount));
                    while (serviceStreamPacketCount--) {
                        pDvbStream->read(buf, bufSize);
                        serviceStream.write(buf, bufSize);
                        LOGNS(Omm::Dvb, dvb, debug, "received stream packets: " + Poco::NumberFormatter::format(streamBufPacketCount));
                    }
                    pDevice->freeStream(pDvbStream);
                }
            }
//        }
    }
    pDevice->close();

    return 0;
}
