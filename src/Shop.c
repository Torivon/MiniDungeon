#include "pebble.h"

#include "Character.h"
#include "Items.h"
#include "Logging.h"
#include "Menu.h"
#include "MiniDungeon.h"
#include "OptionsMenu.h"
#include "Shop.h"
#include "UILayers.h"
#include "Utils.h"

#if ALLOW_SHOP

#if ALLOW_ITEM_SHOP

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
		{"Quit", "Return to shop menu", PopMenu},
		{"10", "Buy Potion", BuyPotion},
		{"100", "Buy Elixir", BuyElixir},
		{"20", "Buy Bomb", BuyBomb},
		{"20", "Buy Icicle", BuyIcicle},
		{"20", "Buy Spark", BuySpark},
	},
	.appear = ShopItemMenuAppear,
	.mainImageId = -1
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

#endif //ALLOW_ITEM_SHOP

#if ALLOW_STAT_SHOP

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
		{"Quit", "Return to shop menu", PopMenu},
		{"Buy", "Buy stat point", BuyStatPoint},
	},
	.appear = ShopStatMenuAppear,
	.mainImageId = -1
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

#endif //ALLOW_STAT_SHOP

void ShopMenuWindowAppear(Window *window);
void ShopMenuWindowInit(Window *window);

MenuDefinition shopMenuDef = 
{
	.menuEntries = 
	{
		{"Quit", "Return to adventure", PopMenu},
#if ALLOW_ITEM_SHOP
		{"Items", "Buy items", ShowShopItemMenu},
#endif
#if ALLOW_STAT_SHOP
		{"Stats", "Buy stat points", ShowShopStatMenu},
#endif
	},
	.init = ShopMenuWindowInit,
	.appear = ShopMenuWindowAppear,
	.mainImageId = RESOURCE_ID_IMAGE_SHOP
};

void ShopMenuWindowInit(Window *window)
{
	if(GetUseOldAssets())
	{
		shopMenuDef.mainImageId = RESOURCE_ID_IMAGE_SHOP_OLD;
	}
}

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
#endif //ALLOW_SHOP
