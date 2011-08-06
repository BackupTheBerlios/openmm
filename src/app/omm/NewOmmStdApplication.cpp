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

#include <Omm/UpnpApplication.h>
#include <Omm/Util.h>

int main(int argc, char** argv)
{
    Omm::Util::PluginLoader<Omm::UpnpApplication> pluginLoader;
    Omm::UpnpApplication* pApp;
    try {
        pApp = pluginLoader.load("stdapplication-qt", "StdApplication");
    }
    catch(Poco::NotFoundException) {
        Omm::Log::instance()->upnp().error("OMM application could not find Qt application plugin");
        return 1;
    }

    pApp->setApplicationName("Std OMM");
    pApp->init();
    pApp->enableController();
    int ret = pApp->run(argc, argv);
    delete pApp;

    return ret;
}

