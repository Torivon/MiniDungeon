#pragma once
#include "MiniDungeon.h"

#if DEBUG_LOGGING > 0
#define DEBUG_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_DEBUG, fmt, ## args)
#else
#define DEBUG_LOG(fmt, args...)
#endif

#if DEBUG_LOGGING > 1
#define DEBUG_VERBOSE_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, fmt, ## args)
#else
#define DEBUG_VERBOSE_LOG(fmt, args...)
#endif

#if ERROR_LOGGING
#define ERROR_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_ERROR, fmt, ## args)
#else
#define ERROR_LOG(fmt, args...)
#endif

#if WARNING_LOGGING
#define WARNING_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_WARNING, fmt, ## args)
#else
#define WARNING_LOG(fmt, args...)
#endif

#if INFO_LOGGING
#define INFO_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_INFO, fmt, ## args)
#else
#define INFO_LOG(fmt, args...)
#endif

#if PROFILE_LOGGING
#define PROFILE_LOG(fmt, args...) APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, fmt, ## args)
void ProfileLogStart(char *name);
void ProfileLogStop(char *name);
#else
#define PROFILE_LOG(fmt, args...)
#define ProfileLogStart(name)
#define ProfileLogStop(name)
#endif
