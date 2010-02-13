/***************************************************************************|
|  Jamm - Just another multimedia ...                                       |
|         ... set of applications and libraries based on the UPnP-AV specs  |
|                                                                           |
|  Copyright (C) 2009                                                       |
|  Jörg Bakker (joerg'at'hakker'dot'de)                                     |
|                                                                           |
|  This file is part of Jamm.                                               |
|                                                                           |
|  Jamm is free software: you can redistribute it and/or modify             |
|  it under the terms of the GNU General Public License as published by     |
|  the Free Software Foundation version 3 of the License.                   |
|                                                                           |
|  Jamm is distributed in the hope that it will be useful,                  |
|  but WITHOUT ANY WARRANTY; without even the implied warranty of           |
|  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            |
|  GNU General Public License for more details.                             |
|                                                                           |
|  You should have received a copy of the GNU General Public License        |
|  along with this program.  If not, see <http://www.gnu.org/licenses/>.    |
***************************************************************************/
#include <Jamm/UpnpAvTypes.h>

// Content Directory 1, 2.8.3.2. Browsing the root level metadata

static std::string ex1 = "\
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


static std::string ex2 = "\
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



int main(int argc, char** argv)
{
    Jamm::Av::MediaObject ex1Object;
    std::clog << "Content Directory 1, 2.8.3.2." << std::endl << ex1 << std::endl << std::endl;
    ex1Object.readMetaData(ex1);
    std::clog << std::endl;
    
    std::clog << "Content Directory 1, 2.8.3.3." << std::endl << ex2 << std::endl << std::endl;
    ex1Object.readChildren(ex2);
}
