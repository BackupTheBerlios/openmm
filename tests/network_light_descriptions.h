#ifndef NETWORK_LIGHT_DESCRIPTIONS_H
#define NETWORK_LIGHT_DESCRIPTIONS_H

std::string NetworkLight::m_deviceDescription = 
"<?xml version=\"1.0\"?>\
<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\
<specVersion>\
<major>1</major>\
<minor>0</minor>\
</specVersion>\
<device>\
<deviceType>urn:schemas-upnp-org:device:DimmableLight:1</deviceType>\
<friendlyName>Jamm Network Light</friendlyName>\
<manufacturer>Jamm Developers</manufacturer>\
<manufacturerURL>http://hakker.de/jamm</manufacturerURL>\
<modelDescription>Software emulated UPnP light bulb</modelDescription>\
<modelName>Network Light</modelName>\
<modelNumber>0.1</modelNumber>\
<modelURL>http://hakker.de/jamm</modelURL>\
<serialNumber>0.1</serialNumber>\
<UDN>uuid:23b0189c-549f-11dc-a7c7-001641597c49</UDN>\
<serviceList>\
<service>\
<serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>\
<serviceId>urn:upnp-org:serviceId:SwitchPower:1</serviceId>\
<SCPDURL>/SwitchPower-scpd.xml</SCPDURL>\
<controlURL>/SwitchPower/Control</controlURL>\
<eventSubURL>/SwitchPower/Events</eventSubURL>\
</service>\
<service>\
<serviceType>urn:schemas-upnp-org:service:Dimming:1</serviceType>\
<serviceId>urn:upnp-org:serviceId:Dimming:1</serviceId>\
<SCPDURL>/Dimming-scpd.xml</SCPDURL>\
<controlURL>/Dimming/Control</controlURL>\
<eventSubURL>/Dimming/Events</eventSubURL>\
</service>\
</serviceList>\
<presentationURL></presentationURL>\
</device>\
</root>";

std::string SwitchPower::m_description = 
"<?xml version=\"1.0\"?>\
<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\
<specVersion>\
<major>1</major>\
<minor>0</minor>\
</specVersion>\
\
<actionList>\
<action>\
<name>SetTarget</name>\
<argumentList>\
<argument>\
<name>NewTargetValue</name>\
<relatedStateVariable>Target</relatedStateVariable>\
<direction>in</direction>\
</argument>\
</argumentList>\
</action>\
\
<action>\
<name>GetTarget</name>\
<argumentList>\
<argument>\
<name>RetTargetValue</name>\
<relatedStateVariable>Target</relatedStateVariable>\
<direction>out</direction>\
</argument>\
</argumentList>\
</action>\
\
<action>\
<name>GetStatus</name>\
<argumentList>\
<argument>\
<name>ResultStatus</name>\
<relatedStateVariable>Status</relatedStateVariable>\
<direction>out</direction>\
</argument>\
</argumentList>\
</action>\
</actionList>\
\
<serviceStateTable>\
<stateVariable sendEvents=\"no\">\
<name>Target</name>\
<dataType>boolean</dataType>\
<defaultValue>0</defaultValue>\
</stateVariable>\
\
<stateVariable sendEvents=\"yes\">\
<name>Status</name>\
<dataType>boolean</dataType>\
<defaultValue>0</defaultValue>\
</stateVariable>\
</serviceStateTable>\
\
</scpd>";

std::string Dimming::m_description = 
"<?xml version=\"1.0\"?>\
<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\
<specVersion>\
<major>1</major>\
<minor>0</minor>\
</specVersion>\
\
<actionList>\
<action>\
<name>SetLoadLevelTarget</name>\
<argumentList>\
<argument>\
<name>newLoadlevelTarget</name>\
<direction>in</direction>\
<relatedStateVariable>LoadLevelTarget</relatedStateVariable>\
</argument>\
</argumentList>\
</action>\
\
<action>\
<name>GetLoadLevelTarget</name>\
<argumentList>\
<argument>\
<name>retLoadlevelTarget</name>\
<direction>out</direction>\
<retval />\
<relatedStateVariable>LoadLevelTarget</relatedStateVariable>\
</argument>\
</argumentList>\
</action>\
\
<action>\
<name>GetLoadLevelStatus</name>\
<argumentList>\
<argument>\
<name>retLoadlevelStatus</name>\
<direction>out</direction>\
<retval />\
<relatedStateVariable>LoadLevelStatus</relatedStateVariable>\
</argument>\
</argumentList>\
</action>\
</actionList>\
\
<serviceStateTable>\
<stateVariable sendEvents=\"no\">\
<name>LoadLevelTarget</name>\
<dataType>ui1</dataType>\
<defaultValue>0</defaultValue>\
<allowedValueRange>\
<minimum>0</minimum>\
<maximum>100</maximum>\
</allowedValueRange>\
</stateVariable>\
\
<stateVariable sendEvents=\"yes\">\
<name>LoadLevelStatus</name>\
<dataType>ui1</dataType>\
<defaultValue>0</defaultValue>\
<allowedValueRange>\
<minimum>0</minimum>\
<maximum>100</maximum>\
</allowedValueRange>\
</stateVariable>\
</serviceStateTable>\
</scpd>";

#endif
