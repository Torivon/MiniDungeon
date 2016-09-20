#pragma once

#define FAST_MODE_IN_BACKGROUND false


// This needs to be kept in sync with "../src/Worker_Control.h"

// Worker message types
enum
{
    WORKER_LAUNCHED = 0,
    WORKER_DYING,
    TRIGGER_EVENT,
    WORKER_SEND_STATE1,
    WORKER_SEND_STATE2,
    WORKER_SEND_ERROR,
    
    APP_DYING,
    APP_AWAKE,
    APP_SEND_WORKER_CAN_LAUNCH
};

inline uint16_t Random_inline(uint16_t max)
{
    int result = (uint16_t)(rand() % max) + 1;
    return result;
}

inline int ComputeRandomEvent_inline(int baseChanceOfEvent, int ticksSinceLastEvent, int *events, int eventCount, bool fastMode)
{
    int result = Random_inline(100);
    int i = 0;
    int acc = 0;
    int chanceOfEvent = baseChanceOfEvent;
    int event = -1;
    if(ticksSinceLastEvent > 20)
    {
        chanceOfEvent += (ticksSinceLastEvent - 20) * 2;
    }
    
    if(!fastMode && result > chanceOfEvent)
        return -1;
    
    result = Random_inline(100);
    
    do
    {
        acc += events[i];
        if(acc >= result)
        {
            event = i;
            break;
        }
        ++i;      
    } while (i < eventCount);
    return event;
}
