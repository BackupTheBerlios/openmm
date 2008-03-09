#include "debug.h"
#include <cstdio>
#include <cstdarg>


// basically we use the standard printf functions, with two changes:
// 1. we name it TRACE(), so we have to go through the hassle with varargs.
// 2. we append a newline.

// // #ifdef __DEBUG__

void
TRACE(const char *msg, ...) {
    char buf[10240];  // TODO: fix these horrible fixed size C-String buffers, ..., aarghhh!
    va_list ap;
    va_start( ap, msg );                    // use variable arg list
    vsprintf( buf, msg, ap );
    va_end( ap );
    fprintf( stderr, "%s\n", buf );         // add newline
}

// // #endif  // __DEBUG__
