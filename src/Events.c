#include "Events.h"

// This needs to be kept in sync with ../worker_src/Worker_Events.c

// These should add up to 100
int chances[] = 
{
	44, //EVENT_ITEM
	44, //EVENT_BATTLE
	9, //EVENT_NEW_FLOOR
	3 //EVENT_SHOP
};

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
