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

#include <vdr/plugin.h>

#include "VdrMediaServer.h"

static const char *VERSION        = Omm::OMM_VERSION.c_str();
static const char *DESCRIPTION    = "Expose VDR's capabilities to UPnP-AV";
static const char *MAINMENUENTRY  = "UPnP";

class cPluginUpnp : public cPlugin {
private:
  // Add any member variables or functions you may need here.
    Omm::Av::UpnpAvServer*     _pChannels;
    Omm::Av::UpnpAvServer*     _pRecordings;

public:
  cPluginUpnp(void);
  virtual ~cPluginUpnp();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginUpnp::cPluginUpnp(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginUpnp::~cPluginUpnp()
{
  // Clean up after yourself!
}

const char *cPluginUpnp::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginUpnp::ProcessArgs(int /*argc*/, char */*argv*/[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginUpnp::Initialize(void)
{
    // Initialize any background activities the plugin shall perform.
    _pChannels = new Omm::Av::UpnpAvServer;
    _pChannels->setRoot(new VdrChannels);
    _pChannels->setFriendlyName("VDR Channels");
    
    _pRecordings = new Omm::Av::UpnpAvServer;
    _pRecordings->setRoot(new VdrRecordings);
    _pRecordings->setFriendlyName("VDR Recordings");
    
    return true;
}

bool cPluginUpnp::Start(void)
{
  // Start any background activities the plugin shall perform.
    _pChannels->start();
    _pRecordings->start();
    return true;
}

void cPluginUpnp::Stop(void)
{
  // Stop any background activities the plugin shall perform.
    _pChannels->stop();
    _pRecordings->stop();
}

void cPluginUpnp::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginUpnp::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginUpnp::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginUpnp::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginUpnp::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginUpnp::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginUpnp::SetupParse(const char */*Name*/, const char */*Value*/)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginUpnp::Service(const char */*Id*/, void */*Data*/)
{
  // Handle custom service requests from other plugins
  return false;
}

const char **cPluginUpnp::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginUpnp::SVDRPCommand(const char */*Command*/, const char */*Option*/, int &/*ReplyCode*/)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginUpnp); // Don't touch this!
