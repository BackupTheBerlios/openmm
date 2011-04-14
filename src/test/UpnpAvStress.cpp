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

#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Random.h>
#include <Poco/Environment.h>
#include <Poco/DirectoryIterator.h>

#include <Omm/UpnpAvController.h>
#include <Omm/Util.h>


class StressAvUserInterface : public Omm::Av::AvUserInterface
{
public:
    virtual int eventLoop();

private:
    Omm::Av::ControllerObject* chooseRandomItem(Omm::Av::ControllerObject* pParentContainer);
};


int
StressAvUserInterface::eventLoop()
{
    const int maxTests = 10;
    const int maxPlayTime = 2000;  // msec
    const int waitForDevice = 1000; // msec
    const std::string ignoreServerUuid("fa095ecc-e13e-40e7-8e6c-001f3fbdd43e");

    Poco::Random serverNumber;
    serverNumber.seed();
    Poco::Random rendererNumber;
    rendererNumber.seed();
    Poco::Random playTime;
    playTime.seed();

    bool testCountUnlimited = true;
    int test = 1;
    while (testCountUnlimited || test <= maxTests) {
        if (serverCount() == 0 || rendererCount() == 0) {
            Poco::Thread::sleep(waitForDevice);
        }
        else {
            Omm::Av::Log::instance()->upnpav().debug("test #" + Poco::NumberFormatter::format(test)
                + ", servers: " + Poco::NumberFormatter::format(serverCount())
                + ", renderers: " + Poco::NumberFormatter::format(rendererCount())
                );
            // choose a random server
            int selectedServerNumber;
            do {
                selectedServerNumber = serverNumber.next(serverCount());
            }
            while (serverUuid(selectedServerNumber) == ignoreServerUuid);

            Omm::Av::ControllerObject* pRootObject = serverRootObject(selectedServerNumber);
            Omm::Av::Log::instance()->upnpav().debug("server: " + pRootObject->getTitle() + ", uuid: " + serverUuid(selectedServerNumber));

            // choose a random item
            Omm::Av::ControllerObject* pObject = pRootObject;
            if (pRootObject->isContainer()) {
                pObject = chooseRandomItem(pRootObject);
            }

            // choose a random renderer
            Omm::Av::RendererView* pRenderer = rendererView(rendererNumber.next(rendererCount()));

            // play item on renderer
            if (pRenderer && pObject) {
                rendererSelected(pRenderer);
                mediaObjectSelected(pObject);
                Omm::Av::Log::instance()->upnpav().debug("playing: " + pObject->getTitle() + " on: " + pRenderer->getName());
                playPressed();
                Poco::Thread::sleep(maxPlayTime);
//                Poco::Thread::sleep(playTime.next(maxPlayTime));
                stopPressed();
            }
            test++;
        }
    }
}


Omm::Av::ControllerObject*
StressAvUserInterface::chooseRandomItem(Omm::Av::ControllerObject* pParentContainer)
{
    Poco::Random childNumber;
    childNumber.seed();

    if (pParentContainer->childCount() == 0) {
        return 0;
    }

    int child = childNumber.next(pParentContainer->childCount());
    while (pParentContainer->getChildCount() <= child) {
        pParentContainer->fetchChildren();
    }

    Omm::Av::ControllerObject* pObject = static_cast<Omm::Av::ControllerObject*>(pParentContainer->getChild(child));
    if (pObject->isContainer()) {
        pObject = chooseRandomItem(pObject);
    }

    return pObject;
}


int
main(int argc, char** argv) {
    Omm::Av::AvController controller;
    StressAvUserInterface userInterface;

    controller.setUserInterface(&userInterface);
    controller.start();
    
    Omm::Av::Log::instance()->upnpav().debug("Upnp-AV stress test: starting event loop");
    return userInterface.eventLoop();
}