#ifndef DEBUG_H
#define DEBUG_H

// uncomment this to turn debugging on (or provide it on the compiler command line)
//#define __DEBUG__

// //#ifdef __DEBUG__
extern void TRACE( const char *, ... );

//#define TRACE(args) printf args
//#define TRACE( const char *, ... ) __attribute__ ((format (printf, 1, 2)))
//void TRACE( const char *, ... ) __attribute__ ((format (printf, 1, 2)));
//extern void trace( const char *, ... ) __attribute__ ((format (printf, 1, 2)));
//#define TRACE(args) trace args
// // #else
// // #define TRACE(args) 
// // #endif  // __DEBUG__


// extern void traceprint( const char *, ... );
// #define TRACE(args) traceprint( #args )




#endif  // DEBUG_H
