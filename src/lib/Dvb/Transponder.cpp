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

#include <Poco/NumberParser.h>
#include <Poco/NumberFormatter.h>
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

#include "DvbLogger.h"
#include "Service.h"
#include "Transponder.h"
#include "Frontend.h"


namespace Omm {
namespace Dvb {

const int Transponder::InvalidTransportStreamId(-1);

Transponder::Transponder(Frontend* pFrontend, unsigned int freq, int tsid) :
_pFrontend(pFrontend),
_frequency(freq),
_transportStreamId(tsid)
{
}


void
Transponder::addService(Dvb::Service* pService)
{
    _services.push_back(pService);
}


Service*
Transponder::getService(unsigned int serviceId)
{
    std::vector<Dvb::Service*>::iterator it = _services.begin();
    while (it != _services.end() && (*it)->_sid != serviceId) {
        ++it;
    }
    if (it != _services.end()) {
        return *it;
    }
    else {
        return 0;
    }
}


unsigned int
Transponder::getFrequency()
{
    return _frequency;
}


Service*
Transponder::getService(const std::string& serviceName)
{
    std::vector<Dvb::Service*>::iterator it = _services.begin();
    while (it != _services.end() && (*it)->_name != serviceName) {
        ++it;
    }
    if (it != _services.end()) {
        return *it;
    }
    else {
        return 0;
    }
}


void
Transponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read transponder ...");

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlService = pXmlTransponder->firstChild();
        while (pXmlService && pXmlService->nodeName() == "service") {
            std::string name = static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("name");
            int sid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("sid"));
            unsigned int pmtid = Poco::NumberParser::parse(static_cast<Poco::XML::Element*>(pXmlService)->getAttribute("pmtid"));
            LOG(dvb, debug, "new service with name: " + name);

            Service* pService = new Service(this, name, sid, pmtid);
            pService->readXml(pXmlService);
            addService(pService);
            pXmlService = pXmlService->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb transponder description contains no services");
        return;
    }

    LOG(dvb, debug, "read transponder.");
}


void
Transponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write transponder ...");

    Poco::XML::Document* pDoc = pFrontend->ownerDocument();
    _pXmlTransponder = pDoc->createElement("transponder");
    pFrontend->appendChild(_pXmlTransponder);
    _pXmlTransponder->setAttribute("frequency", Poco::NumberFormatter::format(_frequency));
    _pXmlTransponder->setAttribute("tsid", Poco::NumberFormatter::format(_transportStreamId));

    for (std::vector<Service*>::iterator it = _services.begin(); it != _services.end(); ++it) {
        (*it)->writeXml(_pXmlTransponder);
    }

    LOG(dvb, debug, "wrote transponder.");
}


bool
Transponder::equal(Transponder* pOtherTransponder)
{
//    LOG(dvb, trace, "test transponder with freq: " + Poco::NumberFormatter::format(_frequency) + " equals transponder with freq: " + Poco::NumberFormatter::format(pOtherTransponder->_frequency));
    return (abs(_frequency - pOtherTransponder->_frequency) < 2000);
}


const std::string SatTransponder::POL_HORIZ("H");
const std::string SatTransponder::POL_VERT("V");
const std::string SatTransponder::POL_CIRC_LEFT("CL");
const std::string SatTransponder::POL_CIRC_RIGHT("CR");
const std::string SatTransponder::MOD_S("DVBS");
const std::string SatTransponder::MOD_S2("DVBS2");
const std::string SatTransponder::MOD_TYPE_AUTO("Auto");
const std::string SatTransponder::MOD_TYPE_QPSK("QPSK");
const std::string SatTransponder::MOD_TYPE_8PSK("8PSK");
const std::string SatTransponder::MOD_TYPE_16QAM("16QAM");
const std::string SatTransponder::FEC_NOT_DEFINED("NotDefined");
const std::string SatTransponder::FEC_1_2("1/2");
const std::string SatTransponder::FEC_2_3("2/3");
const std::string SatTransponder::FEC_3_4("3/4");
const std::string SatTransponder::FEC_5_6("5/6");
const std::string SatTransponder::FEC_7_8("7/8");
const std::string SatTransponder::FEC_8_9("8/9");
const std::string SatTransponder::FEC_3_5("3/5");
const std::string SatTransponder::FEC_4_5("4/5");
const std::string SatTransponder::FEC_9_10("9/10");
const std::string SatTransponder::FEC_NO_CODING("NoCoding");


SatTransponder::SatTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid) :
Transponder(pFrontend, freq, tsid),
_satNum(SatFrontend::InvalidSatNum)
{
}


void
SatTransponder::init(const std::string satPosition, unsigned int satNum, unsigned int symbolRate, const std::string& polarization)
{
    LOG(dvb, debug, "init sat transponder.");

    SatFrontend* pFrontend = dynamic_cast<SatFrontend*>(_pFrontend);
    if (!pFrontend) {
         LOG(dvb, error, "frontend of sat transponder is not sat frontend, cannot init transponder");
         return;
    }

    if (satPosition != "") {
        _satPosition = satPosition;
    }
    if (satNum != SatFrontend::InvalidSatNum) {
        _satNum = satNum;
    }
    else if (_satPosition != "") {
        _satNum = pFrontend->getSatNum(_satPosition);
    }
//    if (_satPosition != "" && _satNum != SatFrontend::InvalidSatNum) {
//        pFrontend->setSatNum(_satPosition, _satNum);
//    }
    _symbolRate = symbolRate;
    _polarization = polarization;
}


void
SatTransponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read sat transponder ...");

    Transponder::readXml(pXmlTransponder);

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlTransponder->firstChild();
        while (pXmlParam) {
            Poco::XML::Node* pXmlParamVal = pXmlParam->firstChild();
            if (!pXmlParamVal) {
                LOG(dvb, error, "dvb sat transponder parameter without value: " + pXmlParam->nodeName());
                pXmlParam = pXmlParam->nextSibling();
                continue;
            }
            if (pXmlParam->nodeName() == "service") {
                // do nothing
            }
            else if (pXmlParam->nodeName() == "satNum") {
                _satNum = Poco::NumberParser::parse(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "symbolRate") {
                _symbolRate = Poco::NumberParser::parse(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "polarization") {
                _polarization = pXmlParamVal->innerText();
            }
            else {
                LOG(dvb, error, "dvb sat transponder unknown parameter: " + pXmlParam->nodeName());
            }

            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb sat transponder description contains no tuning parameters");
        return;
    }

    LOG(dvb, debug, "read sat transponder.");
}


void
SatTransponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write sat transponder ...");

    Transponder::writeXml(pFrontend);
    Poco::XML::Document* pDoc = pFrontend->ownerDocument();

    Poco::AutoPtr<Poco::XML::Element> pSatNum = pDoc->createElement("satNum");
    Poco::AutoPtr<Poco::XML::Text> pSatNumVal = pDoc->createTextNode(Poco::NumberFormatter::format(_satNum));
    pSatNum->appendChild(pSatNumVal);
    _pXmlTransponder->appendChild(pSatNum);

    Poco::AutoPtr<Poco::XML::Element> pSymbolRate = pDoc->createElement("symbolRate");
    Poco::AutoPtr<Poco::XML::Text> pSymbolRateVal = pDoc->createTextNode(Poco::NumberFormatter::format(_symbolRate));
    pSymbolRate->appendChild(pSymbolRateVal);
    _pXmlTransponder->appendChild(pSymbolRate);

    Poco::AutoPtr<Poco::XML::Element> pPolarization = pDoc->createElement("polarization");
    Poco::AutoPtr<Poco::XML::Text> pPolarizationVal = pDoc->createTextNode(_polarization);
    pPolarization->appendChild(pPolarizationVal);
    _pXmlTransponder->appendChild(pPolarization);

    LOG(dvb, debug, "wrote sat transponder.");
}


bool
SatTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "S" || params.count() < 4) {
        LOG(dvb, error, "invalid parameter data for sat transponder.");
        return false;
    }
    unsigned int symbolRate;
    try {
        symbolRate = Poco::NumberParser::parse(params[3]);
    }
    catch (Poco::Exception& e) {
        LOG(dvb, error, "invalid symbol rate for sat transponder: " + params[3]);
        return false;
    }
    init("", SatFrontend::InvalidSatNum, symbolRate, params[2]);
    return true;
}


const std::string TerrestrialTransponder::BANDWIDTH_8_MHZ("8_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_7_MHZ("7_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_6_MHZ("6_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_AUTO("AUTO");
const std::string TerrestrialTransponder::BANDWIDTH_5_MHZ("5_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_10_MHZ("10_MHZ");
const std::string TerrestrialTransponder::BANDWIDTH_1_712_MHZ("1_712_MHZ");
const std::string TerrestrialTransponder::PRIORITY_HIGH("HP");
const std::string TerrestrialTransponder::PRIORITY_LOW("LP");
const std::string TerrestrialTransponder::FEC_NONE("NONE");
const std::string TerrestrialTransponder::FEC_1_2("1/2");
const std::string TerrestrialTransponder::FEC_2_3("2/3");
const std::string TerrestrialTransponder::FEC_3_4("3/4");
const std::string TerrestrialTransponder::FEC_4_5("4/5");
const std::string TerrestrialTransponder::FEC_5_6("5/6");
const std::string TerrestrialTransponder::FEC_6_7("6/7");
const std::string TerrestrialTransponder::FEC_7_8("7/8");
const std::string TerrestrialTransponder::FEC_8_9("8/9");
const std::string TerrestrialTransponder::FEC_AUTO("AUTO");
const std::string TerrestrialTransponder::FEC_3_5("3/5");
const std::string TerrestrialTransponder::FEC_9_10("9/10");
const std::string TerrestrialTransponder::QPSK("QPSK");
const std::string TerrestrialTransponder::QAM_16("QAM16");
const std::string TerrestrialTransponder::QAM_32("QAM32");
const std::string TerrestrialTransponder::QAM_64("QAM64");
const std::string TerrestrialTransponder::QAM_128("QAM128");
const std::string TerrestrialTransponder::QAM_256("QAM256");
const std::string TerrestrialTransponder::QAM_AUTO("AUTO");
const std::string TerrestrialTransponder::VSB_8("8VSB");
const std::string TerrestrialTransponder::VSB_16("16VSB");
const std::string TerrestrialTransponder::PSK_8("8PSK");
const std::string TerrestrialTransponder::APSK_16("16APSK");
const std::string TerrestrialTransponder::APSK_32("32APSK");
const std::string TerrestrialTransponder::DQPSK("DQPSK");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_2K("2k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_8K("8k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_AUTO("AUTO");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_4K("4k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_1K("1k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_16K("16k");
const std::string TerrestrialTransponder::TRANSMISSION_MODE_32K("32k");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_32("1/32");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_16("1/16");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_8("1/8");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_4("1/4");
const std::string TerrestrialTransponder::GUARD_INTERVAL_AUTO("AUTO");
const std::string TerrestrialTransponder::GUARD_INTERVAL_1_128("1/128");
const std::string TerrestrialTransponder::GUARD_INTERVAL_19_128("19/128");
const std::string TerrestrialTransponder::GUARD_INTERVAL_19_256("19/256");
const std::string TerrestrialTransponder::HIERARCHY_NONE("NONE");
const std::string TerrestrialTransponder::HIERARCHY_1("1");
const std::string TerrestrialTransponder::HIERARCHY_2("2");
const std::string TerrestrialTransponder::HIERARCHY_4("4");
const std::string TerrestrialTransponder::HIERARCHY_AUTO("AUTO");


TerrestrialTransponder::TerrestrialTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid) :
Transponder(pFrontend, freq, tsid)
{
}


void
TerrestrialTransponder::init(fe_bandwidth_t bandwidth,
            fe_code_rate_t code_rate_HP,
            fe_code_rate_t code_rate_LP,
            fe_modulation_t constellation,
            fe_transmit_mode_t transmission_mode,
            fe_guard_interval_t guard_interval,
            fe_hierarchy_t hierarchy_information)
{
    LOG(dvb, debug, "init terrestrial transponder: " +
            Poco::NumberFormatter::format(bandwidth) + ", " +
            Poco::NumberFormatter::format(code_rate_HP) + ", " +
            Poco::NumberFormatter::format(code_rate_LP) + ", " +
            Poco::NumberFormatter::format(constellation) + ", " +
            Poco::NumberFormatter::format(transmission_mode) + ", " +
            Poco::NumberFormatter::format(guard_interval) + ", " +
            Poco::NumberFormatter::format(hierarchy_information));

    _bandwidth = bandwidth;
    _code_rate_HP = code_rate_HP;
    _code_rate_LP = code_rate_LP;
    _constellation = constellation;
    _transmission_mode = transmission_mode;
    _guard_interval = guard_interval;
    _hierarchy_information = hierarchy_information;
}


void
TerrestrialTransponder::readXml(Poco::XML::Node* pXmlTransponder)
{
    LOG(dvb, debug, "read terrestrial transponder ...");

    Transponder::readXml(pXmlTransponder);

    if (pXmlTransponder->hasChildNodes()) {
        Poco::XML::Node* pXmlParam = pXmlTransponder->firstChild();
        while (pXmlParam) {
            Poco::XML::Node* pXmlParamVal = pXmlParam->firstChild();
            if (!pXmlParamVal) {
                LOG(dvb, error, "dvb terrestrial transponder parameter without value: " + pXmlParam->nodeName());
                pXmlParam = pXmlParam->nextSibling();
                continue;
            }
            if (pXmlParam->nodeName() == "service") {
                // do nothing
            }
            else if (pXmlParam->nodeName() == "bandwidth") {
                _bandwidth = bandwidthFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "codeRateHP") {
                _code_rate_HP = coderateFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "codeRateLP") {
                _code_rate_LP = coderateFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "constellation") {
                _constellation = modulationFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "transmissionMode") {
                _transmission_mode = transmitModeFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "guardInterval") {
                _guard_interval = guard_intervalFromString(pXmlParamVal->innerText());
            }
            else if (pXmlParam->nodeName() == "hierarchyInformation") {
                _hierarchy_information = hierarchyFromString(pXmlParamVal->innerText());
            }
            else {
                LOG(dvb, error, "dvb terrestrial transponder unknown parameter: " + pXmlParam->nodeName());
            }

            pXmlParam = pXmlParam->nextSibling();
        }
    }
    else {
        LOG(dvb, error, "dvb terrestrial transponder description contains no tuning parameters");
        return;
    }

    LOG(dvb, debug, "read terrestrial transponder.");
}


void
TerrestrialTransponder::writeXml(Poco::XML::Element* pFrontend)
{
    LOG(dvb, debug, "write terrestrial transponder ...");

    Transponder::writeXml(pFrontend);
    Poco::XML::Document* pDoc = pFrontend->ownerDocument();

    Poco::AutoPtr<Poco::XML::Element> pBandwidth = pDoc->createElement("bandwidth");
    Poco::AutoPtr<Poco::XML::Text> pBandwidthVal = pDoc->createTextNode(bandwidthToString(_bandwidth));
    pBandwidth->appendChild(pBandwidthVal);
    _pXmlTransponder->appendChild(pBandwidth);

    Poco::AutoPtr<Poco::XML::Element> pCodeRateHP = pDoc->createElement("codeRateHP");
    Poco::AutoPtr<Poco::XML::Text> pCodeRateHPVal = pDoc->createTextNode(coderateToString(_code_rate_HP));
    pCodeRateHP->appendChild(pCodeRateHPVal);
    _pXmlTransponder->appendChild(pCodeRateHP);

    Poco::AutoPtr<Poco::XML::Element> pCodeRateLP = pDoc->createElement("codeRateLP");
    Poco::AutoPtr<Poco::XML::Text> pCodeRateLPVal = pDoc->createTextNode(coderateToString(_code_rate_LP));
    pCodeRateLP->appendChild(pCodeRateLPVal);
    _pXmlTransponder->appendChild(pCodeRateLP);

    Poco::AutoPtr<Poco::XML::Element> pConstellation = pDoc->createElement("constellation");
    Poco::AutoPtr<Poco::XML::Text> pConstellationVal = pDoc->createTextNode(modulationToString(_constellation));
    pConstellation->appendChild(pConstellationVal);
    _pXmlTransponder->appendChild(pConstellation);

    Poco::AutoPtr<Poco::XML::Element> pTransmissionMode = pDoc->createElement("transmissionMode");
    Poco::AutoPtr<Poco::XML::Text> pTransmissionModeVal = pDoc->createTextNode(transmitModeToString(_transmission_mode));
    pTransmissionMode->appendChild(pTransmissionModeVal);
    _pXmlTransponder->appendChild(pTransmissionMode);

    Poco::AutoPtr<Poco::XML::Element> pGuardInterval = pDoc->createElement("guardInterval");
    Poco::AutoPtr<Poco::XML::Text> pGuardIntervalVal = pDoc->createTextNode(guard_intervalToString(_guard_interval));
    pGuardInterval->appendChild(pGuardIntervalVal);
    _pXmlTransponder->appendChild(pGuardInterval);

    Poco::AutoPtr<Poco::XML::Element> pHierarchyInformation = pDoc->createElement("hierarchyInformation");
    Poco::AutoPtr<Poco::XML::Text> pHierarchyInformationVal = pDoc->createTextNode(hierarchyToString(_hierarchy_information));
    pHierarchyInformation->appendChild(pHierarchyInformationVal);
    _pXmlTransponder->appendChild(pHierarchyInformation);

    LOG(dvb, debug, "wrote terrestrial transponder.");
}


bool
TerrestrialTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "T" || params.count() < 9) {
        LOG(dvb, error, "invalid parameter data for terrestrial transponder.");
        return false;
    }
    init(bandwidthFromString(params[2]),
        coderateFromString(params[3]),
        coderateFromString(params[4]),
        modulationFromString(params[5]),
        transmitModeFromString(params[6]),
        guard_intervalFromString(params[7]),
        hierarchyFromString(params[8]));
    return true;
}


fe_bandwidth_t
TerrestrialTransponder::bandwidthFromString(const std::string& val)
{
    if (val == BANDWIDTH_8_MHZ) {
        return ::BANDWIDTH_8_MHZ;
    }
    else if (val == BANDWIDTH_7_MHZ) {
        return ::BANDWIDTH_7_MHZ;
    }
    else if (val == BANDWIDTH_6_MHZ) {
        return ::BANDWIDTH_6_MHZ;
    }
    else if (val == BANDWIDTH_AUTO) {
        return ::BANDWIDTH_AUTO;
    }
    else if (val == BANDWIDTH_5_MHZ) {
        return ::BANDWIDTH_5_MHZ;
    }
    else if (val == BANDWIDTH_10_MHZ) {
        return ::BANDWIDTH_10_MHZ;
    }
    else if (val == BANDWIDTH_1_712_MHZ) {
        return ::BANDWIDTH_1_712_MHZ;
    }
}


std::string
TerrestrialTransponder::bandwidthToString(fe_bandwidth_t val)
{
    if (val == ::BANDWIDTH_8_MHZ) {
        return BANDWIDTH_8_MHZ;
    }
    else if (val == ::BANDWIDTH_7_MHZ) {
        return BANDWIDTH_7_MHZ;
    }
    else if (val == ::BANDWIDTH_6_MHZ) {
        return BANDWIDTH_6_MHZ;
    }
    else if (val == ::BANDWIDTH_AUTO) {
        return BANDWIDTH_AUTO;
    }
    else if (val == ::BANDWIDTH_5_MHZ) {
        return BANDWIDTH_5_MHZ;
    }
    else if (val == ::BANDWIDTH_10_MHZ) {
        return BANDWIDTH_10_MHZ;
    }
    else if (val == ::BANDWIDTH_1_712_MHZ) {
        return BANDWIDTH_1_712_MHZ;
    }
}


fe_code_rate_t
TerrestrialTransponder::coderateFromString(const std::string& val)
{
    if (val == FEC_NONE) {
        return ::FEC_NONE;
    }
    else if (val == FEC_1_2) {
        return ::FEC_1_2;
    }
    else if (val == FEC_2_3) {
        return ::FEC_2_3;
    }
    else if (val == FEC_3_4) {
        return ::FEC_3_4;
    }
    else if (val == FEC_4_5) {
        return ::FEC_4_5;
    }
    else if (val == FEC_5_6) {
        return ::FEC_5_6;
    }
    else if (val == FEC_6_7) {
        return ::FEC_6_7;
    }
    else if (val == FEC_7_8) {
        return ::FEC_7_8;
    }
    else if (val == FEC_8_9) {
        return ::FEC_8_9;
    }
    else if (val == FEC_AUTO) {
        return ::FEC_AUTO;
    }
    else if (val == FEC_3_5) {
        return ::FEC_3_5;
    }
    else if (val == FEC_9_10) {
        return ::FEC_9_10;
    }
}


std::string
TerrestrialTransponder::coderateToString(fe_code_rate_t val)
{
    if (val == ::FEC_NONE) {
        return FEC_NONE;
    }
    else if (val == ::FEC_1_2) {
        return FEC_1_2;
    }
    else if (val == ::FEC_2_3) {
        return FEC_2_3;
    }
    else if (val == ::FEC_3_4) {
        return FEC_3_4;
    }
    else if (val == ::FEC_4_5) {
        return FEC_4_5;
    }
    else if (val == ::FEC_5_6) {
        return FEC_5_6;
    }
    else if (val == ::FEC_6_7) {
        return FEC_6_7;
    }
    else if (val == ::FEC_7_8) {
        return FEC_7_8;
    }
    else if (val == ::FEC_8_9) {
        return FEC_8_9;
    }
    else if (val == ::FEC_AUTO) {
        return FEC_AUTO;
    }
    else if (val == ::FEC_3_5) {
        return FEC_3_5;
    }
    else if (val == ::FEC_9_10) {
        return FEC_9_10;
    }
}


fe_modulation_t
TerrestrialTransponder::modulationFromString(const std::string& val)
{
    if (val == QPSK) {
        return ::QPSK;
    }
    else if (val == QAM_16) {
        return ::QAM_16;
    }
    else if (val == QAM_32) {
        return ::QAM_32;
    }
    else if (val == QAM_64) {
        return ::QAM_64;
    }
    else if (val == QAM_128) {
        return ::QAM_128;
    }
    else if (val == QAM_256) {
        return ::QAM_256;
    }
    else if (val == QAM_AUTO) {
        return ::QAM_AUTO;
    }
    else if (val == VSB_8) {
        return ::VSB_8;
    }
    else if (val == VSB_16) {
        return ::VSB_16;
    }
    else if (val == PSK_8) {
        return ::PSK_8;
    }
    else if (val == APSK_16) {
        return ::APSK_16;
    }
    else if (val == APSK_32) {
        return ::APSK_32;
    }
    else if (val == DQPSK) {
        return ::DQPSK;
    }
}


std::string
TerrestrialTransponder::modulationToString(fe_modulation_t val)
{
    if (val == ::QPSK) {
        return QPSK;
    }
    else if (val == ::QAM_16) {
        return QAM_16;
    }
    if (val == ::QAM_32) {
        return QAM_32;
    }
    else if (val == ::QAM_64) {
        return QAM_64;
    }
    if (val == ::QAM_128) {
        return QAM_128;
    }
    else if (val == ::QAM_256) {
        return QAM_256;
    }
    if (val == ::QAM_AUTO) {
        return QAM_AUTO;
    }
    else if (val == ::VSB_8) {
        return VSB_8;
    }
    if (val == ::VSB_16) {
        return VSB_16;
    }
    else if (val == ::PSK_8) {
        return PSK_8;
    }
    if (val == ::APSK_16) {
        return APSK_16;
    }
    else if (val == ::APSK_32) {
        return APSK_32;
    }
    if (val == ::DQPSK) {
        return DQPSK;
    }
}


fe_transmit_mode_t
TerrestrialTransponder::transmitModeFromString(const std::string& val)
{
    if (val == TRANSMISSION_MODE_2K) {
        return ::TRANSMISSION_MODE_2K;
    }
    else if (val == TRANSMISSION_MODE_8K) {
        return ::TRANSMISSION_MODE_8K;
    }
    else if (val == TRANSMISSION_MODE_AUTO) {
        return ::TRANSMISSION_MODE_AUTO;
    }
    else if (val == TRANSMISSION_MODE_4K) {
        return ::TRANSMISSION_MODE_4K;
    }
    else if (val == TRANSMISSION_MODE_1K) {
        return ::TRANSMISSION_MODE_1K;
    }
    else if (val == TRANSMISSION_MODE_16K) {
        return ::TRANSMISSION_MODE_16K;
    }
    else if (val == TRANSMISSION_MODE_32K) {
        return ::TRANSMISSION_MODE_32K;
    }
}


std::string
TerrestrialTransponder::transmitModeToString(fe_transmit_mode_t val)
{
    if (val == ::TRANSMISSION_MODE_2K) {
        return TRANSMISSION_MODE_2K;
    }
    else if (val == ::TRANSMISSION_MODE_8K) {
        return TRANSMISSION_MODE_8K;
    }
    else if (val == ::TRANSMISSION_MODE_AUTO) {
        return TRANSMISSION_MODE_AUTO;
    }
    else if (val == ::TRANSMISSION_MODE_4K) {
        return TRANSMISSION_MODE_4K;
    }
    else if (val == ::TRANSMISSION_MODE_1K) {
        return TRANSMISSION_MODE_1K;
    }
    else if (val == ::TRANSMISSION_MODE_16K) {
        return TRANSMISSION_MODE_16K;
    }
    else if (val == ::TRANSMISSION_MODE_32K) {
        return TRANSMISSION_MODE_32K;
    }
}


fe_guard_interval_t
TerrestrialTransponder::guard_intervalFromString(const std::string& val)
{
    if (val == GUARD_INTERVAL_1_32) {
        return ::GUARD_INTERVAL_1_32;
    }
    else if (val == GUARD_INTERVAL_1_16) {
        return ::GUARD_INTERVAL_1_16;
    }
    else if (val == GUARD_INTERVAL_1_8) {
        return ::GUARD_INTERVAL_1_8;
    }
    else if (val == GUARD_INTERVAL_1_4) {
        return ::GUARD_INTERVAL_1_4;
    }
    else if (val == GUARD_INTERVAL_AUTO) {
        return ::GUARD_INTERVAL_AUTO;
    }
    else if (val == GUARD_INTERVAL_1_128) {
        return ::GUARD_INTERVAL_1_128;
    }
    else if (val == GUARD_INTERVAL_19_128) {
        return ::GUARD_INTERVAL_19_128;
    }
    else if (val == GUARD_INTERVAL_19_256) {
        return ::GUARD_INTERVAL_19_256;
    }

}


std::string
TerrestrialTransponder::guard_intervalToString(fe_guard_interval_t val)
{
    if (val == ::GUARD_INTERVAL_1_32) {
        return GUARD_INTERVAL_1_32;
    }
    else if (val == ::GUARD_INTERVAL_1_16) {
        return GUARD_INTERVAL_1_16;
    }
    else if (val == ::GUARD_INTERVAL_1_8) {
        return GUARD_INTERVAL_1_8;
    }
    else if (val == ::GUARD_INTERVAL_1_4) {
        return GUARD_INTERVAL_1_4;
    }
    else if (val == ::GUARD_INTERVAL_AUTO) {
        return GUARD_INTERVAL_AUTO;
    }
    else if (val == ::GUARD_INTERVAL_1_128) {
        return GUARD_INTERVAL_1_128;
    }
    else if (val == ::GUARD_INTERVAL_19_128) {
        return GUARD_INTERVAL_19_128;
    }
    else if (val == ::GUARD_INTERVAL_19_256) {
        return GUARD_INTERVAL_19_256;
    }
}


fe_hierarchy_t
TerrestrialTransponder::hierarchyFromString(const std::string& val)
{
    if (val == HIERARCHY_NONE) {
        return ::HIERARCHY_NONE;
    }
    else if (val == HIERARCHY_1) {
        return ::HIERARCHY_1;
    }
    else if (val == HIERARCHY_2) {
        return ::HIERARCHY_2;
    }
    else if (val == HIERARCHY_4) {
        return ::HIERARCHY_4;
    }
    else if (val == HIERARCHY_AUTO) {
        return ::HIERARCHY_AUTO;
    }
}


std::string
TerrestrialTransponder::hierarchyToString(fe_hierarchy_t val)
{
    if (val == ::HIERARCHY_NONE) {
        return HIERARCHY_NONE;
    }
    else if (val == ::HIERARCHY_1) {
        return HIERARCHY_1;
    }
    else if (val == ::HIERARCHY_2) {
        return HIERARCHY_2;
    }
    else if (val == ::HIERARCHY_4) {
        return HIERARCHY_4;
    }
    else if (val == ::HIERARCHY_AUTO) {
        return HIERARCHY_AUTO;
    }
}


CableTransponder::CableTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid) :
Transponder(pFrontend, freq, tsid)
{

}


bool
CableTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "C") {
        LOG(dvb, error, "invalid parameter data for cable transponder.");
        return false;
    }
    return true;
}


AtscTransponder::AtscTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid) :
Transponder(pFrontend, freq, tsid)
{

}


bool
AtscTransponder::initTransponder(Poco::StringTokenizer& params)
{
    if (params[0] != "A") {
        LOG(dvb, error, "invalid parameter data for atsc transponder.");
        return false;
    }
    return true;
}

}  // namespace Omm
}  // namespace Dvb
