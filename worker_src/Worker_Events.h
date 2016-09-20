#pragma once

// These should add up to 100
enum
{
	EVENT_ITEM,
	EVENT_BATTLE,
	EVENT_NEW_FLOOR,
	EVENT_SHOP
};

int *GetEventChances(void);
int GetEventCount(void);
int GetBaseChanceOfEvent(void);
