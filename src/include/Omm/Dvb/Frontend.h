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
    void readXml(Poco::XML::Node* pXmlFrontend);
    void writeXml(Poco::XML::Element* pAdapter);

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
    void scanTransponder(Transponder* pTransponder);
    void scanPatPmt(Transponder* pTransponder);
    void scanSdt(Transponder* pTransponder);
    void scanNit(Transponder* pTransponder);
//    void scanTransponderOld(Transponder* pTransponder);
//    std::string trim(const std::string& str);
    std::string filter(const std::string& str);

    int                         _fileDescFrontend;
    struct dvb_frontend_info    _feInfo;
    Poco::Timestamp::TimeDiff   _frontendTimeout;

private:
//    void diseqc(unsigned int satNum, Channel::Polarization pol, bool hiBand);
//    bool tuneFrontend(unsigned int freq, unsigned int symbolRate);
    void checkFrontend();
    bool addKownTransponder(Transponder* pTransponder);
    /// addKownTransponder() returns true if transponder is new (has not been scanned yet)

    Adapter*                    _pAdapter;
    std::string                 _deviceName;
    std::string                 _name;
    int                         _num;
    std::string                 _type;
    std::vector<Transponder*>   _initialTransponders;
//    std::vector<Transponder*>   _scannedTransponders;
    std::map<unsigned int, Transponder*>   _scannedTransponders;
//    std::map<Poco::UInt16, std::map<Poco::UInt16, Transponder*> >   _scannedTransponders;
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
    SatFrontend(Adapter* pAdapter, int num);

    virtual bool tune(Transponder* pTransponder);
    virtual Transponder* createTransponder(unsigned int freq, unsigned int tsid);

private:
    void diseqc(unsigned int satNum, const std::string& polarization, bool hiBand);

    std::map<std::string,Lnb*>  _lnbs;  // possible LNB types
    Lnb*                        _pLnb;
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
