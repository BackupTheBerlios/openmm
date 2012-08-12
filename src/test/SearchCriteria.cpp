/***************************************************************************|
|  OMM - Open Multimedia                                                    |
|                                                                           |
|  Copyright (C) 2009, 2010, 2011                                           |
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
#include <vector>
#include <string>

#include <Poco/NumberFormatter.h>
#include <Poco/String.h>
#include <Poco/LineEndingConverter.h>

#include <Omm/UpnpAvObject.h>
#include <Omm/Log.h>


int
main(int argc, char** argv) {
    Omm::Av::SqlSearchCriteria search;
    std::vector<std::string> searchStrings;

    if (argc > 1) {
        searchStrings.push_back(std::string(argv[1]));
    }
    else {
        searchStrings.push_back("*");
        searchStrings.push_back("upnp:artist = \"foo\"");
        searchStrings.push_back("upnp:artist contains \"foo\"");
        searchStrings.push_back("upnp:artist = \"foo\" and dc:title = \"bar\"");
        searchStrings.push_back("upnp:artist = \"foo\" and (dc:title = \"bar\" or dc:title = \"foobar\")");
        searchStrings.push_back("upnp:artist contains \"foo\" and (dc:title doesNotContain \"bar\" or dc:title derivedfrom \"foobar\")");
        searchStrings.push_back("upnp:artist contains \"foo\" and ((dc:title doesNotContain \"bar\") or (dc:title derivedfrom \"foobar\"))");
    }

    for (std::vector<std::string>::iterator it = searchStrings.begin(); it != searchStrings.end(); ++it) {
        LOGNS(Omm::Av, upnpav, debug, "Upnp-AV search test search: " + *it);
        std::string translatedString;
        try {
            translatedString = search.parse(*it);
        }
        catch (Poco::Exception& e) {
            LOGNS(Omm::Av, upnpav, debug, "Upnp-AV search test error parsing search criteria: " + e.displayText());
        }
        LOGNS(Omm::Av, upnpav, debug, "Upnp-AV search test transl: " + translatedString + Poco::LineEnding::NEWLINE_DEFAULT);
    }

    return 0;
}
