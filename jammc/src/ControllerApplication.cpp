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

#include <Poco/ClassLoader.h>
#include <Poco/Exception.h>
#include <Jamm/UpnpController.h>

int main(int argc, char** argv)
{
    // TODO: write a class loader that checks environment variable JAMM_PLUGIN_PATH
    Poco::ClassLoader<Jamm::Av::UpnpAvUserInterface> guiLoader;
    try {
        guiLoader.loadLibrary("/home/jb/devel/cc/jammbin/jammc/src/QtUi/libjammc-av-ui-qt.so");
    }
    catch(Poco::NotFoundException) {
        std::cerr << "Error in ControllerApplication: could not find plugin for user interface" << std::endl;
        return 1;
    }
    std::clog << "ControllerApplication: user interface loaded successfully" << std::endl;
    
    Jamm::Av::UpnpAvUserInterface* pUserInterface;
    Jamm::Av::UpnpAvController controller;
    
    pUserInterface = guiLoader.create("ControllerGui");
    controller.setUserInterface(pUserInterface);
    pUserInterface->initGui();
    pUserInterface->showMainWindow();
    controller.start();
    std::clog << "ControllerApplication: starting event loop" << std::endl;
    return pUserInterface->eventLoop();
}
