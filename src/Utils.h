#pragma once

void IntToString(char *buffer, size_t bufferSize, int value);

void SeedRandom(void);

void IntToPercent(char *buffer, size_t bufferSize, int value);

// Returns an integer in the range [0,max)
inline uint16_t Random_inline(uint16_t max)
{
	int result = (uint16_t)(rand() % max) + 1;
	return result;
}

uint16_t Random(uint16_t max);

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

#define BOOL_TO_STR(b) (b) ? "true" : "false"
