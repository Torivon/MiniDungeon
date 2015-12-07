#include <pebble_worker.h>
// ../src/Events.c holds the probablities for events. 
// This way we only have to write it once.
#if ALLOW_WORKER_APP
#include "../src/Events.c"
#endif
