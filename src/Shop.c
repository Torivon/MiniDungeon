#include "pebble.h"

#if ENABLE_SHOPS

#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

int costs[5] = 
{
	10,
	100,
	20,
	20,
	20
};

void ShopItemMenuAppear(Window *window);

void DrawMainItemShopWindow(void)
{
	ShowAllItemCounts();
	ShowMainWindowRow(0, "Gold", UpdateGoldText());
}

void BuyItem(ItemType type)
{
	int cost = costs[type];
	CharacterData *data = GetCharacter();
	if (data->gold >= cost)
	{
		if(AddItem(type))
		{
			data->gold -= cost;
			DrawMainItemShopWindow();
		}
	}	
}

void BuyPotion(void)
{
	BuyItem(ITEM_TYPE_POTION);
}

void BuyElixir(void)
{
	BuyItem(ITEM_TYPE_FULL_POTION);
}

void BuyBomb(void)
{
	BuyItem(ITEM_TYPE_FIRE_SCROLL);
}

void BuyIcicle(void)
{
	BuyItem(ITEM_TYPE_ICE_SCROLL);
}

void BuySpark(void)
{
	BuyItem(ITEM_TYPE_LIGHTNING_SCROLL);
}

// These costs need to match the numbers in the costs array above
MenuDefinition shopItemMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to shop menu", .menuFunction = PopMenu},
		{.text = "10", .description = "Buy Potion", .menuFunction = BuyPotion},
		{.text = "100", .description = "Buy Elixir", .menuFunction = BuyElixir},
		{.text = "20", .description = "Buy Bomb", .menuFunction = BuyBomb},
		{.text = "20", .description = "Buy Icicle", .menuFunction = BuyIcicle},
		{.text = "20", .description = "Buy Spark", .menuFunction = BuySpark},
	},
	.appear = ShopItemMenuAppear,
	.mainImageId = -1,
	.floorImageId = -1
};

void ShopItemMenuAppear(Window *window)
{
	MenuAppear(window);
	DrawMainItemShopWindow();
}

void ShowShopItemMenu(void)
{
	PushNewMenu(&shopItemMenuDef);
}

void ShopStatMenuAppear(Window *window);

static int statPointsPurchased = 0;

void ResetStatPointsPurchased(void)
{
	statPointsPurchased = 0;
}

void SetStatPointsPurchased(int count)
{
	statPointsPurchased = count;
}

int GetStatPointsPurchased(void)
{
	return statPointsPurchased;
}

const char *UpdateStatCostText(void)
{
	static char costText[] = "0000"; // Needs to be static because it's used by the system later.
	int cost = 1 << statPointsPurchased;
	IntToString(costText, 4, cost);
	return costText;
}

void DrawMainStatShopWindow(void)
{
	ShowMainWindowRow(0, "Gold", UpdateGoldText());
	ShowMainWindowRow(1, "Stat Point", UpdateStatCostText());
}

void BuyStatPoint(void)
{
	int cost = 1 << statPointsPurchased;
	CharacterData *data = GetCharacter();
	if (data->gold >= cost)
	{
		++statPointsPurchased;
		AddStatPointToSpend();
		data->gold -= cost;
		DrawMainStatShopWindow();
	}
}

MenuDefinition shopStatMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to shop menu", .menuFunction = PopMenu},
		{.text = "Buy", .description = "Buy stat point", .menuFunction = BuyStatPoint},
	},
	.appear = ShopStatMenuAppear,
	.mainImageId = -1,
	.floorImageId = -1
};

void ShopStatMenuAppear(Window *window)
{
	MenuAppear(window);
	DrawMainStatShopWindow();
}

void ShowShopStatMenu(void)
{
	PushNewMenu(&shopStatMenuDef);
}

void ShopMenuWindowAppear(Window *window);

MenuDefinition shopMenuDef = 
{
	.menuEntries = 
	{
		{.text = "Quit", .description = "Return to adventure", .menuFunction = PopMenu},
		{.text = "Items", .description = "Buy items", .menuFunction = ShowShopItemMenu},
		{.text = "Stats", .description = "Buy stat points", .menuFunction = ShowShopStatMenu},
	},
	.appear = ShopMenuWindowAppear,
	.mainImageId = RESOURCE_ID_IMAGE_SHOP,
	.floorImageId = -1
};

void ShopMenuWindowAppear(Window *window)
{
	MenuAppear(window);
	ShowMainWindowRow(0, "Shop", "");
}

void ShowShopWindow(void)
{
	INFO_LOG("Entering shop.");
	PushNewMenu(&shopMenuDef);
}

#endif