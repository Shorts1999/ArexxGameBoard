#ifndef __SIMPLEDEBUG_H
#define __SIMPLEDEBUG_H

#if defined(DEBUG)
#define DEBUG_PRINT(x, ...) Serial.printf(x, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(x, ...) 
#endif
#undef DEBUG

#endif //define __SIMPLEDEBUG_H