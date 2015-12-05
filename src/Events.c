#include "Events.h"

#if ALLOW_SHOP
// These should add up to 100
int chances[] = 
{
	44, //EVENT_ITEM
	44, //EVENT_BATTLE
	9, //EVENT_NEW_FLOOR
	3 //EVENT_SHOP
};
#else
// These should add up to 100
int chances[] = 
{
	40, //EVENT_ITEM
	50, //EVENT_BATTLE
	10 //EVENT_NEW_FLOOR
};
#endif

int *GetEventChances(void)
{
	return chances;
}

int GetEventCount(void)
{
	return sizeof(chances)/sizeof(*chances);
}

static int baseChanceOfEvent = 35;

int GetBaseChanceOfEvent(void)
{
	return baseChanceOfEvent;
}
