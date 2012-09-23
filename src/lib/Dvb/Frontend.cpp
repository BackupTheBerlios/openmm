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

#include <Poco/String.h>
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

#include "DvbLogger.h"
#include "Descriptor.h"
#include "Section.h"
#include "Stream.h"
#include "TransponderData.h"
#include "Transponder.h"
#include "Demux.h"
#include "Dvr.h"
#include "Frontend.h"
#include "Device.h"


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
const std::string Frontend::DVBS("DVB-S");
const std::string Frontend::DVBT("DVB-T");
const std::string Frontend::DVBC("DVB-C");
const std::string Frontend::ATSC("ATSC");

Frontend::Frontend(Adapter* pAdapter, int num) :
_fileDescFrontend(-1),
_pAdapter(pAdapter),
_num(num),
//_frontendTimeout(2000000)
_frontendTimeout(1000000)
{
    _deviceName = _pAdapter->_deviceName + "/frontend" + Poco::NumberFormatter::format(_num);
    _pDemux = new Demux(pAdapter, 0);
    if (Device::instance()->useDvrDevice()) {
        _pDvr = new Dvr(pAdapter, 0);
    }
    _pTextConverter = new Poco::TextConverter(_sourceEncoding, _targetEncoding);
}


Frontend::~Frontend()
{
    closeFrontend();
    delete _pDemux;
    if (Device::instance()->useDvrDevice()) {
        delete _pDvr;
    }
}


void
Frontend::addTransponder(Transponder* pTransponder)
{
    _transponders.push_back(pTransponder);
}


void
Frontend::openFrontend()
{
    LOG(dvb, debug, "opening frontend");

    if ((_fileDescFrontend = open(_deviceName.c_str(), O_RDWR | O_NONBLOCK)) < 0) {
        LOG(dvb, error, "opening frontend failed: " + std::string(strerror(errno)));
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

//    _pDemux->openDemux(false);
    if (Device::instance()->useDvrDevice()) {
        _pDvr->openDvr(Device::instance()->blockDvrDevice());
    }
}


void
Frontend::closeFrontend()
{
    LOG(dvb, debug, "closing frontend");

    if (Device::instance()->useDvrDevice()) {
        _pDvr->closeDvr();
    }
//    _pDemux->closeDemux();

    if (close(_fileDescFrontend)) {
        LOG(dvb, error, "failed to close frontend: " + std::string(strerror(errno)));
    }
}


void
Frontend::scan(const std::string& initialTransponderData)
{
    // TODO: clear transponder lists

    getInitialTransponderData(initialTransponderData);
    LOG(dvb, debug, "number of initial transponders: " + Poco::NumberFormatter::format(_initialTransponders.size()));
    openFrontend();
//    std::vector<Transponder*>::iterator it = _initialTransponders.begin() + 2;
//    SatTransponder* pT = new SatTransponder(this, 11856000, 1072);
//    pT->init(0, 27500000, "V");
    for (std::vector<Transponder*>::iterator it = _initialTransponders.begin(); it != _initialTransponders.end(); ++it) {
        LOG(dvb, trace, "initial transponder (freq: " + Poco::NumberFormatter::format((*it)->_frequency) + ", tsid: " + Poco::NumberFormatter::format((*it)->_transportStreamId) + ")");
        if (tune(*it)) {
            scanTransponder(*it);
            if (addKownTransponder(*it)) {
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
    Poco::XML::Element* pFrontend = pDoc->createElement("frontend");
    pFrontend->setAttribute("type", getType());
    pFrontend->setAttribute("name", getName());
    pAdapter->appendChild(pFrontend);
    for (std::vector<Transponder*>::iterator it = _transponders.begin(); it != _transponders.end(); ++it) {
        (*it)->writeXml(pFrontend);
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
Frontend::getInitialTransponderKeys(const std::string& countryCode, std::vector<std::string>& keys)
{

}


void
Frontend::getInitialTransponderData(const std::string& key)
{
    std::istringstream ss(TransponderData::instance()->getResource("transponder.zip"), std::ios::binary);
    Poco::Zip::ZipArchive arch(ss);
    Poco::Zip::ZipArchive::FileHeaders::const_iterator it = arch.findHeader(key);
    if (it == arch.headerEnd()) {
        LOG(dvb, error, "transponder data not found for: " + key);
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


//bool
//Frontend::tune(Channel* pChannel)
//{
//    LOG(dvb, debug, "frontend tuning to channel");
//
//    bool success = false;
//    unsigned int ifreq;
//    if (getType() == DVBS) {
//        bool hiBand = _pAdapter->_pLnb->isHiBand(pChannel->_freq, ifreq);
//        diseqc(pChannel->_satNum, pChannel->_pol, hiBand);
//    }
//    else if (getType() == DVBT) {
//        ifreq = pChannel->_freq;
//    }
//    else {
//        LOG(dvb, error, "frontend type not yet supported, stop tuning.");
//        return false;
//    }
//
//    if (tuneFrontend(ifreq, pChannel->_symbolRate)) {
//        if (_pAdapter->_pDemux->setVideoPid(pChannel->_vpid) &&
//            _pAdapter->_pDemux->setAudioPid(pChannel->_apid) &&
//            _pAdapter->_pDemux->setPcrPid(pChannel->_cpid)) {
//                success = true;
//                if (_pAdapter->_recPsi) {
//                    unsigned int pmtPid = pChannel->_pmtid;
//                    if (pmtPid == 0) {
//                        pmtPid = _pAdapter->_pDemux->getPmtPid(pChannel->_tid, pChannel->_sid);
//                    }
//                    LOG(dvb, debug, "pmt pid: " + Poco::NumberFormatter::format(pmtPid));
//                    if (pmtPid == 0) {
//                        LOG(dvb, error, "couldn't find pmt-pid for sid");
//                        success = false;
//                    }
//                    else if (!_pAdapter->_pDemux->setPatPid(0) ||
//                             !_pAdapter->_pDemux->setPmtPid(pmtPid))
//                    {
//                        success = false;
//                    }
//                }
//        }
//    }
//
//    LOG(dvb, debug, "frontend tuning " + std::string(success ? "success." : "failed."));
//    return success;
//}


//void
//Frontend::stopTune()
//{
//    _pt->stop();
//    delete _pt;
//}


//bool
//Frontend::tuneFrontend(unsigned int freq, unsigned int symbolRate)
//{
//    LOG(dvb, debug, "tune frontend to frequency: " + Poco::NumberFormatter::format(freq) + " ...");
//
//    struct dvb_frontend_parameters tuneto;
//    struct dvb_frontend_event event;
//
//    // discard stale QPSK events
//    while (1) {
//        if (ioctl(_fileDescFrontend, FE_GET_EVENT, &event) == -1)
//            break;
//    }
//
//    tuneto.frequency = freq;
//    tuneto.inversion = INVERSION_AUTO;
//    if (getType() == DVBS) {
//        tuneto.u.qpsk.symbol_rate = symbolRate;
//        tuneto.u.qpsk.fec_inner = FEC_AUTO;
//    }
//    else if (getType() == DVBT) {
//        tuneto.u.ofdm.bandwidth = BANDWIDTH_8_MHZ;
//        tuneto.u.ofdm.code_rate_HP = FEC_2_3;
//        tuneto.u.ofdm.code_rate_LP = FEC_NONE;
//        tuneto.u.ofdm.constellation = QAM_16;
//        tuneto.u.ofdm.transmission_mode = TRANSMISSION_MODE_8K;
//        tuneto.u.ofdm.guard_interval = GUARD_INTERVAL_1_4;
//        tuneto.u.ofdm.hierarchy_information = HIERARCHY_NONE;
//    }
//
//    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
//        LOG(dvb, error, "FE_SET_FRONTEND failed");
//        return false;
//    }
//
//    LOG(dvb, debug, "tune frontend to frequency finished.");
//
//    return true;
//}


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


//bool
//Frontend::addKownTransponder(Transponder* pTransponder)
//{
//    bool newTransponder = false;
//
//    std::map<unsigned int, Transponder*>::iterator pos = _scannedTransponders.find(pTransponder->_transportStreamId);
////    std::map<Poco::UInt16, std::map<Poco::UInt16, Transponder*> >::iterator pos = _scannedTransponders.find(pTransponder->);
//    if (pos == _scannedTransponders.end()) {
//        LOG(dvb, trace, "new transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
//        newTransponder = true;
//        _scannedTransponders[pTransponder->_transportStreamId] = pTransponder;
//    }
//    else {
//        LOG(dvb, trace, "known transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
//    }
//    return newTransponder;
//}


bool
Frontend::addKownTransponder(Transponder* pTransponder)
{
    bool newTransponder = false;
    std::map<unsigned int, Transponder*>::iterator pos = _scannedTransponders.find(pTransponder->_transportStreamId);
    if (pos == _scannedTransponders.end()) {
        LOG(dvb, trace, "new transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
        newTransponder = true;
        _scannedTransponders[pTransponder->_transportStreamId] = pTransponder;
    }
    else {
        LOG(dvb, trace, "known transponder (freq: " + Poco::NumberFormatter::format(pTransponder->_frequency) + ", tsid: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + ")");
    }
    return newTransponder;
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


void
Frontend::scanTransponder(Transponder* pTransponder)
{
    LOG(dvb, trace, "************** Transponder **************");
    scanPatPmt(pTransponder);
//    scanSdt(pTransponder);
    scanNit(pTransponder);
}


void
Frontend::scanPatPmt(Transponder* pTransponder)
{
    LOG(dvb, trace, "--------------     PAT     --------------");
    PatSection pat;
    if (_pDemux->readSection(&pat)) {
        if (pTransponder->_transportStreamId = Transponder::InvalidTransportStreamId) {
            pTransponder->_transportStreamId = pat.transportStreamId();
        }
        else if (pTransponder->_transportStreamId != pat.transportStreamId()) {
            LOG(dvb, error, "transport stream id mismatch: " + Poco::NumberFormatter::format(pTransponder->_transportStreamId) + " != " + Poco::NumberFormatter::format(pat.transportStreamId()));
        }
        LOG(dvb, trace, "service count: " + Poco::NumberFormatter::format(pat.serviceCount()));
        for (int serviceIndex = 0; serviceIndex < pat.serviceCount(); serviceIndex++) {
            LOG(dvb, trace, "--------------     PMT     --------------");
            LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pat.serviceId(serviceIndex)) +
                          ", pmt pid: " + Poco::NumberFormatter::format(pat.pmtPid(serviceIndex)));
            PmtSection pmt(pat.pmtPid(serviceIndex));
            if (pat.serviceId(serviceIndex) && _pDemux->readSection(&pmt)) {
                for (int streamIndex = 0; streamIndex < pmt.streamCount(); streamIndex++) {
                    LOG(dvb, trace, "stream pid: " + Poco::NumberFormatter::format(pmt.streamPid(streamIndex)) +
                                  ", type: " + Stream::streamTypeToString(pmt.streamType(streamIndex)));
                }
            }
        }
    }
}


void
Frontend::scanSdt(Transponder* pTransponder)
{
    LOG(dvb, trace, "--------------     SDT     --------------");
    SdtSection sdt;
    if (_pDemux->readSection(&sdt)) {
        for (int serviceIndex = 0; serviceIndex < sdt.serviceCount(); serviceIndex++) {
            LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(sdt.serviceId(serviceIndex)) +
                          ", running status: " + Poco::NumberFormatter::format(sdt.runningStatus(serviceIndex)) +
                          ", scrambled: " + Poco::NumberFormatter::format(sdt.scrambled(serviceIndex)));
        }
    }
}


void
Frontend::scanNit(Transponder* pTransponder)
{
    std::vector<Transponder*> additionalTransponders;
    LOG(dvb, trace, "--------------     NIT     --------------");
    NitOtherSection nit;
    if (_pDemux->readSection(&nit)) {
        LOG(dvb, trace, "network id: " + Poco::NumberFormatter::format(nit.networkId()) + ", name: " + nit.networkName());
//        LOG(dvb, trace, "network descriptor count: " + Poco::NumberFormatter::format(nit.networkDescriptorCount()));
        for (unsigned int t = 0; t < nit.transportStreamCount(); t++) {
            LOG(dvb, trace, "original network id: " + Poco::NumberFormatter::format(nit.originalNetworkId(t)) +
                          ", transport stream id: " + Poco::NumberFormatter::format(nit.transportStreamId(t)));

            for (unsigned int d = 0; d < nit.transportStreamDescriptorCount(t); d++) {
                Descriptor* pDescriptor = nit.transportStreamDescriptor(t, d);
                if (ServiceListDescriptor* pD = dynamic_cast<ServiceListDescriptor*>(pDescriptor)) {
//                    LOG(dvb, trace, "service count: " + Poco::NumberFormatter::format(pD->serviceCount()));
                    for (int i = 0; i < pD->serviceCount(); i++) {
//                        LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pD->serviceId(i)));
                    }
                }
                else if (SatelliteDeliverySystemDescriptor* pD = dynamic_cast<SatelliteDeliverySystemDescriptor*>(pDescriptor)) {
                    LOG(dvb, trace, "orbital position: " + Poco::NumberFormatter::format(pD->orbitalPosition()) +
                                  ", frequency[kHz]: " + Poco::NumberFormatter::format(pD->frequency()));
                    SatTransponder* pT = new SatTransponder(this, pD->frequency(), nit.transportStreamId(t));
                    pT->init(0, pD->symbolRate(), pD->polarization());
                    additionalTransponders.push_back(pT);
                }
                else if (TerrestrialDeliverySystemDescriptor* pD = dynamic_cast<TerrestrialDeliverySystemDescriptor*>(pDescriptor)) {
                    LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency()));
                    TerrestrialTransponder* pT = new TerrestrialTransponder(this, pD->centreFrequency(), nit.transportStreamId(t));
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
                else if (FrequencyListDescriptor* pD = dynamic_cast<FrequencyListDescriptor*>(pDescriptor)) {
                    for (int i = 0; i < pD->centreFrequencyCount(); i++) {
                        LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency(i)));
                    }
                }
            }
        }
    }
    for (std::vector<Transponder*>::iterator it = additionalTransponders.begin(); it != additionalTransponders.end(); ++it) {
        if (addKownTransponder(*it) && tune(*it)) {
            scanTransponder(*it);
            addTransponder(*it);
        }
        else {
            delete *it;
        }
    }
}


//void
//Frontend::scanTransponderOld(Transponder* pTransponder)
//{
//    LOG(dvb, trace, "************** PAT **************");
//    Poco::UInt16 patPid = 0x00;
//    Stream* pPat = new Stream(Stream::Other, patPid);
//    _pDemux->selectStream(pPat, Demux::TargetDemux, true);
//    Poco::UInt8 patTableId = 0x00;
//    _pDemux->setSectionFilter(pPat, patTableId);
//    _pDemux->runStream(pPat, true);
//    Section patTable(patTableId);
//    patTable.read(pPat);
//    _pDemux->runStream(pPat, false);
//    _pDemux->unselectStream(pPat);
//
//    LOG(dvb, trace, "pat table length: " + Poco::NumberFormatter::format(patTable.length()));
//    Poco::UInt16 transportStreamId = patTable.getValue<Poco::UInt16>(24, 16);
//    LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transportStreamId));
//    pTransponder->_transportStreamId = transportStreamId;
//
//    int serviceCount = (patTable.length() - 8 - 4) / 4;  // section header size = 8,  crc = 4, service section size = 4
//    unsigned int headerSize = 8 * 8;
//    unsigned int serviceSize = 4 * 8;
//    while (serviceCount--) {
//        LOG(dvb, trace, "************** PMT (Service) **************");
//
//        Poco::UInt16 serviceId = patTable.getValue<Poco::UInt16>(headerSize + serviceCount * serviceSize, 16);
//        LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(serviceId));
//
//        Poco::UInt16 pmtPid = patTable.getValue<Poco::UInt16>(headerSize + serviceCount * serviceSize + 19, 13);
//        LOG(dvb, trace, "pmt pid: " + Poco::NumberFormatter::format(pmtPid));
//
//        if (serviceId) {  // no NIT service
//            Service* pService = new Dvb::Service(pTransponder, "", serviceId, pmtPid);
//            pTransponder->addService(pService);
//
//            Stream* pPmt = new Stream(Stream::ProgramMapTable, pmtPid);
//            _pDemux->selectStream(pPmt, Demux::TargetDemux, true);
//            Poco::UInt8 pmtTableId = 0x02;
//            _pDemux->setSectionFilter(pPmt, pmtTableId);
//            _pDemux->runStream(pPmt, true);
//            Section pmtTable(pmtTableId);
//            try {
//                pmtTable.read(pPmt, 5);
//                LOG(dvb, debug, "pmt table length: " + Poco::NumberFormatter::format(pmtTable.length()));
//                Poco::UInt16 programInfoLength = pmtTable.getValue<Poco::UInt16>(84, 12);
//                LOG(dvb, trace, "program info length: " + Poco::NumberFormatter::format(programInfoLength));
//                unsigned int headerSize = 96 + programInfoLength * 8;
//                unsigned int totalStreamSectionSize = pmtTable.length() * 8 - headerSize - 4 * 8;
//                unsigned int offset = 0;
//                while (offset < totalStreamSectionSize) {
//                    Poco::UInt8 streamType = pmtTable.getValue<Poco::UInt8>(headerSize + offset, 8);
//                    LOG(dvb, trace, "stream type: " + Poco::NumberFormatter::format(streamType));
//                    Poco::UInt16 streamPid = pmtTable.getValue<Poco::UInt16>(headerSize + offset + 11, 13);
//                    LOG(dvb, trace, "stream pid: " + Poco::NumberFormatter::format(streamPid));
//                    Poco::UInt16 esInfoLength = pmtTable.getValue<Poco::UInt16>(headerSize + offset + 28, 12);
//                    LOG(dvb, trace, "es info length: " + Poco::NumberFormatter::format(esInfoLength));
//                    offset += 40 + esInfoLength * 8;
//
//                    Stream* pStream = new Stream(Stream::streamTypeToString(streamType), streamPid);
//                    pService->addStream(pStream);
//                }
//                pService->addStream(pPmt);
//            }
//            catch (Poco::TimeoutException& e) {
//                LOG(dvb, error, "PMT table read timeout");
//            }
//            _pDemux->runStream(pPmt, false);
//            _pDemux->unselectStream(pPmt);
//        }
//    }
//    LOG(dvb, trace, "************** PAT/PMT finished **************");
//
//    Poco::UInt16 sdtPid = 0x11;
//    Stream* pSdt = new Stream(Stream::Other, sdtPid);
//    _pDemux->selectStream(pSdt, Demux::TargetDemux, true);
//    Poco::UInt8 sdtTableId = 0x42;
//    _pDemux->setSectionFilter(pSdt, sdtTableId);
//    _pDemux->runStream(pSdt, true);
//    Section sdtTable(sdtTableId);
//    try {
//        sdtTable.read(pSdt, 5);
//        unsigned int sdtHeaderSize = 88;
//        unsigned int totalSdtSectionSize = sdtTable.length() * 8 - sdtHeaderSize - 4 * 8;
//        unsigned int sdtOffset = 0;
//        while (sdtOffset < totalSdtSectionSize) {
//            LOG(dvb, trace, "************** SDT (Service) **************");
//
//            Poco::UInt16 sdtServiceId = sdtTable.getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset, 16);
//            LOG(dvb, trace, "std service id: " + Poco::NumberFormatter::format(sdtServiceId));
//            Poco::UInt8 runningStatus = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 24, 3);
//            LOG(dvb, trace, "sdt running status: " + Poco::NumberFormatter::format(runningStatus));
//            Poco::UInt8 scrambled = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 27, 1);
//            LOG(dvb, trace, "sdt scrambled: " + Poco::NumberFormatter::format(scrambled));
//            Poco::UInt16 sdtInfoLength = sdtTable.getValue<Poco::UInt16>(sdtHeaderSize + sdtOffset + 28, 12);
//    //        LOG(dvb, trace, "sdt descriptor total length: " + Poco::NumberFormatter::format(sdtInfoLength));
//            Poco::UInt8 descId = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 40, 8);
//    //        LOG(dvb, trace, "sdt descriptor tag: " + Poco::NumberFormatter::format(descId));
//            Poco::UInt8 descLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 48, 8);
//    //        LOG(dvb, trace, "sdt descriptor length: " + Poco::NumberFormatter::format(descLength));
//            Poco::UInt8 descType = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 56, 8);
//            LOG(dvb, trace, "sdt descriptor type: " + Poco::NumberFormatter::format(descType));
//            Poco::UInt8 descProvNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 64, 8);
//    //        LOG(dvb, trace, "sdt descriptor provider name length: " + Poco::NumberFormatter::format(descProvNameLength));
//            std::string providerName((char*)(sdtTable.getData()) + (sdtHeaderSize + sdtOffset + 72) / 8, descProvNameLength);
//            LOG(dvb, trace, "sdt provider name: " + providerName);
//            Poco::UInt8 descServiceNameLength = sdtTable.getValue<Poco::UInt8>(sdtHeaderSize + sdtOffset + 72 + descProvNameLength * 8, 8);
//    //        LOG(dvb, trace, "sdt descriptor service name length: " + Poco::NumberFormatter::format(descServiceNameLength));
//            std::string serviceName((char*)(sdtTable.getData()) + (sdtHeaderSize + sdtOffset + 80 + descProvNameLength * 8) / 8, descServiceNameLength);
//            LOG(dvb, trace, "sdt service name: " + serviceName);
//
//            Service* pService = pTransponder->getService(sdtServiceId);
//            if (pService) {
////                pService->_name = trim(serviceName);
//                pService->_name = filter(serviceName);
//            }
//
//            sdtOffset += 40 + sdtInfoLength * 8;
//        }
//    }
//    catch (Poco::TimeoutException& e) {
//        LOG(dvb, error, "SDT table read timeout");
//    }
//    _pDemux->runStream(pSdt, false);
//    _pDemux->unselectStream(pSdt);
//
//    LOG(dvb, trace, "************** NIT **************");
//    Poco::UInt16 nitPid = 0x10;
//    Stream* pNit = new Stream(Stream::Other, nitPid);
//    _pDemux->selectStream(pNit, Demux::TargetDemux, true);
////            Poco::UInt8 nitTableId = 0x40;
//    Poco::UInt8 nitTableId = 0x41;
//    _pDemux->setSectionFilter(pNit, nitTableId);
//    _pDemux->runStream(pNit, true);
//    Section nitTable(nitTableId);
//    std::vector<Transponder*> additionalTransponders;
//    try {
//        nitTable.read(pNit, 15);
//        LOG(dvb, trace, "nit table length: " + Poco::NumberFormatter::format(nitTable.length()));
//        Poco::UInt16 networkId = nitTable.getValue<Poco::UInt16>(24, 16);
//        LOG(dvb, trace, "network id: " + Poco::NumberFormatter::format(networkId));
//        Poco::UInt16 networkDescriptorsLength = nitTable.getValue<Poco::UInt16>(68, 12);
////        LOG(dvb, trace, "network descriptors length: " + Poco::NumberFormatter::format(networkDescriptorsLength));
//        unsigned int head = 80;
//        unsigned int byteOffset = 0;
//        while (byteOffset < networkDescriptorsLength) {
//            Descriptor* pDescriptor = Descriptor::createDescriptor(nitTable.getData(10 + byteOffset));
//            if (NetworkNameDescriptor* pd = dynamic_cast<NetworkNameDescriptor*>(pDescriptor)) {
//                std::string networkName = pd->getNetworkName();
//                LOG(dvb, trace, "network name: " + networkName);
//            }
//            if (pDescriptor) {
//                byteOffset += pDescriptor->getDescriptorLength();
//            }
//            else {
//                break;
//            }
//        }
//        Poco::UInt16 transportStreamLoopLength = nitTable.getValue<Poco::UInt16>(head + networkDescriptorsLength * 8 + 4, 12);
////        LOG(dvb, trace, "transport stream loop length: " + Poco::NumberFormatter::format(transportStreamLoopLength));
//        head = head + networkDescriptorsLength * 8 + 16;
//        byteOffset = 0;
//        while (byteOffset < transportStreamLoopLength) {
//            Poco::UInt16 transportStreamId = nitTable.getValue<Poco::UInt16>(head + byteOffset * 8, 16);
//            LOG(dvb, trace, "transport stream id: " + Poco::NumberFormatter::format(transportStreamId));
//            Poco::UInt16 originalNetworkId = nitTable.getValue<Poco::UInt16>(head + byteOffset * 8 + 16, 16);
//            LOG(dvb, trace, "originial network id: " + Poco::NumberFormatter::format(originalNetworkId));
//            Poco::UInt16 transportDescriptorsLength = nitTable.getValue<Poco::UInt16>(head + byteOffset * 8 + 36, 12);
////            LOG(dvb, trace, "transport descriptors length: " + Poco::NumberFormatter::format(transportDescriptorsLength));
//
//            unsigned int tHead = head + byteOffset * 8 + 48;
//            unsigned int tByteOffset = 0;
//            while (tByteOffset < transportDescriptorsLength) {
//                Descriptor* pDescriptor = Descriptor::createDescriptor(nitTable.getData(tHead / 8 + tByteOffset));
//                if (ServiceListDescriptor* pd = dynamic_cast<ServiceListDescriptor*>(pDescriptor)) {
//                    LOG(dvb, trace, "service count: " + Poco::NumberFormatter::format(pd->serviceCount()));
//                    for (int i = 0; i < pd->serviceCount(); i++) {
//                        LOG(dvb, trace, "service id: " + Poco::NumberFormatter::format(pd->serviceId(i)));
//                    }
//                }
//                else if (SatelliteDeliverySystemDescriptor* pD = dynamic_cast<SatelliteDeliverySystemDescriptor*>(pDescriptor)) {
//                    LOG(dvb, trace, "frequency[kHz]: " + Poco::NumberFormatter::format(pD->frequency()));
//                    SatTransponder* pT = new SatTransponder(this, pD->frequency(), transportStreamId);
//                    pT->init(0, pD->symbolRate(), pD->polarization());
//                    additionalTransponders.push_back(pT);
//                }
//                else if (TerrestrialDeliverySystemDescriptor* pD = dynamic_cast<TerrestrialDeliverySystemDescriptor*>(pDescriptor)) {
//                    LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency()));
//                }
//                else if (FrequencyListDescriptor* pD = dynamic_cast<FrequencyListDescriptor*>(pDescriptor)) {
//                    for (int i = 0; i < pD->centreFrequencyCount(); i++) {
//                        LOG(dvb, trace, "centre frequency[Hz]: " + Poco::NumberFormatter::format(pD->centreFrequency(i)));
//                    }
//                }
//                if (pDescriptor) {
//                    tByteOffset += pDescriptor->getDescriptorLength();
//                }
//                else {
//                    break;
//                }
//            }
//
//            byteOffset += transportDescriptorsLength + 6;
//        }
//    }
//    catch (Poco::TimeoutException& e) {
//        LOG(dvb, error, "NIT table read timeout");
//    }
//    _pDemux->runStream(pNit, false);
//    _pDemux->unselectStream(pNit);
//
//    for (std::vector<Transponder*>::iterator it = additionalTransponders.begin(); it != additionalTransponders.end(); ++it) {
//
////        bool alreadyScanned = false;
////        for (std::vector<Transponder*>::iterator tit = _scannedTransponders.begin(); tit != _scannedTransponders.end(); ++tit) {
////            if ((*it)->_tid == (*tit)->_tid) {
////                alreadyScanned = true;
////                break;
////            }
////        }
////        if (alreadyScanned) {
////            continue;
////        }
//
//        std::map<unsigned int, Transponder*>::iterator pos = _scannedTransponders.find((*it)->_transportStreamId);
//        if (pos == _scannedTransponders.end()) {
//            LOG(dvb, debug, "************** scan new transport stream with id: " + Poco::NumberFormatter::format((*it)->_transportStreamId) + ", freq: " + Poco::NumberFormatter::format((*it)->_frequency));
//            _scannedTransponders[(*it)->_transportStreamId] = *it;
//            if (tune(*it)) {
//                scanTransponder(*it);
//                _transponders.push_back(*it);
//            }
//        }
//        else {
//            LOG(dvb, debug, "************** transport stream with id already scanned: " + Poco::NumberFormatter::format((*it)->_transportStreamId) + ", freq: " + Poco::NumberFormatter::format((*it)->_frequency));
//        }
//    }
//}


//std::string
//Frontend::trim(const std::string& str)
//	/// Returns a copy of str with all leading and
//	/// trailing whitespace removed.
//{
//	int first = 0;
//	int last  = int(str.size()) - 1;
//
//	while (first <= last && std::iscntrl(str[first])) ++first;
//	while (last >= first && std::iscntrl(str[last])) --last;
//
//	return std::string(str, first, last - first + 1);
//}


std::string
Frontend::filter(const std::string& str)
{
    std::string res(str);
    for (int i = 0; i < res.length(); i++) {
        if (!std::isprint(res[i])) {
            res[i] = ' ';
        }
    }
    return Poco::trim(res);
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


SatFrontend::SatFrontend(Adapter* pAdapter, int num) :
Frontend(pAdapter, num)
{
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
    LOG(dvb, debug, "sat frontend tuning to transponder with frequency: " + Poco::NumberFormatter::format(pTransponder->_frequency) + " ...");

    SatTransponder* pTrans = static_cast<SatTransponder*>(pTransponder);
    struct dvb_frontend_parameters tuneto;

    unsigned int ifreq;
    bool hiBand = _pLnb->isHiBand(pTrans->_frequency, ifreq);
    diseqc(pTrans->_satNum, pTrans->_polarization, hiBand);

    tuneto.frequency = ifreq;
    tuneto.inversion = INVERSION_AUTO;
    tuneto.u.qpsk.symbol_rate = pTrans->_symbolRate;
    tuneto.u.qpsk.fec_inner = FEC_AUTO;

    if (ioctl(_fileDescFrontend, FE_SET_FRONTEND, &tuneto) == -1) {
        LOG(dvb, debug, "sat frontend tuning failed.");
        return false;
    }
    return waitForLock(_frontendTimeout);
}


Transponder*
SatFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new SatTransponder(this, freq, tsid);
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


bool
TerrestrialFrontend::tune(Transponder* pTransponder)
{
    LOG(dvb, debug, "terrestrial frontend tuning to transponder with frequency: " + Poco::NumberFormatter::format(pTransponder->_frequency) + " ...");

    TerrestrialTransponder* pTrans = static_cast<TerrestrialTransponder*>(pTransponder);
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
    return waitForLock(_frontendTimeout);
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
    return waitForLock(_frontendTimeout);
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
    return waitForLock(_frontendTimeout);
}


Transponder*
AtscFrontend::createTransponder(unsigned int freq, unsigned int tsid)
{
    return new AtscTransponder(this, freq, tsid);
}


}  // namespace Omm
}  // namespace Dvb
