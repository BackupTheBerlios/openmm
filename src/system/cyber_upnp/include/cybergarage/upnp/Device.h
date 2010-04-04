/******************************************************************
*
*	CyberLink for C++
*
*	Copyright (C) Satoshi Konno 2002-2003
*
*	File: Device.h
*
*	Revision;
*
*	07/10/03
*		- first revision
*
******************************************************************/

#ifndef _CLINK_DEVICE_H_
#define _CLINK_DEVICE_H_

#include <cybergarage/http/HTTPRequestListener.h>
#include <cybergarage/util/StringUtil.h>
#include <cybergarage/util/TimeUtil.h>
#include <cybergarage/xml/Node.h>
#include <cybergarage/io/File.h>
#include <cybergarage/upnp/UPnP.h>
#include <cybergarage/upnp/Device.h>
#include <cybergarage/upnp/DeviceList.h>
#include <cybergarage/upnp/Service.h>
#include <cybergarage/upnp/ServiceList.h>
#include <cybergarage/upnp/Action.h>
#include <cybergarage/upnp/StateVariable.h>
#include <cybergarage/upnp/IconList.h>
#include <cybergarage/upnp/ssdp/SSDPResponse.h>
#include <cybergarage/upnp/device/SearchListener.h>
#include <cybergarage/upnp/device/InvalidDescriptionException.h>
#include <cybergarage/upnp/xml/DeviceData.h>
#include <cybergarage/upnp/event/SubscriptionRequest.h>
#include <cybergarage/upnp/event/SubscriptionResponse.h>
#include <cybergarage/upnp/device/Advertiser.h>
#include <cybergarage/util/Mutex.h>

#include <sstream>

namespace CyberLink {

class Device : public CyberHTTP::HTTPRequestListener, public SearchListener
{
	CyberXML::Node *rootNode;
	CyberXML::Node *deviceNode;

	Device *rootDevice;
	Device *parentDevice;

	DeviceList deviceList;
	ServiceList serviceList;
	IconList iconList;

	bool localRootDeviceFlag;

	std::string devUUID;

	CyberUtil::Mutex mutex;

	bool wirelessMode;

	////////////////////////////////////////////////
	//	Constants
	////////////////////////////////////////////////
	
public:

	static const char *ELEM_NAME;
	static const char *UPNP_ROOTDEVICE;
	static const int DEFAULT_STARTUP_WAIT_TIME;
	static const int DEFAULT_DISCOVERY_WAIT_TIME;
	static const int DEFAULT_LEASE_TIME;
	static const int HTTP_DEFAULT_PORT;
	static const char *DEFAULT_DESCRIPTION_URI;
	static const char *URLBASE_NAME;
	static const char *DEVICE_TYPE;
	static const char *FRIENDLY_NAME;
	static const char *MANUFACTURE;
	static const char *MANUFACTURE_URL;
	static const char *MODEL_DESCRIPTION;
	static const char *MODEL_NAME;
	static const char *MODEL_NUMBER;
	static const char *MODEL_URL;
	static const char *SERIAL_NUMBER;
	static const char *UDN;
	static const char *UPC;
	static const char *presentationURL;
	
	////////////////////////////////////////////////
	//	Member
	////////////////////////////////////////////////

public:

	CyberXML::Node *getRootNode();

	CyberXML::Node *getDeviceNode()
	{
		return deviceNode;
	}

	void setRootNode(CyberXML::Node *node)
	{
		rootNode = node;
	}

	void setDeviceNode(CyberXML::Node *node)
	{
		deviceNode = node;
	}
				
	////////////////////////////////////////////////
	//	Constructor
	////////////////////////////////////////////////

	Device();
	Device(CyberXML::Node *root, CyberXML::Node *device);
	Device(CyberXML::Node *device);
#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
	Device(CyberIO::File *descriptionFile);
	Device(const char *descriptionFileName);
#endif

private:

	void initUUID();
	void initDeviceData();
	void initChildList();

public:

	virtual ~Device();

	////////////////////////////////////////////////
	// Mutex
	////////////////////////////////////////////////
	
public:

	void lock()
	{
		mutex.lock();
	}
	
	void unlock()
	{
		mutex.unlock();
	}

	////////////////////////////////////////////////
	//	NMPR
	////////////////////////////////////////////////

public:

	void setNMPRMode(bool flag);
	bool isNMPRMode();

	////////////////////////////////////////////////
	//	Wireless
	////////////////////////////////////////////////
	
public:

	void setWirelessMode(bool flag)
	{
		wirelessMode = flag;
	}

	bool isWirelessMode()
	{
		return wirelessMode;
	}
	
	int getSSDPAnnounceCount()
	{
		if (isNMPRMode() == true && isWirelessMode() == true)
			return UPnP::INMPR03_DISCOVERY_OVER_WIRELESS_COUNT;
		return 1;
	}

	////////////////////////////////////////////////
	//	Device UUID
	////////////////////////////////////////////////

private:

	void setUUID(const char *uuid)
	{
		devUUID = uuid;
	}

	const char *getUUID()
	{
		return devUUID.c_str();
	}

	void updateUDN()
	{
		std::string uuid;
		uuid.append("uuid:");
		uuid.append(getUUID());
		setUDN(uuid.c_str());
	}
	
	//////////////////////////////////////////////
	//	Root Device
	////////////////////////////////////////////////

private:

	void setLocalRootDeviceFlag(bool flag)
	{
		localRootDeviceFlag = flag;
	}

	bool isLocalRootDevice()
	{
		return localRootDeviceFlag;
	}

	 ////////////////////////////////////////////////
	//	Root Device
	////////////////////////////////////////////////

public:

	Device *getRootDevice();

	 ////////////////////////////////////////////////
	//	Parent Device
	////////////////////////////////////////////////

public:

	Device *getParentDevice();

	////////////////////////////////////////////////
	//	UserData
	////////////////////////////////////////////////

public:
	
	DeviceData *getDeviceData();

	////////////////////////////////////////////////
	//	Description
	////////////////////////////////////////////////

public:

	void setDescriptionFile(const char *file)
	{
		getDeviceData()->setDescriptionFile(file);
	}

	const char *getDescriptionFile()
	{
		return getDeviceData()->getDescriptionFile();
	}

	void setDescriptionURI(const char *uri)
	{
		getDeviceData()->setDescriptionURI(uri);
	}

	const char *getDescriptionURI()
	{
		return getDeviceData()->getDescriptionURI();
	}

	bool isDescriptionURI(const char *uri)
	{
		const char *descriptionURI = getDescriptionURI();
		if (uri == NULL || descriptionURI == NULL)
			return false;
		CyberUtil::String descriptionURIStr = descriptionURI;
		return descriptionURIStr.equals(uri);
	}

	const char *getDescriptionFilePath(std::string &buf)
	{
		const char *descriptionFileName = getDescriptionFile();
		if (descriptionFileName == NULL)
			return "";
		CyberIO::File descriptionFile(descriptionFileName);
		buf = descriptionFile.getAbsoluteFile()->getParent();
		return buf.c_str();
	}

	bool loadDescription(const char *descString);
#if !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) 
	bool loadDescription(CyberIO::File *file);
#endif

private:

	bool initializeLoadedDescription();

	////////////////////////////////////////////////
	//	isDeviceNode
	////////////////////////////////////////////////

public:

	static bool isDeviceNode(CyberXML::Node *node)
	{
		return node->isName(Device::ELEM_NAME);
	}
	
	////////////////////////////////////////////////
	//	Root Device
	////////////////////////////////////////////////

	bool isRootDevice()
	{
		return (getRootNode() != NULL) ? true : false;
	}
	
	////////////////////////////////////////////////
	//	Root Device
	////////////////////////////////////////////////

	void setSSDPPacket(SSDPPacket *packet)
	{
		getDeviceData()->setSSDPPacket(packet);
	}

	SSDPPacket *getSSDPPacket()
	{
		if (isRootDevice() == false)
			return NULL;
		return getDeviceData()->getSSDPPacket();
	}
	
	////////////////////////////////////////////////
	//	Location 
	////////////////////////////////////////////////

	void setLocation(const char *value)
	{
		getDeviceData()->setLocation(value);
	}

	const char *getLocation()
	{
		SSDPPacket *packet = getSSDPPacket();
		if (packet != NULL) {
			std::string buf;
			setLocation(packet->getLocation(buf));
		}
		return getDeviceData()->getLocation();
	}

	////////////////////////////////////////////////
	//	LeaseTime 
	////////////////////////////////////////////////

public:

	void setLeaseTime(int value);
	int getLeaseTime();

	////////////////////////////////////////////////
	//	TimeStamp 
	////////////////////////////////////////////////

public:

	long getTimeStamp();
	long getElapsedTime()
	{
		return time(NULL) - getTimeStamp();
	}

	bool isExpired();
	
	////////////////////////////////////////////////
	//	URL Base
	////////////////////////////////////////////////

private:

	void setURLBase(const char *value);

	void updateURLBase(const char *host);

public:

	const char *getURLBase();

	////////////////////////////////////////////////
	//	deviceType
	////////////////////////////////////////////////

	void setDeviceType(const char *value)
	{
		getDeviceNode()->setNode(DEVICE_TYPE, value);
	}

	const char *getDeviceType()
	{
		return getDeviceNode()->getNodeValue(DEVICE_TYPE);
	}

	bool isDeviceType(const char *value);

	////////////////////////////////////////////////
	//	friendlyName
	////////////////////////////////////////////////

	void setFriendlyName(const char *value)
	{
		getDeviceNode()->setNode(FRIENDLY_NAME, value);
	}

	const char *getFriendlyName()
	{
		return getDeviceNode()->getNodeValue(FRIENDLY_NAME);
	}

	////////////////////////////////////////////////
	//	manufacture
	////////////////////////////////////////////////
	
	void setManufacture(const char *value)
	{
		getDeviceNode()->setNode(MANUFACTURE, value);
	}

	const char *getManufacture()
	{
		return getDeviceNode()->getNodeValue(MANUFACTURE);
	}

	////////////////////////////////////////////////
	//	manufactureURL
	////////////////////////////////////////////////

	void setManufactureURL(const char *value)
	{
		getDeviceNode()->setNode(MANUFACTURE_URL, value);
	}

	const char *getManufactureURL()
	{
		return getDeviceNode()->getNodeValue(MANUFACTURE_URL);
	}

	////////////////////////////////////////////////
	//	modelDescription
	////////////////////////////////////////////////

	void setModelDescription(const char *value)
	{
		getDeviceNode()->setNode(MODEL_DESCRIPTION, value);
	}

	const char *getModelDescription()
	{
		return getDeviceNode()->getNodeValue(MODEL_DESCRIPTION);
	}

	////////////////////////////////////////////////
	//	modelName
	////////////////////////////////////////////////

	void setModelName(const char *value)
	{
		getDeviceNode()->setNode(MODEL_NAME, value);
	}

	const char *getModelName()
	{
		return getDeviceNode()->getNodeValue(MODEL_NAME);
	}

	////////////////////////////////////////////////
	//	modelNumber
	////////////////////////////////////////////////

	void setModelNumber(const char *value)
	{
		getDeviceNode()->setNode(MODEL_NUMBER, value);
	}

	const char *getModelNumber()
	{
		return getDeviceNode()->getNodeValue(MODEL_NUMBER);
	}

	////////////////////////////////////////////////
	//	modelURL
	////////////////////////////////////////////////

	void setModelURL(const char *value)
	{
		getDeviceNode()->setNode(MODEL_URL, value);
	}

	const char *getModelURL()
	{
		return getDeviceNode()->getNodeValue(MODEL_URL);
	}

	////////////////////////////////////////////////
	//	serialNumber
	////////////////////////////////////////////////

	void setSerialNumber(const char *value)
	{
		getDeviceNode()->setNode(SERIAL_NUMBER, value);
	}

	const char *getSerialNumber()
	{
		return getDeviceNode()->getNodeValue(SERIAL_NUMBER);
	}

	////////////////////////////////////////////////
	//	UDN
	////////////////////////////////////////////////

	void setUDN(const char *value)
	{
		getDeviceNode()->setNode(UDN, value);
	}

	const char *getUDN()
	{
		return getDeviceNode()->getNodeValue(UDN);
	}

	bool hasUDN()
	{
		const char *udn = getUDN();
		if (udn == NULL)
			return false;
		std::string udnStr = udn;
		if (udnStr.length() <= 0)
			return false;
		return true;
	}

	////////////////////////////////////////////////
	//	UPC
	////////////////////////////////////////////////

	void setUPC(const char *value)
	{
		getDeviceNode()->setNode(UPC, value);
	}

	const char *getUPC()
	{
		return getDeviceNode()->getNodeValue(UPC);
	}

	////////////////////////////////////////////////
	//	presentationURL
	////////////////////////////////////////////////

	void setPresentationURL(const char *value)
	{
		getDeviceNode()->setNode(presentationURL, value);
	}

	const char *getPresentationURL()
	{
		return getDeviceNode()->getNodeValue(presentationURL);
	}

	////////////////////////////////////////////////
	//	deviceList
	////////////////////////////////////////////////

private:

	void initDeviceList();

public:

	DeviceList *getDeviceList()
	{
		return &deviceList;
	}

	bool isDevice(const char *name);
	
	Device *getDevice(const char *name);
	Device *getDeviceByDescriptionURI(const char *uri);
	
	////////////////////////////////////////////////
	//	serviceList
	////////////////////////////////////////////////

private:

	void initServiceList();

public:

	ServiceList *getServiceList()
	{
		return &serviceList;
	}

	Service *getService(const char *name);
	Service *getSubscriberService(const char *uuid);

public:

	Service *getServiceBySCPDURL(const char *searchUrl);
	Service *getServiceByControlURL(const char *searchUrl);
	Service *getServiceByEventSubURL(const char *searchUrl);

	////////////////////////////////////////////////
	//	StateVariable
	////////////////////////////////////////////////

public:
	
	StateVariable *getStateVariable(const char *serviceType, const char *name);
	StateVariable *getStateVariable(const char *name);

	////////////////////////////////////////////////
	//	Action
	////////////////////////////////////////////////

public:

	Action *getAction(const char *name);

	////////////////////////////////////////////////
	//	iconList
	////////////////////////////////////////////////

private:

	void initIconList();

public:

	IconList *getIconList()
	{
		return &iconList;
	}
	
	Icon *getIcon(int n)
	{
		IconList *iconList = getIconList();
		if (n < 0 && ((int)iconList->size()-1) < n)
			return NULL;
		return iconList->getIcon(n);
	}

	////////////////////////////////////////////////
	//	Notify
	////////////////////////////////////////////////

public:

	const char *getLocationURL(const char *host, std::string &buf);

private:

	const char *getNotifyDeviceNT(std::string &buf);
	const char *getNotifyDeviceUSN(std::string &buf);
	const char *getNotifyDeviceTypeNT(std::string &buf);
	const char *getNotifyDeviceTypeUSN(std::string &buf);

public:

	static void notifyWait();

	void announce(const char *bindAddr);
	void announce();
	
	void byebye(const char *bindAddr);
	void byebye();

	////////////////////////////////////////////////
	//	Search
	////////////////////////////////////////////////

public:

	bool postSearchResponse(SSDPPacket *ssdpPacket, const char *st, const char *usn);
	void deviceSearchResponse(SSDPPacket *ssdpPacket);
	void deviceSearchReceived(SSDPPacket *ssdpPacket);

	////////////////////////////////////////////////
	//	HTTP Server	
	////////////////////////////////////////////////

public:

	void setHTTPPort(int port)
	{
		getDeviceData()->setHTTPPort(port);
	}
	
	int getHTTPPort()
	{
		return getDeviceData()->getHTTPPort();
	}

	void httpRequestRecieved(CyberHTTP::HTTPRequest *httpReq);

private:

	const char *getDescriptionData(const char *host, std::string &buf);
	void httpGetRequestRecieved(CyberHTTP::HTTPRequest *httpReq);
	void httpPostRequestRecieved(CyberHTTP::HTTPRequest *httpReq);

	////////////////////////////////////////////////
	//	SOAP
	////////////////////////////////////////////////

private:

	void soapBadActionRecieved(CyberHTTP::HTTPRequest *soapReq);
	void soapActionRecieved(CyberHTTP::HTTPRequest *soapReq);

	////////////////////////////////////////////////
	//	controlAction
	////////////////////////////////////////////////

private:

	void invalidActionControlRecieved(ControlRequest *ctlReq);
	void deviceControlRequestRecieved(ControlRequest *ctlReq, Service *service);
	void deviceActionControlRecieved(ActionRequest *ctlReq, Service *service);
	void deviceQueryControlRecieved(QueryRequest *ctlReq, Service *service);

	////////////////////////////////////////////////
	//	eventSubscribe
	////////////////////////////////////////////////

private:

	void upnpBadSubscriptionRecieved(SubscriptionRequest *subReq, int code);
	void deviceEventSubscriptionRecieved(SubscriptionRequest *subReq);
	void deviceEventNewSubscriptionRecieved(Service *service, SubscriptionRequest *subReq);
	void deviceEventRenewSubscriptionRecieved(Service *service, SubscriptionRequest *subReq);
	void deviceEventUnsubscriptionRecieved(Service *service, SubscriptionRequest *subReq);
	
	////////////////////////////////////////////////
	//	Thread	
	////////////////////////////////////////////////

public:

	CyberHTTP::HTTPServerList *getHTTPServerList() 
	{
		return getDeviceData()->getHTTPServerList();
	}

	SSDPSearchSocketList *getSSDPSearchSocketList() 
	{
		return getDeviceData()->getSSDPSearchSocketList();
	}

	void setAdvertiser(Advertiser *adv)
	{
		getDeviceData()->setAdvertiser(adv);
	}

	Advertiser *getAdvertiser()
	{
		return getDeviceData()->getAdvertiser();
	}

public:
	
	bool start();
	bool stop()
	{
		return stop(true);
	}

private:
	
	bool stop(bool doByeBye);

	////////////////////////////////////////////////
	// Interface Address
	////////////////////////////////////////////////

public:

	const char *getInterfaceAddress() 
	{
		SSDPPacket *ssdpPacket = getSSDPPacket();
		if (ssdpPacket == NULL)
			return "";
		return ssdpPacket->getLocalAddress();
	}

	////////////////////////////////////////////////
	// AcionListener
	////////////////////////////////////////////////
	
public:
	
	void setActionListener(ActionListener *listener);
	void setQueryListener(QueryListener *listener);

	////////////////////////////////////////////////
	// AcionListener (includeSubDevices)
	////////////////////////////////////////////////
	
public:
	
	void setActionListener(ActionListener *listener, bool includeSubDevices);
	void setQueryListener(QueryListener *listener, bool includeSubDevices);

	////////////////////////////////////////////////
	//	output
	////////////////////////////////////////////////

/*
	public void output(PrintWriter ps) 
	{
		ps.println("deviceType = " + getDeviceType());
		ps.println("freindlyName = " + getFriendlyName());
		ps.println("presentationURL = " + getPresentationURL());

		DeviceList devList = getDeviceList();
		ps.println("devList = " + devList.size());
		
		ServiceList serviceList = getServiceList();
		ps.println("serviceList = " + serviceList.size());

		IconList iconList = getIconList();
		ps.println("iconList = " + iconList.size());
	}

	public void print()
	{
		PrintWriter pr = new PrintWriter(System.out);
		output(pr);
		pr.flush();
	}
*/
};

}

#endif
