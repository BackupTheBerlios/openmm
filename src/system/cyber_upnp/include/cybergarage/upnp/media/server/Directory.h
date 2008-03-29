/******************************************************************
*
*	MediaServer for CyberLink
*
*	Copyright (C) Satoshi Konno 2003
*
*	File : Directory.h
*
*	Revision:
*
*	03/23/04
*		- first revision.
*
******************************************************************/

#ifndef _CLINK_MEDIA_DIRECTORY_H_
#define _CLINK_MEDIA_DIRECTORY_H_

#include <cybergarage/upnp/media/server/object/container/ContainerNode.h>

namespace CyberLink {

class Directory : public ContainerNode
{
	////////////////////////////////////////////////
	// Constructor
	////////////////////////////////////////////////
	
public:

	Directory(ContentDirectory *cdir, const char *name)
	{
		setContentDirectory(cdir);
		setFriendlyName(name);
	}

	Directory(const char *name)
	{
		setContentDirectory(NULL);
		setFriendlyName(name);
	}
	
	////////////////////////////////////////////////
	// Name
	////////////////////////////////////////////////

public:

	void setFriendlyName(const char *name)
	{
		setTitle(name);
	}
	
	const char *getFriendlyName()
	{
		return getTitle();
	}
	

	////////////////////////////////////////////////
	// update
	////////////////////////////////////////////////
	
public:

	virtual void update() = 0;
};

}

#endif


