#include <pebble.h>
#include "Logging.h"

#if PROFILE_LOGGING

static time_t seconds = 0;
static uint16_t ms = 0;
static bool startCalled = false;

void ProfileLogStart(char *name)
{
	if(startCalled)
		return;
	
	ms = time_ms(&seconds, NULL);
	PROFILE_LOG("Start profiling %s: %u.%d", name, (unsigned int)seconds, ms);
	startCalled = true;
}

void ProfileLogStop(char *name)
{
	if(!startCalled)
		return;
	
	time_t endSeconds = 0;
	uint16_t endMs = time_ms(&endSeconds, NULL);
	uint16_t total = (endSeconds - seconds) * 1000;
	total += endMs - ms;
	PROFILE_LOG("Stop profiling %s: %u.%d; %d ms", name, (unsigned int)endSeconds, endMs, total);
	startCalled = false;
}

#endif
