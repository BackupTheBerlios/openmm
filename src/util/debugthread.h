#ifndef DEBUG_THREAD_H
#define DEBUG_THREAD_H

#include <cstdio>

using namespace std;

#include <stdlib.h>

#include "threads.h"

extern wxMutex MutexLog;

class DebugThread : public wxThread
{
public:
	DebugThread ();

	void* Entry ();
	void OnExit ();
	
	void Write (string s, bool newline = true);
	
private:
	string* buffer;
	FILE* debugout;
	
	void Flush ();
};

#endif  //DEBUG_THREAD_H
