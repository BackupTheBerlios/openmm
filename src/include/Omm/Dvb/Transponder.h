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

#ifndef Transponder_INCLUDED
#define Transponder_INCLUDED

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#include <sys/poll.h>

#include <Poco/StringTokenizer.h>
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
#include <Poco/DOM/DocumentFragment.h>

namespace Omm {
namespace Dvb {

class Frontend;

class Transponder
{
    friend class Device;
    friend class Frontend;
    friend class SatFrontend;
    friend class TerrestrialFrontend;
    friend class CableFrontend;
    friend class AtscFrontend;
    friend class Service;

public:
    static const int InvalidTransportStreamId;

    Transponder(Frontend* pFrontend, unsigned int freq, int tsid);

    void addService(Dvb::Service* pService);
    Service* getService(unsigned int serviceId);
    Service* getService(const std::string& serviceName);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

    bool equal(Transponder* pOtherTransponder);

protected:
    virtual bool initTransponder(Poco::StringTokenizer& params) {}

    Frontend*                           _pFrontend;
    std::vector<Dvb::Service*>          _services;
    Poco::AutoPtr<Poco::XML::Element>   _pXmlTransponder;

private:
    unsigned int                        _frequency;
    int                                 _transportStreamId;
};


class SatTransponder : public Transponder
{
    friend class Frontend;
    friend class SatFrontend;

public:
    static const std::string POL_HORIZ;
    static const std::string POL_VERT;
    static const std::string POL_CIRC_LEFT;
    static const std::string POL_CIRC_RIGHT;
    static const std::string MOD_S;
    static const std::string MOD_S2;
    static const std::string MOD_TYPE_AUTO;
    static const std::string MOD_TYPE_QPSK;
    static const std::string MOD_TYPE_8PSK;
    static const std::string MOD_TYPE_16QAM;
    static const std::string FEC_NOT_DEFINED;
    static const std::string FEC_1_2;
    static const std::string FEC_2_3;
    static const std::string FEC_3_4;
    static const std::string FEC_5_6;
    static const std::string FEC_7_8;
    static const std::string FEC_8_9;
    static const std::string FEC_3_5;
    static const std::string FEC_4_5;
    static const std::string FEC_9_10;
    static const std::string FEC_NO_CODING;

    SatTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid);

    void init(const std::string satPosition, unsigned int satNum, unsigned int symbolRate, const std::string& polarization);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

    std::string         _satPosition;
    int                 _satNum;
    unsigned int        _symbolRate;
    std::string         _polarization;
};


class TerrestrialTransponder : public Transponder
{
    friend class TerrestrialFrontend;

public:
    static const std::string BANDWIDTH_8_MHZ;
    static const std::string BANDWIDTH_7_MHZ;
    static const std::string BANDWIDTH_6_MHZ;
    static const std::string BANDWIDTH_AUTO;
    static const std::string BANDWIDTH_5_MHZ;
    static const std::string BANDWIDTH_10_MHZ;
    static const std::string BANDWIDTH_1_712_MHZ;
    static const std::string PRIORITY_HIGH;
    static const std::string PRIORITY_LOW;
    static const std::string FEC_NONE;
    static const std::string FEC_1_2;
    static const std::string FEC_2_3;
    static const std::string FEC_3_4;
    static const std::string FEC_4_5;
    static const std::string FEC_5_6;
    static const std::string FEC_6_7;
    static const std::string FEC_7_8;
    static const std::string FEC_8_9;
    static const std::string FEC_AUTO;
    static const std::string FEC_3_5;
    static const std::string FEC_9_10;
    static const std::string QPSK;
    static const std::string QAM_16;
    static const std::string QAM_32;
    static const std::string QAM_64;
    static const std::string QAM_128;
    static const std::string QAM_256;
    static const std::string QAM_AUTO;
    static const std::string VSB_8;
    static const std::string VSB_16;
    static const std::string PSK_8;
    static const std::string APSK_16;
    static const std::string APSK_32;
    static const std::string DQPSK;
    static const std::string TRANSMISSION_MODE_2K;
    static const std::string TRANSMISSION_MODE_8K;
    static const std::string TRANSMISSION_MODE_AUTO;
    static const std::string TRANSMISSION_MODE_4K;
    static const std::string TRANSMISSION_MODE_1K;
    static const std::string TRANSMISSION_MODE_16K;
    static const std::string TRANSMISSION_MODE_32K;
    static const std::string GUARD_INTERVAL_1_32;
    static const std::string GUARD_INTERVAL_1_16;
    static const std::string GUARD_INTERVAL_1_8;
    static const std::string GUARD_INTERVAL_1_4;
    static const std::string GUARD_INTERVAL_AUTO;
    static const std::string GUARD_INTERVAL_1_128;
    static const std::string GUARD_INTERVAL_19_128;
    static const std::string GUARD_INTERVAL_19_256;
    static const std::string HIERARCHY_NONE;
    static const std::string HIERARCHY_1;
    static const std::string HIERARCHY_2;
    static const std::string HIERARCHY_4;
    static const std::string HIERARCHY_AUTO;

    TerrestrialTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid);

    void init(fe_bandwidth_t bandwidth,
            fe_code_rate_t code_rate_HP,
            fe_code_rate_t code_rate_LP,
            fe_modulation_t constellation,
            fe_transmit_mode_t transmission_mode,
            fe_guard_interval_t guard_interval,
            fe_hierarchy_t hierarchy_information);

    virtual void readXml(Poco::XML::Node* pXmlTransponder);
    virtual void writeXml(Poco::XML::Element* pFrontend);

    static fe_bandwidth_t bandwidthFromString(const std::string& val);
    static std::string bandwidthToString(fe_bandwidth_t val);
    static fe_code_rate_t coderateFromString(const std::string& val);
    static std::string coderateToString(fe_code_rate_t val);
    static fe_modulation_t modulationFromString(const std::string& val);
    static std::string modulationToString(fe_modulation_t val);
    static fe_transmit_mode_t transmitModeFromString(const std::string& val);
    static std::string transmitModeToString(fe_transmit_mode_t val);
    static fe_guard_interval_t guard_intervalFromString(const std::string& val);
    static std::string guard_intervalToString(fe_guard_interval_t val);
    static fe_hierarchy_t hierarchyFromString(const std::string& val);
    static std::string hierarchyToString(fe_hierarchy_t val);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

    fe_bandwidth_t              _bandwidth;
    fe_code_rate_t              _code_rate_HP;
    fe_code_rate_t              _code_rate_LP;
    fe_modulation_t             _constellation;
    fe_transmit_mode_t          _transmission_mode;
    fe_guard_interval_t         _guard_interval;
    fe_hierarchy_t              _hierarchy_information;
};


class CableTransponder : public Transponder
{
public:
    CableTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid);
//    CableTransponder(unsigned int freq, unsigned int tsid);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

};


class AtscTransponder : public Transponder
{
public:
    AtscTransponder(Frontend* pFrontend, unsigned int freq, unsigned int tsid);
//    AtscTransponder(unsigned int freq, unsigned int tsid);

private:
    virtual bool initTransponder(Poco::StringTokenizer& params);

};


}  // namespace Omm
}  // namespace Dvb

#endif
