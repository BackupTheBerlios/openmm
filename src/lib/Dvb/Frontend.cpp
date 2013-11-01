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

#include <sstream>

#include <linux/dvb/frontend.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <Poco/NumberParser.h>
#include <Poco/DOM/AbstractContainerNode.h>
#include <Poco/DOM/DOMException.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AttrMap.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Attr.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <Poco/Zip/ZipArchive.h>
#include <Poco/Zip/ZipStream.h>
#include <Poco/StringTokenizer.h>

#include "DvbLogger.h"
#include "Descriptor.h"
#include "Section.h"
#include "Stream.h"
#include "Service.h"
#include "TransponderData.h"
#include "Transponder.h"
#include "Demux.h"
#include "Dvr.h"
#include "Frontend.h"
#include "Device.h"
#include "Dvb/Transponder.h"
#include "Dvb/Service.h"
#include "Dvb/Section.h"
#include "Dvb/Remux.h"


namespace Omm {
namespace Dvb {

class SignalCheckThread : public Poco::Runnable
{
public:
    SignalCheckThread(Frontend* pFrontend);

    void run();
    void stop();

private:
    Frontend*        _pFrontend;
    bool                _stop;
};


SignalCheckThread::SignalCheckThread(Frontend* pFrontend) :
_pFrontend(pFrontend),
_stop(false)
{
}


void
SignalCheckThread::run()
{
//     Poco::ScopedLock<Poco::FastMutex> lock(DvbDevice::instance()->_tuneLock);
    do {
        _pFrontend->checkFrontend();
        Poco::Thread::sleep(1000);
    } while(!_stop);
}


void
SignalCheckThread::stop()
{
    _stop = true;
}


const std::string Frontend::Unknown("unknown");
const std::string Frontend::DVBS("dvb-s");
const std::string Frontend::DVBT("dvb-t");
const std::string Frontend::DVBC("dvb-c");
const std::string Frontend::ATSC("atsc");

Frontend::Frontend(Adapter* pAdapter, int num) :
_fileDescFrontend(-1),
_pAdapter(pAdapter),
_num(num),
_frontendTimeout(2000000),
_pTunedTransponder(0)
{
    _deviceName = _pAdapter->_deviceName + "/frontend" + Poco::NumberFormatter::format(_num);
    _pDemux = new Demux(pAdapter, 0);
    _pDvr = new Dvr(pAdapter, 0);
    _pTextConverter = new Poco::TextConverter(_sourceEncoding, _targetEncoding);
}


Frontend::~Frontend()
{
    closeFrontend();
    delete _pDemux;
    delete _pDvr;
}


Frontend*
Frontend::detectFrontend(Adapter* pAdapter, int num)
{
    std::string deviceName = pAdapter->_deviceName + "/frontend" + Poco::NumberFormatter::format(num);
    LOG(dvb, information, "detect frontend " + deviceName + " ...");

    LOG(dvb, debug, "open frontend");
    int fileDescFrontend;
    if ((fileDescFrontend = open(deviceName.c_str(), O_RDONLY | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "open frontend failed: " + std::string(strerror(errno)));
        return 0;
    }

    struct dvb_frontend_info feInfo;
    int result = ioctl(fileDescFrontend, FE_GET_INFO, &feInfo);

    if (result < 0) {
        LOG(dvb, error, "ioctl FE_GET_INFO failed");
        if (close(fileDescFrontend)) {
            LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
        }
        return 0;
    }

    Frontend* pFrontend = 0;
    switch (feInfo.type) {
        case FE_QPSK:
            pFrontend = new SatFrontend(pAdapter, num);
            break;
        case FE_OFDM:
            pFrontend = new TerrestrialFrontend(pAdapter, num);
            break;
        case FE_QAM:
            pFrontend = new CableFrontend(pAdapter, num);
            break;
        case FE_ATSC:
            pFrontend = new AtscFrontend(pAdapter, num);
            break;
    }
    pFrontend->_name = std::string(feInfo.name);

    LOG(dvb, debug, "close frontend");
    if (close(fileDescFrontend)) {
        LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
    }
    if (pFrontend) {
        LOG(dvb, information, "found frontend " + pFrontend->_name + " (" + pFrontend->_type + ")");
    }
    else {
        LOG(dvb, error, "could not detect frontend " + deviceName);
    }

    return pFrontend;
}


void
Frontend::addTransponder(Transponder* pTransponder)
{
    LOG(dvb, debug, "add transponder with tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId));
    _transponders.push_back(pTransponder);
}


void
Frontend::openFrontend()
{
    LOG(dvb, debug, "open frontend");

    if ((_fileDescFrontend = open(_deviceName.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "open frontend failed: " + std::string(strerror(errno)));
        return;
    }

    int result = ioctl(_fileDescFrontend, FE_GET_INFO, &_feInfo);

    if (result < 0) {
        LOG(dvb, error, "ioctl FE_GET_INFO failed");
        closeFrontend();
    }

    switch (_feInfo.type) {
        case FE_QPSK:
            _type = DVBS;
            break;
        case FE_OFDM:
            _type = DVBT;
            break;
        case FE_QAM:
            _type = DVBC;
            break;
        case FE_ATSC:
            _type = ATSC;
            break;
        default:
            _type = Unknown;
    }

    if (!typeSupported()) {
        LOG(dvb, error, "frontend device is not a DVB-S or DVB-T device, not yet supported");
        closeFrontend();
    }
    else if (_transponders.size()) {  // no transponders, no dvr needed
        _pDvr->openDvr();
    }
}


void
Frontend::closeFrontend()
{
    LOG(dvb, debug, "close frontend");

    if (_transponders.size()) {
        _pDvr->closeDvr();
    }

//    dvb_frontend_event event;
//    while (!ioctl(_fileDescFrontend, FE_GET_EVENT, &event)) {
//    }
    if (close(_fileDescFrontend)) {
        LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
    }
}


void
Frontend::scan(const std::string& initialTransponderData)
{
    // TODO: clear transponder lists?
    getInitialTransponderData(initialTransponderData);
    LOG(dvb, debug, "number of initial transponders in " + initialTransponderData + ": " + Poco::NumberFormatter::format(_initialTransponders.size()));
    for (std::vector<Transponder*>::iterator it = _initialTransponders.begin(); it != _initialTransponders.end(); ++it) {
        LOG(dvb, trace, "initial transponder (freq: " + Poco::NumberFormatter::format((*it)->_frequency) + ", tsid: " + Poco::NumberFormatter::format((*it)->_transportStreamId) + ")");
        if (tune(*it)) {
            scanTransponder(*it);
            if (addKnownTransponder(*it)) {
                addTransponder(*it);
            }
            else {
                LOG(dvb, error, "initial transponder double");
            }
        }
    }
    closeFrontend();
}


void
Frontend::readXml(Poco::XML::Node* pXmlFrontend)
{
    LOG(dvb, debug, "read frontend ...");

    _name = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("name");
    _type = static_cast<Poco::XML::Element*>(pXmlFrontend)->getAttribute("type");

    if (pXmlFrontend->hasChildNodes()) {
        Poco::XML::Node* pXmlTransponder = pXmlFrontend->firstChild();
        while (pXmlTransponder && pXmlTransponder->nodeName() == "transponder") {
            unsigned int freq = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlTransponder)->getAttribute("frequency"));
            unsigned int tid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlTransponder)->getAttribute("tsid"));
            LOG(dvb, debug, "new transponder with freq: " + Poco::NumberFormatter::format(freq));

            Transponder* pTransponder;
            if (_type == DVBS) {
                pTransponder = new SatTransponder(this, freq, tid);
            }
            else if (_type == DVBT) {
                pTransponder = new TerrestrialTransponder(this, freq, tid);
            }
            else if (_type == DVBC) {
                pTransponder = new CableTransponder(this, freq, tid);
            }
            else if (_type == ATSC) {
                pTransponder = new AtscTransponder(this, freq, tid);
            }
            else {
                LOG(dvb, error, "dvb frontend type unknown, cannot create transponders");
                return;
            }

            pTransponder->readXml(pXmlTransponder);
            addTransponder(pTransponder);
            pXmlTransponder = pXmlTransponder->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb description contains no transponders");
        return;
    }

    LOG(dvb, debug, "read frontend.");
}


void
Frontend::writeXml(Poco::XML::Element* pAdapter)
{
    LOG(dvb, debug, "write frontend ...");

    Poco::XML::Document* pDoc = pAdapter->ownerDocument();
    _pXmlFrontend = pDoc->createElement("frontend");
    _pXmlFrontend->setAttribute("type", getType());
    _pXmlFrontend->setAttribute("name", getName());
    pAdapter->appendChild(_pXmlFrontend);
    for (std::vector<Transponder*>::iterator it = _transponders.begin(); it != _transponders.end(); ++it) {
        (*it)->writeXml(_pXmlFrontend);
    }

    LOG(dvb, debug, "wrote frontend.");
}


const std::string
Frontend::getType()
{
//    LOG(dvb, debug, "frontend type: " + Poco::NumberFormatter::format(_feInfo.type));

    return _type;

    // FIXME: dvb-s and dvb-t seam to be swapped on my systems ...?
//    switch (_feInfo.type) {
//        case FE_QPSK:
////            return DVBS;
//            return DVBT;
//        case FE_OFDM:
//            return DVBS;
////            return DVBT;
//        case FE_QAM:
//            return DVBC;
//        case FE_ATSC:
//            return ATSC;
//        default:
//            return "";
//    }
}


const std::string
Frontend::getName()
{
    return _name;
//    return _feInfo.name;
}


bool
Frontend::typeSupported()
{
    return getType() == DVBS || getType() == DVBT;
}


bool
Frontend::isTuned()
{
    return _pTunedTransponder != 0;
}


bool
Frontend::isTunedTo(Transponder* pTransponder)
{
    return _pTunedTransponder == pTransponder;
}


void
Frontend::listInitialTransponderData()
{
    LOG(dvb, debug, "compressed transponder data size: " + Poco::NumberFormatter::format(TransponderData::instance()->getResource("transponder.zip").size()));
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"));
    Poco::Zip::ZipArchive arch(ss);
    for (Poco::Zip::ZipArchive::FileInfos::const_iterator it = arch.fileInfoBegin(); it != arch.fileInfoEnd(); ++it) {
        LOG(dvb, debug, it->second.getFileName());
    }
}


void
Frontend::getInitialTransponderKeys(std::vector<std::string>& keys)
{
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"), std::ios::binary);
    Poco::Zip::ZipArchive arch(ss);

    for (Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.headerBegin(); it != arch.headerEnd(); ++it) {
        Poco::StringTokenizer frontendKey(it->first, "/");
        if (frontendKey[0] == getType() && frontendKey.count() > 1) {
            keys.push_back(frontendKey[1]);
        }
    }
}


void
Frontend::getInitialTransponderData(const std::string& key)
{
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"), std::ios::binary);
    Poco::Zip::ZipArchive arch(ss);
    Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.findHeader(getType() + "/" + key);
    if (it == arch.headerEnd()) {
        LOG(dvb, error, "transponder data not found for: " + getType() + "/" + key);
        return;
    }
    Poco::Zip::ZipInputStream zipin(ss, it->second);
    std::string line;
    while (getline(zipin, line)) {
        std::string::size_type pos = line.find('#');
        line = line.substr(0, pos);
        if (line.length() == 0) {
            continue;
        }
        LOG(dvb, debug, line);

        Poco::StringTokenizer params(line, " ");
        unsigned int freq = 0;
        try {
            freq = Poco::NumberParser::parse(params[1]);
        }
        catch (Poco::Exception& e) {
            LOG(dvb, error, "transponder data without frequency: " + line);
            continue;
        }
        Transponder* pTransponder = createTransponder(freq, Transponder::InvalidTransportStreamId);
        if (pTransponder->initTransponder(params)) {
            _initialTransponders.push_back(pTransponder);
        }
        else {
            LOG(dvb, error, "transponder initialization failed: " + line);
        }
    }
}


void
Frontend::checkFrontend()
{
    fe_status_t status;
    uint16_t snr, signal;
    uint32_t ber, uncorrected_blocks;
//     int timeout = 0;

    if (ioctl(_fileDescFrontend, FE_READ_STATUS, &status) == -1) {
        LOG(dvb, error, "FE_READ_STATUS failed");
    }
    /* some frontends might not support all these ioctls, thus we
    * avoid printing errors */
    if (ioctl(_fileDescFrontend, FE_READ_SIGNAL_STRENGTH, &signal) == -1) {
        signal = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_SNR, &snr) == -1) {
        snr = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_BER, &ber) == -1) {
        ber = -2;
    }
    if (ioctl(_fileDescFrontend, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks) == -1) {
        uncorrected_blocks = -2;
    }

    /*
    if (human_readable) {
        printf ("status %02x | signal %3u%% | snr %3u%% | ber %d | unc %d | ",
                status, (signal * 100) / 0xffff, (snr * 100) / 0xffff, ber, uncorrected_blocks);
    } else {
        printf ("status %02x | signal %04x | snr %04x | ber %08x | unc %08x | ",
                status, signal, snr, ber, uncorrected_blocks);
    }
    */

    if (status & FE_HAS_LOCK) {
        LOG(dvb, debug, "FE_HAS_LOCK");
    }
}


bool
Frontend::addKnownTransponder(Transponder* pTransponder)
{
    LOG(dvb, trace, "addKnownTransponder");
    for (std::vector<Transponder*>::iterator it = _scannedTransponders.begin(); it != _scannedTransponders.end(); ++it) {
        if ((*it)->equal(pTransponder)) {
            LOG(dvb, trace, "known transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
            return false;
        }
    }
    LOG(dvb, trace, "new transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
    _scannedTransponders.push_back(pTransponder);
    return true;
}


bool
Frontend::waitForLock(Poco::Timestamp::TimeDiff timeout)
{
    LOG(dvb, debug, "frontend wait for lock ...");
    Poco::Timestamp now;
    while (!timeout || (!hasLock() && now.elapsed() < timeout)) {
        Poco::Thread::sleep(10);
    }
    if (hasLock()) {
        LOG(dvb, debug, "frontend has lock.");
        return true;
    }
    else {
        LOG(dvb, error, "frontend timeout.");
        return false;
    }
}


bool
Frontend::hasLock()
{
    fe_status_t status;
    if (!ioctl(_fileDescFrontend, FE_READ_STATUS, &status) && (status & FE_HAS_LOCK)) {
        return true;
    }
    else {
        return false;
    }
}


bool
Frontend::scanTransponder(Transponder* pTransponder)
{
    LOG(dvb, trace, "************** Transponder **************");
    if (scanPatPmt(pTransponder)) {
        scanSdt(pTransponder);
        // actual NIT
        scanNit(pTransponder, true);
        // other NIT
    //    scanNit(pTransponder);
        return true;
    }
    else {
        return false;
    }
}


bool
Frontend::scanPatPmt(Transponder* pTransponder)
{
    LOG(dvb, trace, "--------------     PAT     --------------");
    PatSection pat;
    Table patTab(pat);
    if (_pDemux->readTable(&patTab)) {
        for (int sPat = 0; sPat < patTab.sectionCount(); sPat++) {
            PatSection* pPat = static_cast<PatSection*>(patTab.getSection(sPat));
            LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(pPat->transportStreamId()));
            if (pTransponder->_transportStreamId == Transponder::InvalidTransportStreamId) {
                pTransponder->_transportStreamId = pPat->transportStreamId();
            }
            else if (pTransponder->_transportStreamId != pPat->transportStreamId()) {
                LOG(dvb, error, "transport stream id mismatch: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + " != " + Poco::NumberFormatter::format(pPat->transportStreamId()));
                return false;
            }
            LOG(dvb, trace, "service count: " + Poco::NumberFormatter::format(pPat->serviceCount()));
            for (int serviceIndex = 0; serviceIndex < pPat->serviceCount(); serviceIndex++) {
                LOG(dvb, trace, "--------------     PMT     --------------");
                LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pPat->serviceId(serviceIndex)) +
                              ", pmt pid: " + Poco::NumberFormatter::format(pPat->pmtPid(serviceIndex)));
                if (pPat->serviceId(serviceIndex)) { // no NIT service
                    Service* pService = new Dvb::Service(pTransponder, "", pPat->serviceId(serviceIndex), pPat->pmtPid(serviceIndex));
                    pTransponder->addService(pService);
                    PmtSection pmt(pPat->pmtPid(serviceIndex));
                    Table pmtTab(pmt);
                    if (pPat->serviceId(serviceIndex) && _pDemux->readTable(&pmtTab)) {
                        for (int sPmt = 0; sPmt < pmtTab.sectionCount(); sPmt++) {
                            PmtSection* pPmt = static_cast<PmtSection*>(pmtTab.getSection(sPmt));
                            for (int streamIndex = 0; streamIndex < pPmt->streamCount(); streamIndex++) {
                                LOG(dvb, trace, "stream pid: " + Poco::NumberFormatter::format(pPmt->streamPid(streamIndex)) +
                                            ", type: " + Stream::streamTypeToString(pPmt->streamType(streamIndex)));
                                pService->addStream(new Stream(Stream::streamTypeToString(pPmt->streamType(streamIndex)), pPmt->streamPid(streamIndex)));
                            }
                            pService->addStream(new Stream(Stream::ProgramMapTable, pPmt->packetId()));
                            pService->_pcrPid = pPmt->pcrPid();
        //                    pService->addStream(new Stream(Stream::ProgramClock, pPmt->pcrPid()));
                        }
                    }
                }
            }
            return true;
        }
    }
    else {
        return false;
    }
}


void
Frontend::scanSdt(Transponder* pTransponder)
{
    LOG(dvb, trace, "--------------     SDT     --------------");
    SdtSection sdtSection;
    Table sdtTab(sdtSection);
    if (_pDemux->readTable(&sdtTab)) {
        for (int s = 0; s < sdtTab.sectionCount(); s++) {
            SdtSection* pS = static_cast<SdtSection*>(sdtTab.getSection(s));
            for (int serviceIndex = 0; serviceIndex < pS->serviceCount(); serviceIndex++) {
                LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pS->serviceId(serviceIndex)) +
                            ", running status: " + pS->runningStatus(serviceIndex) +
                            ", scrambled: " + Poco::NumberFormatter::format(pS->scrambled(serviceIndex)));
                Service* pService = pTransponder->getService(pS->serviceId(serviceIndex));
                if (pService) {
                    pService->_status = pS->runningStatus(serviceIndex);
                    pService->_scrambled = pS->scrambled(serviceIndex);
                    for (unsigned int d = 0; d < pS->serviceDescriptorCount(serviceIndex); d++) {
                        Descriptor* pDescriptor = pS->serviceDescriptor(serviceIndex, d);
                        if (ServiceDescriptor* pD = dynamic_cast<ServiceDescriptor*>(pDescriptor)) {
                            pService->_type = Service::typeToString(pD->serviceType());
                            pService->_providerName = pD->providerName();
                            pService->_name = pD->serviceName();
                            LOG(dvb, trace, "service name: " + pService->_name);
                        }
                    }
                    Poco::NotificationCenter::defaultCenter().postNotification(new ScanNotification(pService));
                }
            }
        }
    }
}


void
Frontend::scanNit(Transponder* pTransponder, bool actual)
{
    std::vector<Transponder*> additionalTransponders;
    LOG(dvb, trace, "--------------     NIT (" + std::string(actual ? "actual" : "other") + ")    --------------");
    NitSection nitSection(actual ? NitSection::NitActualTableId : NitSection::NitOtherTableId);
    Table nitTab(nitSection);
    if (_pDemux->readTable(&nitTab)) {
        NitSection* pNit = static_cast<NitSection*>(nitTab.getFirstSection());
        LOG(dvb, trace, "network id: " + Poco::NumberFormatter::format(pNit->networkId()) + ", name: " + pNit->networkName());
//        LOG(dvb, trace, "network descriptor count: " + Poco::NumberFormatter::format(nit.networkDescriptorCount()));
        for (int s = 0; s < nitTab.sectionCount(); s++) {
            NitSection* pS = static_cast<NitSection*>(nitTab.getSection(s));
            for (unsigned int t = 0; t < pS->transportStreamCount(); t++) {
                LOG(dvb, trace, "original network id: " + Poco::NumberFormatter::format(pS->originalNetworkId(t)) +
                            ", transport stream id: " + Poco::NumberFormatter::format(pS->transportStreamId(t)));

                for (unsigned int d = 0; d < pS->transportStreamDescriptorCount(t); d++) {
                    Descriptor* pDescriptor = pS->transportStreamDescriptor(t, d);
                    if (ServiceListDescriptor* pD = dynamic_cast<ServiceListDescriptor*>(pDescriptor)) {
    //                    LOG(dvb, trace, "service count: " + Poco::NumberFormatter::format(pD->serviceCount()));
                        for (int i = 0; i < pD->serviceCount(); i++) {
    //                        LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pD->serviceId(i)));
                        }
                    }
                    else if (SatelliteDeliverySystemDescriptor* pD = dynamic_cast<SatelliteDeliverySystemDescriptor*>(pDescriptor)) {
                        LOG(dvb, trace, "orbital position: " + pD->orbitalPosition() +
                                    ", frequency[kHz]: " + Poco::NumberFormatter::format(pD->frequency()) +
                                    ", polarization: " + pD->polarization() +
                                    ", symbol rate: " + Poco::NumberFormatter::format(pD->symbolRate()));
                        SatTransponder* pT = new SatTransponder(this, pD->frequency(), pS->transportStreamId(t));
                        pT->init(pD->orbitalPosition(), SatFrontend::InvalidSatNum, pD->symbolRate(), pD->polarization());
                        additionalTransponders.push_back(pT);
                    }
                    else if (TerrestrialDeliverySystemDescriptor* pD = dynamic_cast<TerrestrialDeliverySystemDescriptor*>(pDescriptor)) {
                        LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency()));
                        TerrestrialTransponder* pT = new TerrestrialTransponder(this, pD->centreFrequency(), pS->transportStreamId(t));
                        pT->init(TerrestrialTransponder::bandwidthFromString(pD->bandwidth()),
                                TerrestrialTransponder::coderateFromString(pD->codeRateHpStream()),
                                TerrestrialTransponder::coderateFromString(pD->codeRateLpStream()),
                                TerrestrialTransponder::modulationFromString(pD->constellation()),
                                TerrestrialTransponder::transmitModeFromString(pD->transmissionMode()),
                                TerrestrialTransponder::guard_intervalFromString(pD->guardInterval()),
                                TerrestrialTransponder::hierarchyFromString(pD->hierarchyInformation())
                        );
                        additionalTransponders.push_back(pT);
                    }
//                    else if (FrequencyListDescriptor* pD = dynamic_cast<FrequencyListDescriptor*>(pDescriptor)) {
//                        for (int i = 0; i < pD->centreFrequencyCount(); i++) {
//                            LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency(i)));
//                        }
//                    }
                }
            }
        }
    }
    for (std::vector<Transponder*>::iterator it = additionalTransponders.begin(); it != additionalTransponders.end(); ++it) {
        if (addKnownTransponder(*it) && tune(*it) && scanTransponder(*it)) {
            addTransponder(*it);
        }
        else {
            delete *it;
        }
    }
}


Lnb::Lnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal) :
_desc(desc),
_lowVal(lowVal),
_highVal(highVal),
_switchVal(switchVal)
{
}


bool
Lnb::isHiBand(unsigned int freq, unsigned int& ifreq)
{
    bool hiBand = false;

    if (_switchVal && _highVal && freq >= _switchVal) {
        hiBand = true;
    }

    if (hiBand)
        ifreq = freq - _highVal;
    else {
        if (freq < _lowVal)
            ifreq = _lowVal - freq;
        else
            ifreq = freq - _lowVal;
    }

    return hiBand;
}


const int SatFrontend::InvalidSatNum(-1);
const int SatFrontend::maxSatNum(4);

SatFrontend::SatFrontend(Adapter* pAdapter, int num) :
Frontend(pAdapter, num)
{
    _type = DVBS;
    // FIXME: when do we need to multiply freqs with 1000?
    _lnbs["UNIVERSAL"] = new Lnb("Europe | 10800 to 11800 MHz and 11600 to 12700 Mhz | Dual LO, loband 9750, hiband 10600 MHz",
                                    9750000, 10600000, 11700000);
    _lnbs["DBS"] = new Lnb("Expressvu, North America | 12200 to 12700 MHz | Single LO, 11250 MHz",
                              11250, 0, 0);
    _lnbs["STANDARD"] = new Lnb("10945 to 11450 Mhz | Single LO, 10000 Mhz",
                                   10000, 0, 0);
    _lnbs["ENHANCED"] = new Lnb("Astra | 10700 to 11700 MHz | Single LO, 9750 MHz",
                                   9750, 0, 0);
    _lnbs["C-BAND"] = new Lnb("Big Dish | 3700 to 4200 MHz | Single LO, 5150 Mhz",
                              5150, 0, 0);
    _pLnb = _lnbs["UNIVERSAL"];
}


bool
SatFrontend::tune(Transponder* pTransponder)
{
    SatTransponder* pTrans = dynamic_cast<SatTransponder*>(pTransponder);
    if (!pTrans) {
        LOG(dvb, error, "sat frontend cannot tune to non-sat transponder");
        return false;
    }
    LOG(dvb, debug, "sat frontend tuning to transponder with frequency (sat pos/no): " + Poco::NumberFormatter::format(pTransponder->_frequency)
            + " (" + pTrans->_satPosition + "/" + Poco::NumberFormatter::format(pTrans->_satNum) + ")"
            + " ...");

    int firstSat = 0;
    int lastSat = maxSatNum - 1;
    if (pTrans->_satNum != InvalidSatNum) {
        firstSat = pTrans->_satNum;
        lastSat = pTrans->_satNum;
    }
    bool success = false;
    for (int i = firstSat; i <= lastSat; i++) {
        if (pTrans->_satNum == InvalidSatNum) {
            if (pTrans->_satPosition != "" && getSatNum(pTrans->_satPosition) == InvalidSatNum && isSatNumKnown(i)) {
                // transponder has orbital sat position but satellite is not known, yet, so don't tune to already known satellites
                // (they have different orbital positions than the new position of this transponder)
                LOG(dvb, debug, "sat frontend skipping satellite number (orbital position " + pTrans->_satPosition + " of transponder known to be wrong sat number): " + Poco::NumberFormatter::format(i));
                continue;
            }
            else {
                LOG(dvb, debug, "sat frontend probing satellite number: " + Poco::NumberFormatter::format(i));
            }
        }
        unsigned int ifreq;
        bool hiBand = _pLnb->isHiBand(pTrans->_frequency, ifreq);
        diseqc(i, pTrans->_polarization, hiBand);

        struct dvb_frontend_parameters tuneto;
        tuneto.frequency = ifreq;
        tuneto.inversion = INVERSION_AUTO;
        tuneto.u.qpsk.symbol_rate = pTrans->_symbolRate;
        tuneto.u.qpsk.fec_inner = FEC_AUTO;

        if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
            LOG(dvb, debug, "sat frontend tuning failed.");
            continue;
        }
        success = waitForLock(_frontendTimeout);
        if (success) {
            _pTunedTransponder = pTrans;
            pTrans->_satNum = i;
            if (pTrans->_satPosition != "") {
                setSatNum(pTrans->_satPosition, pTrans->_satNum);
            }
            break;
        }
    }
    return success;
}


Transponder*
SatFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new SatTransponder(this, freq, tsid);
}


void
SatFrontend::readXml(Poco::XML::Node* pXmlFrontend)
{
    Frontend::readXml(pXmlFrontend);

    if (pXmlFrontend->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlFrontend->firstChild();
        while (pXmlParam) {
            if (pXmlParam->nodeName() == "transponder") {
                // do nothing
            }
            else if (pXmlParam->nodeName() == "satellite") {
                Poco::XML::Node* pXmlSatParam = pXmlParam->firstChild();
                std::string satPos;
                int satNum = InvalidSatNum;
                while (pXmlSatParam) {
                    Poco::XML::Node* pXmlSatParamVal = pXmlSatParam->firstChild();
                    if (!pXmlSatParamVal) {
                        LOG(dvb, error, "sat frontend satellite parameter without value: " + pXmlParam->nodeName());
                        continue;
                    }
                    if (pXmlSatParam->nodeName() == "orbitalPosition") {
                        satPos = pXmlSatParamVal->innerText();
                    }
                    else if (pXmlSatParam->nodeName() == "index") {
                        satNum = Poco::NumberParser::parse(pXmlSatParamVal->innerText());
                    }
                    pXmlSatParam = pXmlSatParam->nextSibling();
                }
                setSatNum(satPos, satNum);
            }
            else {
                LOG(dvb, error, "sat frontend unknown parameter: " + pXmlParam->nodeName());
            }

            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "sat frontend description contains no satellites");
        return;
    }
}


void
SatFrontend::writeXml(Poco::XML::Element* pAdapter)
{
    Frontend::writeXml(pAdapter);

    Poco::XML::Document* pDoc = pAdapter->ownerDocument();

    for (std::map<std::string, int>::iterator it = _satNumMap.begin(); it != _satNumMap.end(); ++it) {
        Poco::AutoPtr<Poco::XML::Element> pSat = pDoc->createElement("satellite");
        Poco::AutoPtr<Poco::XML::Element> pSatPos = pDoc->createElement("orbitalPosition");
        Poco::AutoPtr<Poco::XML::Text> pSatPosVal = pDoc->createTextNode(it->first);
        pSatPos->appendChild(pSatPosVal);
        pSat->appendChild(pSatPos);
        Poco::AutoPtr<Poco::XML::Element> pSatNum = pDoc->createElement("index");
        Poco::AutoPtr<Poco::XML::Text> pSatNumVal = pDoc->createTextNode(Poco::NumberFormatter::format(it->second));
        pSatNum->appendChild(pSatNumVal);
        pSat->appendChild(pSatNum);
        _pXmlFrontend->appendChild(pSat);
    }
}


int
SatFrontend::getSatNum(const std::string& orbitalPosition)
{
    std::map<std::string, int>::iterator pos = _satNumMap.find(orbitalPosition);
    if (pos != _satNumMap.end()) {
        return pos->second;
    }
    else {
        return InvalidSatNum;
    }
}


void
SatFrontend::setSatNum(const std::string& orbitalPosition, int satNum)
{
    std::map<std::string, int>::iterator pos = _satNumMap.find(orbitalPosition);
    if (pos != _satNumMap.end()) {
        if (pos->second != satNum) {
            LOG(dvb, warning, "sat position already mapped (overwriting): " + orbitalPosition + " -> " + Poco::NumberFormatter::format(satNum) +
                    " (was: " + Poco::NumberFormatter::format(pos->second) + ")");
        }
        else {
            return;
        }
    }
    _satNumMap[orbitalPosition] = satNum;
    printSatMap();
}


bool
SatFrontend::isSatNumKnown(int satNum)
{
    bool res = false;
    for (std::map<std::string, int>::iterator it = _satNumMap.begin(); it != _satNumMap.end(); ++it) {
        if (it->second == satNum) {
            res = true;
        }
    }
    return res;
}


void
SatFrontend::diseqc(unsigned int satNum, const std::string& polarization, bool hiBand)
{
    LOG(dvb, debug, "diseqc command on sat: " + Poco::NumberFormatter::format(satNum) + " ...");

    struct diseqc_cmd {
        struct dvb_diseqc_master_cmd cmd;
        uint32_t wait;
    } cmd = { {{0xe0, 0x10, 0x38, 0xf0, 0x00, 0x00}, 4}, 0 };

    // param: high nibble: reset bits, low nibble set bits,
    // bits are: option, position, polarization, band

    cmd.cmd.msg[3] =
        0xf0 | (((satNum * 4) & 0x0f) | (hiBand ? 1 : 0) | ((polarization == SatTransponder::POL_VERT) ? 0 : 2));

    fe_sec_voltage_t voltage = (polarization == SatTransponder::POL_VERT) ? SEC_VOLTAGE_13 : SEC_VOLTAGE_18;
    fe_sec_tone_mode_t tone = hiBand ? SEC_TONE_ON : SEC_TONE_OFF;
    fe_sec_mini_cmd_t burst = satNum % 2 ? SEC_MINI_B : SEC_MINI_A;

    if (ioctl(_fileDescFrontend, FE_SET_TONE, SEC_TONE_OFF) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }
    if (ioctl(_fileDescFrontend, FE_SET_VOLTAGE, voltage) == -1) {
        LOG(dvb, error, "FE_SET_VOLTAGE failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_MASTER_CMD, &cmd.cmd) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_MASTER_CMD failed");
    }
    usleep(cmd.wait * 1000);
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_DISEQC_SEND_BURST, burst) == -1) {
        LOG(dvb, error, "FE_DISEQC_SEND_BURST failed");
    }
    usleep(15 * 1000);
    if (ioctl(_fileDescFrontend, FE_SET_TONE, tone) == -1) {
        LOG(dvb, error, "FE_SET_TONE failed");
    }

    LOG(dvb, debug, "diseqc command finished.");
}


void
SatFrontend::printSatMap()
{
    for (std::map<std::string, int>::iterator it = _satNumMap.begin(); it != _satNumMap.end(); ++it) {
        LOG(dvb, trace, "sat map entry (pos/num): " + it->first + "/" + Poco::NumberFormatter::format(it->second));
    }
}


bool
TerrestrialFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "terrestrial frontend tuning to transponder with frequency: " + Poco::NumberFormatter::format(pTransponder->_frequency) + " ...");

    TerrestrialTransponder* pTrans = dynamic_cast<TerrestrialTransponder*>(pTransponder);
    if (!pTrans) {
        LOG(dvb, error, "terrestrial frontend cannot tune to non-terrestrial transponder");
        return false;
    }

    LOG(dvb, trace, "terrestrial transponder parameters: " +
            Poco::NumberFormatter::format(pTrans->_bandwidth) + ", " +
            Poco::NumberFormatter::format(pTrans->_code_rate_HP) + ", " +
            Poco::NumberFormatter::format(pTrans->_code_rate_LP) + ", " +
            Poco::NumberFormatter::format(pTrans->_constellation) + ", " +
            Poco::NumberFormatter::format(pTrans->_transmission_mode) + ", " +
            Poco::NumberFormatter::format(pTrans->_guard_interval) + ", " +
            Poco::NumberFormatter::format(pTrans->_hierarchy_information));

    struct dvb_frontend_parameters tuneto;

    tuneto.frequency = pTrans->_frequency;
    tuneto.inversion = INVERSION_AUTO;
    tuneto.u.ofdm.bandwidth = pTrans->_bandwidth;
    tuneto.u.ofdm.code_rate_HP = pTrans->_code_rate_HP;
    tuneto.u.ofdm.code_rate_LP = pTrans->_code_rate_LP;
    tuneto.u.ofdm.constellation = pTrans->_constellation;
    tuneto.u.ofdm.transmission_mode = pTrans->_transmission_mode;
    tuneto.u.ofdm.guard_interval = pTrans->_guard_interval;
    tuneto.u.ofdm.hierarchy_information = pTrans->_hierarchy_information;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "terrestrial frontend tuning failed.");
        return false;
    }
    bool success = waitForLock(_frontendTimeout);
    if (success) {
        _pTunedTransponder = pTrans;
    }
    return success;
}


Transponder*
TerrestrialFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new TerrestrialTransponder(this, freq, tsid);
}


bool
CableFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "cable frontend tuning to transponder: " + Poco::NumberFormatter::format(pTransponder->_frequency) + " ...");

    CableTransponder* pTrans = static_cast<CableTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "cable frontend tuning failed.");
        return false;
    }
    bool success = waitForLock(_frontendTimeout);
    if (success) {
        _pTunedTransponder = pTrans;
    }
    return success;
}


Transponder*
CableFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new CableTransponder(this, freq, tsid);
}


bool
AtscFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "atsc frontend tuning to transponder: " + Poco::NumberFormatter::format(pTransponder->_frequency) + " ...");

    AtscTransponder* pTrans = static_cast<AtscTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "atsc frontend tuning failed.");
        return false;
    }
    bool success = waitForLock(_frontendTimeout);
    if (success) {
        _pTunedTransponder = pTrans;
    }
    return success;
}


Transponder*
AtscFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new AtscTransponder(this, freq, tsid);
}


}  // namespace Omm
}  // namespace Dvb
