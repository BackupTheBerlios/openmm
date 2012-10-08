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

#ifndef Frontend_INCLUDED
#define Frontend_INCLUDED

#include <linux/dvb/frontend.h>

#include <Poco/TextConverter.h>
#include <Poco/TextEncoding.h>
#include <Poco/UTF8Encoding.h>

namespace Omm {
namespace Dvb {

class Adapter;
class Demux;
class Dvr;
class SignalCheckThread;

class Frontend
{
    friend class Device;
    friend class Adapter;
    friend class SignalCheckThread;

public:
    static const std::string Unknown;
    static const std::string DVBS;
    static const std::string DVBT;
    static const std::string DVBC;
    static const std::string ATSC;

    Frontend(Adapter* pAdapter, int num);
    ~Frontend();

    void addTransponder(Transponder* pTransponder);
    void openFrontend();
    void closeFrontend();

    void scan(const std::string& initialTransponderData);
    virtual void readXml(Poco::XML::Node* pXmlFrontend);
    virtual void writeXml(Poco::XML::Element* pAdapter);

    const std::string getType();
    const std::string getName();
    bool typeSupported();
    virtual bool tune(Transponder* pTransponder) {}
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid) {}

    void listInitialTransponderData();
    void getInitialTransponderKeys(const std::string& countryCode, std::vector<std::string>& keys);
    void getInitialTransponderData(const std::string& key);

protected:
    bool waitForLock(Poco::Timestamp::TimeDiff timeout = 0);  // timeout in microseconds, 0 means forever
    bool hasLock();
    bool scanTransponder(Transponder* pTransponder);
    bool scanPatPmt(Transponder* pTransponder);
    void scanSdt(Transponder* pTransponder);
    void scanNit(Transponder* pTransponder, bool actual = false);

    int                         _fileDescFrontend;
    struct dvb_frontend_info    _feInfo;
    Poco::Timestamp::TimeDiff   _frontendTimeout;
    Poco::AutoPtr<Poco::XML::Element>   _pXmlFrontend;

private:
//    void diseqc(unsigned int satNum, Channel::Polarization pol, bool hiBand);
//    bool tuneFrontend(unsigned int freq, unsigned int symbolRate);
    void checkFrontend();
    bool addKnownTransponder(Transponder* pTransponder);
    /// addKownTransponder() returns true if transponder is new (has not been scanned yet)

    Adapter*                    _pAdapter;
    std::string                 _deviceName;
    std::string                 _name;
    int                         _num;
    std::string                 _type;
    std::vector<Transponder*>   _initialTransponders;
    std::vector<Transponder*>   _scannedTransponders;
    std::vector<Transponder*>   _transponders;
    Demux*                      _pDemux;
    Dvr*                        _pDvr;

    Poco::Thread                _t;
    SignalCheckThread*          _pt;
    Poco::FastMutex             _tuneLock;

    Poco::UTF8Encoding          _sourceEncoding;
    Poco::UTF8Encoding          _targetEncoding;
    Poco::TextConverter*        _pTextConverter;
};


class Lnb
{
    friend class Adapter;
    friend class Frontend;

public:
    Lnb(const std::string& desc, unsigned long lowVal, unsigned long highVal, unsigned long switchVal);

    bool isHiBand(unsigned int freq, unsigned int& ifreq);

private:
    std::string         _desc;
    unsigned long	_lowVal;
    unsigned long	_highVal;	// zero indicates no hiband
    unsigned long	_switchVal;	// zero indicates no hiband
};


class SatFrontend : public Frontend
{
public:
    static const int InvalidSatNum;
    static const int maxSatNum;

    SatFrontend(Adapter* pAdapter, int num);

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid);
    virtual void readXml(Poco::XML::Node* pXmlFrontend);
    virtual void writeXml(Poco::XML::Element* pAdapter);

    int getSatNum(const std::string& orbitalPosition);
    void setSatNum(const std::string& orbitalPosition, int satNum);
    bool isSatNumKnown(int satNum);

private:
    void diseqc(unsigned int satNum, const std::string& polarization, bool hiBand);
    void printSatMap();

    std::map<std::string, Lnb*> _lnbs;  // possible LNB types
    Lnb*                        _pLnb;
    std::map<std::string, int>  _satNumMap;
};


class TerrestrialFrontend : public Frontend
{
public:
    TerrestrialFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid);
};


class CableFrontend : public Frontend
{
public:
    CableFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid);
};


class AtscFrontend : public Frontend
{
public:
    AtscFrontend(Adapter* pAdapter, int num) : Frontend(pAdapter, num) {}

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid);
};

}  // namespace Omm
}  // namespace Dvb

#endif
