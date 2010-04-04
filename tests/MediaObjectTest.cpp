/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010                                                 |
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
#include "UpnpAvTypes.h"
#include "UpnpPrivate.h"

// Content Directory 1, 2.8.3.2. Browsing the root level metadata

std::string ex1 = "\
<?xml version=\"1.0\"?>\
<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \
xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" \
xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">\
<container id=\"0\" parentID=\"-1\" childCount=\"2\" restricted=\"true\" searchable=\"true\">\
<dc:title>My multimedia stuff</dc:title>\
<upnp:class>object.container.storageFolder</upnp:class>\
<upnp:storageUsed>907000</upnp:storageUsed>\
<upnp:writeStatus>WRITABLE</upnp:writeStatus>\
<upnp:searchClass includeDerived=\"false\" >\
object.container.album.musicAlbum\
</upnp:searchClass>\
<upnp:searchClass includeDerived=\"false\" >\
object.container.album.photoAlbum\
</upnp:searchClass>\
<upnp:searchClass includeDerived=\"false\" >\
object.item.audioItem.musicTrack\
</upnp:searchClass>\
<upnp:searchClass includeDerived=\"false\" >\
object.item.imageItem.photo\
</upnp:searchClass>\
<upnp:searchClass name=\"Vendor Album Art\" includeDerived=\"true\">\
object.item.imageItem.photo.vendorAlbumArt\
</upnp:searchClass>\
</container>\
</DIDL-Lite>\
";


std::string ex2 = "\
<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \
xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" \
xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">\
<container id=\"1\" parentID=\"0\" childCount=\"2\" restricted=\"false\">\
<dc:title>My Music</dc:title>\
<upnp:class>object.container.storageFolder</upnp:class>\
<upnp:storageUsed>730000</upnp:storageUsed>\
<upnp:writeStatus>WRITABLE</upnp:writeStatus>\
<upnp:searchClass includeDerived=\"false\">\
object.container.album.musicAlbum\
</upnp:searchClass>\
<upnp:searchClass includeDerived=\"false\">\
object.item.audioItem.musicTrack\
</upnp:searchClass>\
<upnp:createClass includeDerived=\"false\">\
object.container.album.musicAlbum\
</upnp:createClass>\
</container>\
<container id=\"2\" parentID=\"0\" childCount=\"2\" restricted=\"false\">\
<dc:title>My Photos</dc:title>\
<upnp:class>object.container.storageFolder</upnp:class>\
<upnp:storageUsed>177000</upnp:storageUsed>\
<upnp:writeStatus>WRITABLE</upnp:writeStatus>\
<upnp:searchClass includeDerived=\"false\">\
object.container.album.photoAlbum\
</upnp:searchClass>\
<upnp:searchClass includeDerived=\"false\">\
object.item.imageItem.photo\
</upnp:searchClass>\
<upnp:createClass includeDerived=\"false\">\
object.container.album.photoAlbum\
</upnp:createClass>\
</container>\
<container id=\"30\" parentID=\"0\" childCount=\"2\" restricted=\"false\">\
<dc:title>Album Art</dc:title>\
<upnp:class>object.container.storageFolder</upnp:class>\
<upnp:storageUsed>40000</upnp:storageUsed>\
<upnp:writeStatus>WRITABLE</upnp:writeStatus>\
<upnp:createClass includeDerived=\"true\">\
object.item.imageItem.photo.vendorAlbumArt\
</upnp:createClass>\
</container>\
</DIDL-Lite>\
";

// FIXME: this gives a SAXParseException
// <upnp:searchClass name=\"Vendor Album Art\" includeDerived=\"true\">\
// object.item.imageItem.photo.vendorAlbumArt\
// <upnp:searchClass>\

std::string ex3 = "\
<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \
xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" \
xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\">\
<item id=\"6\" parentID=\"3\" restricted=\"false\">\
<dc:title>Chloe Dancer</dc:title>\
<dc:creator>Mother Love Bone</dc:creator>\
<upnp:class>object.item.audioItem.musicTrack</upnp:class>\
<res protocolInfo=\"http-get:*:audio/x-ms-wma:*\" size=\"200000\">\
http://10.0.0.1/getcontent.asp?id=6\
</res>\
</item>\
<item id=\"8\" parentID=\"3\" restricted=\"false\">\
<dc:title>Drown</dc:title>\
<dc:creator>Smashing Pumpkins</dc:creator>\
<upnp:class>object.item.audioItem.musicTrack</upnp:class>\
<res protocolInfo=\"http-get:*:audio/mpeg:*\" size=\"140000\">\
http://10.0.0.1/getcontent.asp?id=8\
</res>\
</item>\
<item id=\"7\" parentID=\"3\" restricted=\"false\">\
<dc:title>State Of Love And Trust</dc:title>\
<dc:creator>Pearl Jam</dc:creator>\
<upnp:class>object.item.audioItem.musicTrack</upnp:class>\
<res protocolInfo=\"http-get:*:audio/x-ms-wma:*\" size=\"70000\">\
http://10.0.0.1/getcontent.asp?id=7\
</res>\
</item>\
</DIDL-Lite>\
";

std::string arMediaTomb1_orig = "\
<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\n\
<u:BrowseResponse xmlns:u=\"urn:schemas-upnp-org:service:ContentDirectory:1\">\n\
<Result>&lt;DIDL-Lite xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/&quot; xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot;&gt;&lt;container id=&quot;12&quot; parentID=&quot;0&quot; restricted=&quot;1&quot; childCount=&quot;6&quot;&gt;&lt;dc:title&gt;Audio&lt;/dc:title&gt;&lt;upnp:class&gt;object.container&lt;/upnp:class&gt;&lt;/container&gt;&lt;container id=&quot;1&quot; parentID=&quot;0&quot; restricted=&quot;1&quot; childCount=&quot;1&quot;&gt;&lt;dc:title&gt;PC Directory&lt;/dc:title&gt;&lt;upnp:class&gt;object.container&lt;/upnp:class&gt;&lt;/container&gt;&lt;container id=&quot;415&quot; parentID=&quot;0&quot; restricted=&quot;1&quot; childCount=&quot;2&quot;&gt;&lt;dc:title&gt;Photos&lt;/dc:title&gt;&lt;upnp:class&gt;object.container&lt;/upnp:class&gt;&lt;/container&gt;&lt;container id=&quot;786&quot; parentID=&quot;0&quot; restricted=&quot;1&quot; childCount=&quot;2&quot;&gt;&lt;dc:title&gt;Playlists&lt;/dc:title&gt;&lt;upnp:class&gt;object.container&lt;/upnp:class&gt;&lt;/container&gt;&lt;container id=&quot;3074&quot; parentID=&quot;0&quot; restricted=&quot;1&quot; childCount=&quot;9&quot;&gt;&lt;dc:title&gt;Video&lt;/dc:title&gt;&lt;upnp:class&gt;object.container&lt;/upnp:class&gt;&lt;/container&gt;&lt;/DIDL-Lite&gt;</Result>\n\
<NumberReturned>5</NumberReturned>\n\
<TotalMatches>5</TotalMatches>\n\
<UpdateID>0</UpdateID>\n\
</u:BrowseResponse>\n\
</s:Body> </s:Envelope>\
";




int main(int argc, char** argv)
{
    Omm::Av::MediaObject ex1Object;
    std::clog << "Content Directory 1, 2.8.3.2." << std::endl << ex1 << std::endl << std::endl;
    ex1Object.readMetaData(ex1);
    std::clog << std::endl;
    
    std::clog << "Content Directory 1, 2.8.3.3." << std::endl << ex2 << std::endl << std::endl;
    ex1Object.readChildren(ex2);
    std::clog << std::endl;
    
    Omm::Av::MediaObject ex3Object;
    std::clog << "Content Directory 1, 2.8.3.5." << std::endl << ex1 << std::endl << std::endl;
    ex3Object.readChildren(ex3);
    
    Omm::ActionResponseReader ar(arMediaTomb1_orig, new Omm::Action());
    ar.action();
}
