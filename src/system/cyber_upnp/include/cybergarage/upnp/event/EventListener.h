/******************************************************************
*
*	CyberLink for C++
*
*	Copyright (C) Satoshi Konno 2002-2003
*
*	File: EventListener.h
*
*	Revision;
*
*	08/12/03
*		- first revision
*
******************************************************************/

#ifndef _CLINK_EVENTLISTENER_H_
#define _CLINK_EVENTLISTENER_H_

namespace CyberLink {

class EventListener
{
public:
	virtual void eventNotifyReceived(const char *uuid, long seq, const char *varName, const char *value) = 0;
};

}

#endif

