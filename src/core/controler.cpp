/***************************************************************************
 *   Copyright (C) 2006 by Jörg Bakker                                     *
 *   joerg@hakker.de                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "controler.h"
#include "widgetfactory.h"
#include "streamplayer.h"
#include "tv.h"
#include "halwatcher.h"
#include "volumelistmanager.h"
#include "haldeviceevent.h"
#include "system/lirc/lircwatcher.h"
#include "listproxy.h"
#include "listbrowser.h"
#include "debug.h"

#include "tvrecplayer.h"
#include "tvrecpopup.h"
#include "dvdplayer.h"
#include "ejectpage.h"

Controler *Controler::m_instance = 0;

Controler*
Controler::instance()
{
    if (m_instance == 0) {
        m_instance = new Controler();
    }
    return m_instance;
}


Controler::Controler()
 : m_maxAudioDevice(-1)
{
    TRACE("Controler::Controler()");
}


Controler::~Controler()
{
}


void
Controler::init(int argc, char **argv)
{
    m_argc = argc;
    m_argv = argv;
    m_goingBack = false;
    pthread_mutex_init(&m_eventTriggerMutex, 0);
    pthread_cond_init (&m_eventTrigger, 0);

    WidgetFactory::instance()->setToolkit(WidgetFactory::ToolkitQt);

    m_pageStack = new PageStack();
    m_upnp.Start();

    TRACE("Adding Main Menu.");
    m_mainMenu = new Menu("Main Menu");
    TRACE("Added Main Menu: %p", m_mainMenu);

    // Decide, which implementation of StreamPlayer to use.
    TRACE("Adding Xine StreamPlayer.");
    StreamPlayer::instance()->setEngine(StreamPlayer::EngineXine);

    TRACE("Adding Renderer.");
    m_renderer = new Renderer(NULL, "Jamm Media Renderer");
    addDevice(PLT_DeviceHostReference(m_renderer), "Jamm Media Renderer");
//     RendererThread *rendererThread = new RendererThread();
//     addEventLoop(rendererThread);

    // TODO: check if any TV source is available, otherwise don't add TV module.
    TRACE("Adding TV module.");
//     addModule(new Tv());

    TRACE("Adding HalWatcher.");
//     HalWatcher *halWatcher = new HalWatcher();
//     addEventLoop(halWatcher);

    TRACE("Adding LircWatcher.");
//     LircWatcher *lircWatcher = new LircWatcher();
//     addEventLoop(lircWatcher);

    TRACE("Adding Settings Menu.");
    m_settings = Menu("Settings");
    mainMenuAddEntry(&m_settings);

    TRACE("Showing Main Menu.");
    mainMenuShow();
}


int
Controler::loop()
{
    TRACE("Controler::mainLoop()");
    // start all other event loop threads
    for(vector<Thread*>::iterator i = m_eventLoop.begin(); i != m_eventLoop.end(); ++i) {
        (*i)->start();
    }
    // enter main event loop
    TRACE("Controler::mainLoop() starting event loop!!!");
    bool processEvents = true;
    while (processEvents) {
        TRACE("Controler::mainLoop() waiting for events...");
        pthread_mutex_lock(&m_eventTriggerMutex);
        pthread_cond_wait(&m_eventTrigger, &m_eventTriggerMutex);
        pthread_mutex_unlock(&m_eventTriggerMutex);
        TRACE("Controler::mainLoop() event received");
        // process the pending events.
        processEvents = dispatchEvents();
        // while events are processed, all new event triggers are ignored (but events are queued, anyway).
    }
    // exit main event loop
    TRACE("Controler::mainLoop() exit main loop");

    // exit all other event loop threads.
    for(vector<Thread*>::iterator i = m_eventLoop.begin(); i != m_eventLoop.end(); ++i) {
        (*i)->kill();
    }
    // cleanup and exit main thread.
    TRACE("Controler::mainLoop() cleaning up and exit");
    pthread_mutex_destroy(&m_eventTriggerMutex);
    pthread_cond_destroy(&m_eventTrigger);

    remDevice(PLT_DeviceHostReference(m_renderer));
    m_upnp.Stop();
    exit(0);
}


void
Controler::queueEvent(Event *event)
{
    TRACE("Controler::queueEvent()");
    // queue event in the event fifo.
    m_eventQueue.push(event);
    // signal the main loop to wake up and process the event.
    pthread_mutex_lock(&m_eventTriggerMutex);
    pthread_cond_signal(&m_eventTrigger);
    pthread_mutex_unlock(&m_eventTriggerMutex);
}


bool
Controler::dispatchEvents()
{
    // go through queued events and handle each ...
    while(!m_eventQueue.empty()) {
        TRACE("Controler::dispatchEvents() next event ...");
        Event *e = m_eventQueue.front();
        switch(e->type()) {
        // handle all global events
        case Event::QuitE:
            // quit application, no more event processing.
            return false;
        case Event::MenuE:
            TRACE("Controler::dispatchEvents() showing main menu");
            mainMenuShow();
            break;
        case Event::BackE:
            TRACE("Controler::dispatchEvents() going back");
            goBack();
            break;
        case Event::SwitchAudioE:
            TRACE("Controler::dispatchEvents() switching audio device");
//             StreamPlayer::instance()->switchAudioDevice(m_maxAudioDevice);
            StreamPlayer::instance()->switchAudioDevice(1);
            break;
        case Event::DeviceE: {
// Device added:
// HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/volume_label_Cast_Away
// Device removed:
// HalManager::DeviceRemovedCb() removed device /org/freedesktop/Hal/devices/volume_label_Cast_Away
            // TODO: in case of VolumeT and DvdT, create a Menu with entries "Show/Play" and "Eject"
            string devName = ((HalDeviceEvent*)e)->getDevName();
            HalDeviceEvent::DeviceAction devAction = ((HalDeviceEvent*)e)->getDevAction();

            if (devAction == HalDeviceEvent::Add) {
                string devPath = ((HalDeviceEvent*)e)->getDevPath();
                string mediaPath = ((HalDeviceEvent*)e)->getMediaPath();
                HalDeviceEvent::DeviceT devType = ((HalDeviceEvent*)e)->deviceType();
                string label = ((HalDeviceEvent*)e)->getLabel();
                TRACE("Controler::dispatchEvents() device with path %s added", devPath.c_str());
                if (((HalDeviceEvent*)e)->deviceType() == HalDeviceEvent::VolumeT) {
                    VolumeListManager *volMan = new VolumeListManager(mediaPath);
                    ListProxy *volumeList = new ListProxy(volMan);
                    ListBrowser *volumeListBrowser = new ListBrowser("Volume", "Id;Name%$File Name", volumeList);
                    TvRecPlayer *volumePlayer = new TvRecPlayer(volumeList);
                    volumeListBrowser->setPopupMenu(new TvRecPopup(volumePlayer, volumeListBrowser));
                    addRemovableVolume(devName, devPath, devType, label, volumeListBrowser);
                    if (((HalDeviceEvent*)e)->hotplug()) {
                        // get number of entries in volumeListBrowser. If only one entry, start playing otherwise
                        // show ListBrowser
                        volumeListBrowser->showUp();
                        if (volumeListBrowser->entryCount() == 1) {
                            TRACE("Controler::dispatchEvents() only one file on volume, playing directly");
                            volumePlayer->play(volumeListBrowser->getCurrent());
                            volumePlayer->showUp();
                        }
                    }
                }
                else if (((HalDeviceEvent*)e)->deviceType() == HalDeviceEvent::DvdT) {
                    DvdPlayer *dvdPlayer = new DvdPlayer();
                    addRemovableVolume(devName, devPath, devType, label, dvdPlayer);
                    if (((HalDeviceEvent*)e)->hotplug()) {
                        dvdPlayer->showUp();
                    }
                }
                else if (((HalDeviceEvent*)e)->deviceType() == HalDeviceEvent::SoundT) {
                    m_maxAudioDevice++;
                }
            }
            else if (devAction == HalDeviceEvent::Remove) {
                remRemovableVolume(devName);
                mainMenuShow();
            }
            break;
        }
        // all none global events are forwarded to the current visible Page or to the GUI toolkit.
        default:
            // TODO: if event comes from GUI toolkit, hasEventType() was already called once before.
            if (getCurrentPage()->hasEventType(e->type())) {
                TRACE("Controler::dispatchEvents() forward event to page: %p", getCurrentPage());
                getCurrentPage()->eventHandler(e);
            }
            else {
                TRACE("Controler::dispatchEvents() forward event to GUI toolkit");
                pageStack()->queueEvent(e);
            }
        }
        m_eventQueue.pop();
        delete e;
        TRACE("Controler::dispatchEvents() event done");
    }
    TRACE("Controler::dispatchEvents() all events processed");
    return true;  // go on waiting for events
}


void
Controler::addModule(Module *module)
{
    m_module[module->getName()] = module;
}


void
Controler::addDevice(PLT_DeviceHostReference device, string device_name)
{
    m_device[device_name] = device;
    m_upnp.AddDevice(device);
}


void
Controler::remDevice(PLT_DeviceHostReference device)
{
    m_upnp.RemoveDevice(device);
//     m_device.erase(device);
}


void
Controler::addPage(Page *page)
{
    m_pageStack->addPage(page);
}


void
Controler::addEventLoop(Thread *eventLoop)
{
    m_eventLoop.push_back(eventLoop);
}


void
Controler::showPage(Page *page)
{
    if (page != 0) { // some menu entries don't have real screen pages (for example "quit")
        TRACE("Controler::showPage() raising page: %p named: %s", page, page->getName().c_str());
        m_pageStack->raisePage(page);
        TRACE("Controler::showPage() done raising page");
        if (m_goingBack) {
            m_goingBack = false;
            m_pageHistory.pop_back();
        }
        else {
            if (page == m_mainMenu) {
                // forget the history, we are in the main menu again.
                TRACE("Controler::showPage() pageHistory clear");
                m_pageHistory.clear();
            }
            m_pageHistory.push_back(page);
        }
        TRACE("Controler::showPage() pageHistory count: %i", m_pageHistory.size());
    }
}


void
Controler::goBack()
{
    int count = m_pageHistory.size();
    if ( count > 1) {
        m_goingBack = true;
        m_pageHistory.at(count - 2)->showUp();
        TRACE("Controler::goBack() pageHistory count: %i", m_pageHistory.size());
    }
}


void
Controler::mainMenuShow()
{
    m_mainMenu->showUp();
}


void
Controler::addRemovableVolume(string devName, string device, HalDeviceEvent::DeviceT volType, string label, Page *page)
{
    TRACE("Controler::addRemovableVolume() for device: %s", devName.c_str());
    Menu *volumeMenu = new Menu(label);
    mainMenuAddEntry(volumeMenu);
    volumeMenu->addEntry(page);
    // store pointers to Menu and Pages in a hash, indexed with devName, for later removal.
    // TODO: store also Page pointers, otherwise the Pages remain in memory (memory leak!)
    m_volumeMenuDict[devName] = volumeMenu;
    // show entry for volume eject / removal 
    // -> use an empty (dummy) page for it, and only define a showUp() method for eject/umount action
    EjectPage *ejectPage = new EjectPage("Eject", volType, device);
    volumeMenu->addEntry(ejectPage);
}


void
Controler::remRemovableVolume(string devName)
{
    mainMenuRemEntry(m_volumeMenuDict.find(devName)->second);
    m_volumeMenuDict.erase(devName);
}
