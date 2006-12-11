#include "debug.h"

// Debug::Debug ()
// {
// #ifdef __DEBUG__
// 	dt = new DebugThread ();
// #ifdef USE_THREADS
// 	dt->Create ();
// 	dt->Run ();
// #endif  // USE_THREADS
// #endif // __DEBUG__
// }


// Debug::~Debug ()
// {
// #ifdef __DEBUG__
// #ifdef USE_THREADS
// 	dt->Kill ();
// #endif  // USE_THREADS
// #endif // __DEBUG__
// }


void
Debug::trace (string format, ...)
{
#ifdef __DEBUG__
    printf (s, newline);
#endif // __DEBUG__
}


// void
// Debug::trace (string s, bool newline)
// {
// #ifdef __DEBUG__
// 	dt->Write (s, newline);
// #endif // __DEBUG__
// }
// 
// 
// void
// Debug::trace (const char* s1, const char* s2, bool newline)
// {
// #ifdef __DEBUG__
// 	dt->Write (string(s1) + s2, newline);
// #endif // __DEBUG__
// }
// 
// 
// void
// Debug::trace (const char* s, long i, unsigned int radix, bool newline)
// {
// #ifdef __DEBUG__
// 	char buf[sizeof(long)*4+1];  // #bytes / 2 = #bits * 4 and terminating '\0'
// 	ltoa (i, buf, radix);
// 	dt->Write (string(s) + buf, newline);
// #endif // __DEBUG__
// }
