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
#include "lircwatcher.h"
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

    TRACE("Adding Main Menu.");
    m_mainMenu = new Menu("Main Menu");
    TRACE("Added Main Menu: %p", m_mainMenu);

    // Decide, which implementation of StreamPlayer to use.
    TRACE("Adding Xine StreamPlayer.");
    StreamPlayer::instance()->setEngine(StreamPlayer::EngineXine);

    // TODO: check if any TV source is available, otherwise don't add TV module.
    TRACE("Adding TV module.");
    addModule(new Tv());

    TRACE("Adding HalWatcher.");
    HalWatcher *halWatcher = new HalWatcher();
    addEventLoop(halWatcher);

    TRACE("Adding LircWatcher.");
    LircWatcher *lircWatcher = new LircWatcher();
    addEventLoop(lircWatcher);

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


/* TODO: Bug on ws11, no List is shown sometimes when inserting a volume:

Page::Page() adding Page Volume: 0x890931c to PageStack.
Page::Page() adding event types, we now have: 3
ListBrowser::ListBrowser()
ListBrowser::ListBrowser() creating ListBrowser widget with 2 columns
WidgetFactory::createListBrowser()
WidgetFactory::createListBrowser() for toolkit Qt
QtListBrowser::QtListBrowser()
QtListBrowser::QtListBrowser(), parent widget: 0x8909398
QtListBrowser::QtListBrowser(), build QListView widget: 0x89099d0
QtStyle::QtStyle() try to get the default font
QtStyle::QtStyle() try to get the default font
QtStyle::QtStyle() try to get the default font
QtStyle::QtStyle() try to get the default font
QtStyle::QtStyle() try to get the default font
ListBrowser::ListBrowser() adding column: Id
ListBrowser::ListBrowser() adding column: Id without formating
QtListBrowser::addViewColumn() with name: Id
ListBrowser::ListBrowser() adding column: Name%$File Name
QtListBrowser::addViewColumn() with name: File Name
List::link()
WidgetFactory::createPage()
WidgetFactory::createPage() for toolkit Qt
QtPage::QtPage() creating Page widget: 0x890c100.
QtStyle::QtStyle() try to get the default font
Page::Page() adding Page Recordings: 0x890c088 to PageStack.
Page::Page() adding event types, we now have: 3
PopupMenu::PopupMenu()
WidgetFactory::createPopupMenu()
WidgetFactory::createPopupMenu() for toolkit Qt
QtPopupMenu::QtPopupMenu()
QtStyle::QtStyle() try to get the default font
TvRecPopup::TvRecPopup()
QtMenu::addEntry() for page: 0x890931c
X Error: BadWindow (invalid Window parameter) 3
  Major opcode:  12
  Minor opcode:  0
  Resource id:  0x0
Page::showUp()
*/

/* TODO: Bug: inserting an USB-Stick leads to a crash:

HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951
    info.bus: usb_device
    info.linux.driver: usb
    info.parent: /org/freedesktop/Hal/devices/usb_device_0_0_0000_00_1d_7
    info.product: CnMemory
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951
    info.vendor: Unknown (0x090c)
    linux.hotplug_type: 1
    linux.subsystem: usb
    linux.sysfs_path: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4
    linux.sysfs_path_device: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4
    usb_device.bus_number: 4
    usb_device.can_wake_up: no
    usb_device.configuration_value: 1
    usb_device.device_class: 0
    usb_device.device_protocol: 0
    usb_device.device_revision_bcd: 4352
    usb_device.device_subclass: 0
    usb_device.is_self_powered: no
    usb_device.linux.device_number: 2
    usb_device.linux.sysfs_path: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4
    usb_device.max_power: 100
    usb_device.num_configurations: 1
    usb_device.num_interfaces: 1
    usb_device.num_ports: 0
    usb_device.product: CnMemory
    usb_device.product_id: 4096
    usb_device.serial: AA04012700013951
    usb_device.speed_bcd: 294912
    usb_device.vendor: Unknown (0x090c)
    usb_device.vendor_id: 2316
    usb_device.version_bcd: 512
HalManager::handleDevice() found device without "info.category"
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0
    info.bus: usb
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951
    info.product: USB Mass Storage Interface
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0
    linux.hotplug_type: 1
    linux.subsystem: usb
    linux.sysfs_path: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0
    linux.sysfs_path_device: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0
    usb.bus_number: 4
    usb.can_wake_up: no
    usb.configuration_value: 1
    usb.device_class: 0
    usb.device_protocol: 0
    usb.device_revision_bcd: 4352
    usb.device_subclass: 0
    usb.interface.class: 8
    usb.interface.number: 0
    usb.interface.protocol: 80
    usb.interface.subclass: 6
    usb.is_self_powered: no
    usb.linux.device_number: 2
    usb.linux.sysfs_path: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0
    usb.max_power: 100
    usb.num_configurations: 1
    usb.num_interfaces: 1
    usb.num_ports: 0
    usb.product: USB Mass Storage Interface
    usb.product_id: 4096
    usb.serial: AA04012700013951
    usb.speed_bcd: 294912
    usb.vendor: Unknown (0x090c)
    usb.vendor_id: 2316
    usb.version_bcd: 512
HalManager::handleDevice() found device without "info.category"
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host
    info.capabilities: as
    info.category: scsi_host
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0
    info.product: SCSI Host Adapter
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host
    linux.hotplug_type: 2
    linux.subsystem: scsi_host
    linux.sysfs_path: /sys/class/scsi_host/host0
    linux.sysfs_path_device: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0/host0
    scsi_host.host: 0
HalManager::handleDevice() found device of category: scsi_host
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_usbraw
    info.capabilities: as
    info.category: usbraw
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951
    info.product: USB Raw Device Access
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_usbraw
    linux.device_file: /dev/bus/usb/004/002
    linux.hotplug_type: 2
    linux.subsystem: usb_device
    linux.sysfs_path: /sys/class/usb_device/usbdev4.2
    usbraw.device: /dev/bus/usb/004/002
HalManager::handleDevice() found device of category: usbraw
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0
    info.bus: scsi
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host
    info.product: SCSI Device
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0
    linux.hotplug_type: 1
    linux.subsystem: scsi
    linux.sysfs_path: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0/host0/target0:0:0/0:0:0:0
    linux.sysfs_path_device: /sys/devices/pci0000:00/0000:00:1d.7/usb4/4-4/4-4:1.0/host0/target0:0:0/0:0:0:0
    scsi.bus: 0
    scsi.host: 0
    scsi.lun: 0
    scsi.model: CnMemory
    scsi.target: 0
    scsi.type: disk
    scsi.vendor:
HalManager::handleDevice() found device without "info.category"
HalDevice::PropertyModifiedCb() modified property info.linux.driver
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0_scsi_generic
    info.capabilities: as
    info.category: scsi_generic
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0
    info.product: SCSI Generic Interface
    info.udi: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0_scsi_generic
    linux.device_file: /dev/sg0
    linux.hotplug_type: 2
    linux.subsystem: scsi_generic
    linux.sysfs_path: /sys/class/scsi_generic/sg0
    scsi_generic.device: /dev/sg0
HalManager::handleDevice() found device of category: scsi_generic
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/storage_serial_090c_CnMemory_AA04012700013951
    block.device: /dev/sda
    block.is_volume: no
    block.major: 8
    block.minor: 0
    block.storage_device: /org/freedesktop/Hal/devices/storage_serial_090c_CnMemory_AA04012700013951
    info.addons: as
    info.capabilities: as
    info.category: storage
    info.parent: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0_scsi_host_scsi_device_lun0
    info.product: CnMemory
    info.udi: /org/freedesktop/Hal/devices/storage_serial_090c_CnMemory_AA04012700013951
    info.vendor:
    linux.hotplug_type: 3
    linux.sysfs_path: /sys/block/sda
    linux.sysfs_path_device: /sys/block/sda
    storage.automount_enabled_hint: yes
    storage.bus: usb
    storage.drive_type: disk
    storage.firmware_version: 1100
    storage.hotpluggable: yes
    storage.lun: 0
    storage.media_check_enabled: yes
    storage.model: CnMemory
    storage.no_partitions_hint: no
    storage.physical_device: /org/freedesktop/Hal/devices/usb_device_90c_1000_AA04012700013951_if0
    storage.policy.should_mount: yes
    storage.removable: yes
    storage.requires_eject: yes
    storage.serial: 090c_CnMemory_AA04012700013951
    storage.vendor:
HalManager::handleDevice() found device of category: storage
HalManager::DeviceAddedCb() added device /org/freedesktop/Hal/devices/volume_uuid_6887_5A1D
    block.device: /dev/sda1
    block.is_volume: yes
    block.major: 8
    block.minor: 1
    block.storage_device: /org/freedesktop/Hal/devices/storage_serial_090c_CnMemory_AA04012700013951
    info.capabilities: as
    info.category: volume
    info.interfaces: as
    info.parent: /org/freedesktop/Hal/devices/storage_serial_090c_CnMemory_AA04012700013951
    info.product: USB DISK
    info.udi: /org/freedesktop/Hal/devices/volume_uuid_6887_5A1D
    linux.hotplug_type: 3
    linux.sysfs_path: /sys/block/sda/sda1
    linux.sysfs_path_device: /sys/block/sda/sda1
    org.freedesktop.Hal.Device.Volume.method_execpaths: as
    org.freedesktop.Hal.Device.Volume.method_names: as
    org.freedesktop.Hal.Device.Volume.method_signatures: as
    storage.model:
    volume.block_size: 512
    volume.fstype: vfat
    volume.fsusage: filesystem
    volume.fsversion: FAT16
    volume.ignore: no
    volume.is_disc: no
    volume.is_mounted: no
    volume.is_mounted_read_only: no
    volume.is_partition: yes
    volume.label: USB DISK
    volume.mount.valid_options: as
    volume.mount_point:
    volume.num_blocks: 1981408
    volume.partition.msdos_part_table_type: 6
    volume.partition.number: 1
    volume.policy.desired_mount_point: USB DISK
    volume.policy.mount_filesystem: vfat
    volume.policy.mount_option.iocharset=utf8: yes
    volume.policy.mount_option.quiet: yes
    volume.policy.should_mount: yes
    volume.size: t
    volume.unmount.valid_options: as
    volume.uuid: 6887-5A1D
HalManager::handleDevice() found device of category: volume
terminate called after throwing an instance of 'DBus::ErrorInvalidArgs'
  what():  type mismatch
*/
