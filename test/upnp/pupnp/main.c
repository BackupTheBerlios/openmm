/* main.c - Main program routines
 *
 * Copyright (C) 2005-2007   Ivo Clarysse
 *
 * This file is part of GMediaRender.
 *
 * GMediaRender is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GMediaRender is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GMediaRender; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glib.h>

#include <upnp/ithread.h>
#include <upnp/upnp.h>

#include "logging.h"
#include "output_gstreamer.h"
#include "upnp.h"
#include "upnp_device.h"
#include "upnp_renderer.h"

static gboolean show_version = FALSE;
static gboolean show_devicedesc = FALSE;
static gboolean show_connmgr_scpd = FALSE;
static gboolean show_control_scpd = FALSE;
static gboolean show_transport_scpd = FALSE;
static gchar *ip_address = NULL;
static gchar *uuid = "GMediaRender-1_0-000-000-002";
static gchar *friendly_name = "gmediarender";

 
/* Generic GMediaRender options */
static GOptionEntry option_entries[] = {
	{ "version", 0, 0, G_OPTION_ARG_NONE, &show_version,
	  "Output version information and exit", NULL },
	{ "ip-address", 'I', 0, G_OPTION_ARG_STRING, &ip_address,
	  "IP address on which to listen", NULL },
	{ "uuid", 'u', 0, G_OPTION_ARG_STRING, &uuid,
	  "UUID to advertise", NULL },
	{ "friendly-name", 'f', 0, G_OPTION_ARG_STRING, &friendly_name,
	  "Friendly name to advertise", NULL },
	{ "dump-devicedesc", 0, 0, G_OPTION_ARG_NONE, &show_devicedesc,
	  "Dump device descriptor XML and exit", NULL },
	{ "dump-connmgr-scpd", 0, 0, G_OPTION_ARG_NONE, &show_connmgr_scpd,
	  "Dump Connection Manager service description XML and exit", NULL },
	{ "dump-control-scpd", 0, 0, G_OPTION_ARG_NONE, &show_control_scpd,
	  "Dump Rendering Control service description XML and exit", NULL },
	{ "dump-transport-scpd", 0, 0, G_OPTION_ARG_NONE, &show_transport_scpd,
	  "Dump A/V Transport service description XML and exit", NULL },
	{ NULL }
};

static void do_show_version(void)
{
	puts( "gmediarender 0.0.6" "\n"
        	"This is free software. "
		"You may redistribute copies of it under the terms of\n"
		"the GNU General Public License "
		"<http://www.gnu.org/licenses/gpl.html>.\n"
		"There is NO WARRANTY, to the extent permitted by law."
	);
}

static int process_cmdline(int argc, char **argv)
{
	int result = -1;
	GOptionContext *ctx;
	GError *err = NULL;
	int rc;

	ctx = g_option_context_new("- GMediaRender");
	g_option_context_add_main_entries(ctx, option_entries, NULL);

	rc = output_gstreamer_add_options(ctx);
	if (rc != 0) {
		goto out;
	}

	if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
		g_print ("Failed to initialize: %s\n", err->message);
		g_error_free (err);
		goto out;
	}


	result = 0;

out:
	LEAVE();
	return result;
}

int main(int argc, char **argv)
{
	int rc;
	int result = EXIT_FAILURE;
	struct device *upnp_renderer;

	if (!g_thread_supported()) {
		g_thread_init(NULL);
	}

	rc = process_cmdline(argc, argv);
	if (rc != 0) {
		goto out;
	}

	if (show_version) {
		do_show_version();
		exit(EXIT_SUCCESS);
	}
	if (show_connmgr_scpd) {
		upnp_renderer_dump_connmgr_scpd();
		exit(EXIT_SUCCESS);
	}
	if (show_control_scpd) {
		upnp_renderer_dump_control_scpd();
		exit(EXIT_SUCCESS);
	}
	if (show_transport_scpd) {
		upnp_renderer_dump_transport_scpd();
		exit(EXIT_SUCCESS);
	}

	upnp_renderer = upnp_renderer_new(friendly_name, uuid);
	if (upnp_renderer == NULL) {
		goto out;
	}

	if (show_devicedesc) {
		fputs(upnp_get_device_desc(upnp_renderer), stdout);
		exit(EXIT_SUCCESS);
	}

	rc = output_xine_init();
	if (rc != 0) {
		goto out;
	}

	rc = upnp_device_init(upnp_renderer, ip_address);
	if (rc != 0) {
		goto out;
	}

	printf("Ready for rendering..\n");
	output_loop();
	result = EXIT_SUCCESS;

out:
	return result;
}
