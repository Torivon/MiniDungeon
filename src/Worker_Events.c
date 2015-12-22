#include <pebble_worker.h>
#include "MiniDungeon.h"

#ifdef BUILD_WORKER_FILES
// ../src/Events.c holds the probablities for events. 
// This way we only have to write it once.
#include "../src/Events.c"
#endif
