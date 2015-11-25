#include "pebble.h"

#if ENABLE_ITEMS

#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Menu.h"
#include "Adventure.h"
#include "UILayers.h"
#include "Utils.h"

typedef struct
{
	const char *name;
	char countText[2];
} ItemData;

ItemData itemData[] = 
{
	{"Potion", "00"},
	{"Elixir", "00"},
	{"Bomb", "00"},
	{"Icicle", "00"},
	{"Spark", "00"}
};

int randomItemTable[] = 
{
	50,
	5,
	15,
	15,
	15
};

int itemsOwned[ITEM_TYPE_COUNT];

int *GetItemsOwned(void)
{
	return itemsOwned;
}

int GetSizeOfItemsOwned(void)
{
	return sizeof(itemsOwned);
}

const char *UpdateItemCountText(ItemType itemType)
{
#if BOUNDS_CHECKING
	if(itemType >= ITEM_TYPE_COUNT)
		return "";
#endif
		
	IntToString(itemData[itemType].countText, 2, itemsOwned[itemType]);
	return itemData[itemType].countText;
}

void ClearInventory(void)
{
	int i;
	for(i = 0; i < ITEM_TYPE_COUNT; ++i)
	{
		itemsOwned[i] = 0;
	}
}

const char *GetItemName(ItemType itemType)
{
#if BOUNDS_CHECKING
	if(itemType >= ITEM_TYPE_COUNT)
		return "";
#endif

	return itemData[itemType].name;
}

void ItemGainMenuInit(Window *window);
void ItemGainMenuAppear(Window *window);

MenuDefinition itemGainMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Ok", .description = "Return to adventuring", .menuFunction = PopMenu}
	},
	.init = ItemGainMenuInit,
	.appear = ItemGainMenuAppear,
	.mainImageId = -1,
	.floorImageId = -1
};

void ShowAllItemCounts(void)
{
	int i;
	ShowMainWindowRow(0, "Items", "");
	for(i = 0; i < ITEM_TYPE_COUNT; ++i)
	{
		ShowMainWindowRow(i + 1, GetItemName(i), UpdateItemCountText(i));
	}
}

ItemType typeGained;

void ItemGainMenuAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "Item Gained", "");
	ShowMainWindowRow(1, GetItemName(typeGained), UpdateItemCountText(typeGained));
}

bool AddItem(ItemType type)
{
	itemsOwned[type]++;
	if(itemsOwned[type] > 99)
	{
		itemsOwned[type] = 99;
		return false;
	}
	return true;
}

void ItemGainMenuInit(Window *window)
{
	int result = Random(100) + 1;
	int i = 0;
	int acc = 0;
	MenuInit(window);
	do
	{
		acc += randomItemTable[i];
		if(acc >= result)
		{
			typeGained = i;
			AddItem(i);
			break;
		}
		++i;      
	} while (i < ITEM_TYPE_COUNT);
}

void ShowItemGainWindow(void)
{
	INFO_LOG("Item gained.");
	PushNewMenu(&itemGainMenuDef);
}

bool AttemptToUseHealingItem(ItemType type, int power)
{
	if(itemsOwned[type] > 0 && PlayerIsInjured())
	{
		HealPlayerByPercent(power);
		--itemsOwned[type];
		ShowAllItemCounts();
		return true;
	}  
	return false;
}

bool AttemptToUsePotion(void)
{
	return AttemptToUseHealingItem(ITEM_TYPE_POTION, 50);
}

void ActivatePotion(void)
{
	AttemptToUsePotion();
}

bool AttemptToUseFullPotion(void)
{
	return AttemptToUseHealingItem(ITEM_TYPE_FULL_POTION, 100);
}

void ActivateFullPotion(void)
{
	AttemptToUseFullPotion();
}

void ItemMainMenuAppear(Window *window);

MenuDefinition itemMainMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to main menu", .menuFunction = PopMenu},
		{.text = "Drink", .description = "Heal 50% of max health", .menuFunction = ActivatePotion},
		{.text = "Drink", .description = "Heal 100% of max health", .menuFunction = ActivateFullPotion}
	},
	.appear = ItemMainMenuAppear,
	.mainImageId = -1,
	.floorImageId = -1
};

void ItemMainMenuAppear(Window *window)
{
	MenuAppear(window);
	ShowAllItemCounts();
}

void ShowMainItemMenu(void)
{
	PushNewMenu(&itemMainMenuDef);
}

bool AttemptToUseItem(ItemType type)
{
	if(itemsOwned[type] > 0)
	{
		--itemsOwned[type];
		return true;
	}
	
	return false;
}

bool AttemptToConsumeFireScroll(void)
{
	return AttemptToUseItem(ITEM_TYPE_FIRE_SCROLL);
}

bool AttemptToConsumeIceScroll(void)
{
	return AttemptToUseItem(ITEM_TYPE_ICE_SCROLL);
}

bool AttemptToConsumeLightningScroll(void)
{
	return AttemptToUseItem(ITEM_TYPE_LIGHTNING_SCROLL);
}

#endif