#include "debug.h"
#include <cstdio>
#include <cstdarg>
#include <stdlib.h>


// basically we use the standard printf functions, with two changes:
// 1. we name it TRACE(), so we have to go through the hassle with varargs.
// 2. we append a newline.


void
TRACE(const char *msg, ...) {
#ifdef __DEBUG__
    static int debug_env = getenv("DEBUG") ? 1 : 0;

    if(debug_env)
    {
        va_list ap;
        va_start(ap, msg);                    // use variable arg list
        vfprintf(stderr, msg, ap);
        fprintf(stderr, "\n");
        va_end(ap);
    }

#endif  // __DEBUG__
}
