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

#include <Poco/Timestamp.h>

#include <Omm/Util.h>
#include <Omm/Dvb/DvbLogger.h>
#include <Omm/Dvb/Device.h>
#include <Omm/Dvb/Frontend.h>
#include <Omm/Dvb/Transponder.h>
#include <Omm/Dvb/Service.h>
#include <Omm/Dvb/TransportStream.h>
#include <fstream>


void
recordService(std::string serviceName)
{
    Omm::Dvb::Device* pDevice = Omm::Dvb::Device::instance();
    Poco::Timestamp t;
    Poco::Timestamp::TimeDiff maxTime = 5000000;  // record approx. 5,000,000 microsec
    const unsigned int streamBufPacketCount = 500;

    Omm::Dvb::Transponder* pTransponder = pDevice->getFirstTransponder(serviceName);
    if (pTransponder) {
        Omm::Dvb::Service* pService = pTransponder->getService(serviceName);
        if (pService && pService->getStatus() == Omm::Dvb::Service::StatusRunning && !pService->getScrambled()
                && (pService->isAudio() || pService->isSdVideo())) {
            LOGNS(Omm::Dvb, dvb, information, "recording service: " + serviceName);
            std::istream* pDvbStream = pDevice->getStream(serviceName);
            if (pDvbStream) {
                std::ofstream serviceStream((serviceName + std::string(".ts")).c_str());
                const std::streamsize bufSize = Omm::Dvb::TransportStreamPacket::Size * streamBufPacketCount;
                char buf[bufSize];
                while (t.elapsed() < maxTime) {
                    pDvbStream->read(buf, bufSize);
                    serviceStream.write(buf, bufSize);
                    LOGNS(Omm::Dvb, dvb, debug, "received stream packets: " + Poco::NumberFormatter::format(streamBufPacketCount));
                }
                pDevice->freeStream(pDvbStream);
            }
        }
    }
}


int
main(int argc, char** argv)
{
    Omm::Dvb::Device* pDevice = Omm::Dvb::Device::instance();
    std::ifstream dvbXml;
    std::string serviceName;

    if (argc < 2 || argc > 3) {
        std::cerr << "usage: tunedvb [-s<service name>] <dvb config file> " << std::endl;
        return 1;
    }
    if (argc == 3) {
        serviceName = std::string(argv[1]).substr(2);
        dvbXml.open(argv[2]);
    }
    else {
        dvbXml.open(argv[1]);
    }

    pDevice->readXml(dvbXml);
    pDevice->open();

    if (serviceName.length()) {
        recordService(serviceName);
    }
    else {
        for (Omm::Dvb::Device::ServiceIterator it = pDevice->serviceBegin(); it != pDevice->serviceEnd(); ++it) {
            recordService(it->first);
        }
    }

    pDevice->close();

    return 0;
}
