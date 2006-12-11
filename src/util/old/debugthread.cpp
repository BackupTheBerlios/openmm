#include "debugthread.h"

#include "wx/utils.h"

//wxMutex MutexLog;

DebugThread::DebugThread () : wxThread (wxTHREAD_DETACHED)
{
	debugout = fopen ("gideon.log", "w");
	buffer = new string();
}


void*
DebugThread::Entry ()
{
	while (true)
	{
		Flush ();
		_sleep (1000);
	}
	
	return NULL;
}


void
DebugThread::OnExit ()
{
	Flush ();
	fclose (debugout);
	delete buffer;
}


void
DebugThread::Write (string s, bool newline)
{
#ifdef USE_THREADS
// write message to buffer, which is written to file by an extra thread
	wxMutexLocker l(MutexLog);
	buffer->append (s);
	if (newline)
		buffer->append ("\n");
#else
// no threading: write message directly to file
	fwrite (s.c_str(), s.length (), 1, debugout);
	if (newline)
		fwrite ("\n", 1, 1, debugout);
	fflush (debugout);
#endif  // USE_THREADS
}


void
DebugThread::Flush ()
{
	MutexLog.Lock ();
	fwrite (buffer->c_str(), buffer->length (), 1, debugout);
	buffer->clear ();
	MutexLog.Unlock ();
	
	fflush (debugout);
}	
