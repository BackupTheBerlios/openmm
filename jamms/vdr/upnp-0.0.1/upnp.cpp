/*
 * upnp.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>

#include "vdrmediaserver.h"
// #include "streamdev/server/server.h"

#include <platinum/PltUPnP.h>

static const char *VERSION        = "0.0.1";
static const char *DESCRIPTION    = "Expose VDR's capabilities to UPnP-AV";
static const char *MAINMENUENTRY  = "UPnP";

class cPluginUpnp : public cPlugin {
private:
  // Add any member variables or functions you may need here.
  PLT_UPnP *m_upnp;
  VdrMediaServer *m_vdrMediaServer;

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
//   delete m_vdrMediaServer;
//   delete m_upnp;
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
  m_upnp = new PLT_UPnP(1900, true);
  m_vdrMediaServer = new VdrMediaServer("Video Disc Recorder", false, NULL, 5656);
  return true;
}

bool cPluginUpnp::Start(void)
{
  // Start any background activities the plugin shall perform.
//   cStreamdevServer::Initialize();

  PLT_DeviceHostReference device(m_vdrMediaServer);
  m_upnp->AddDevice(device);
  m_upnp->Start();
  return true;
}

void cPluginUpnp::Stop(void)
{
  // Stop any background activities the plugin shall perform.
  m_upnp->Stop();

//   cStreamdevServer::Destruct();
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
