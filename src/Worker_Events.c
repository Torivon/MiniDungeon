#include "MiniDungeon.h"

#ifdef BUILD_WORKER_FILES
#include <pebble_worker.h>
// ../src/Events.c holds the probablities for events. 
// This way we only have to write it once.
#include "../src/Events.c"
#endif
