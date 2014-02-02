#pragma once
#include "MiniDungeon.h"

#if DEBUG_LOGGING
#define DEBUG_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_DEBUG, fmt, ## args)
#else
#define DEBUG_LOG(fmt, args...)
#endif