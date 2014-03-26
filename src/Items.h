#pragma once

#if ENABLE_ITEMS

typedef enum
{
	ITEM_TYPE_POTION = 0,
	ITEM_TYPE_FULL_POTION,
	ITEM_TYPE_FIRE_SCROLL,
	ITEM_TYPE_ICE_SCROLL,
	ITEM_TYPE_LIGHTNING_SCROLL,
	ITEM_TYPE_COUNT
} ItemType;

int *GetItemsOwned(void);
int GetSizeOfItemsOwned(void);

void ShowItemGainWindow(void);
void ShowMainItemMenu(void);

bool AttemptToUsePotion(void);
bool AttemptToUseFullPotion(void);
bool AttemptToConsumeFireScroll(void);
bool AttemptToConsumeIceScroll(void);
bool AttemptToConsumeLightningScroll(void);
bool AddItem(ItemType type);

void ShowAllItemCounts(void);
void ClearInventory(void);

#endif