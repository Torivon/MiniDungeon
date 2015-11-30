#include <pebble.h>

#include "Menu.h"
#include "OptionsMenu.h"
#include "UILayers.h"
#include "WorkerControl.h"

static bool vibration = true;
static bool fastMode = false;
static bool useWorkerApp = false;
static bool workerCanLaunch = true;
static bool optionsMenuVisible = false;

void DrawOptionsMenu(void)
{
	ShowMainWindowRow(0, "Options", "");
	ShowMainWindowRow(1, "Vibration", vibration ? "On" : "Off");
	ShowMainWindowRow(2, "Fast Mode", useWorkerApp ? "-" : fastMode ? "On" : "Off");
#if ALLOW_WORKER_APP
	ShowMainWindowRow(3, "Background", useWorkerApp ? "On" : "Off");
	ShowMainWindowRow(4, "Launch", !useWorkerApp ? "-" : workerCanLaunch ? "On" : "Off");
#endif
}

void ToggleVibration(void)
{
	vibration = !vibration;
	DrawOptionsMenu();
}

bool GetVibration(void)
{
	return vibration;
}

void SetVibration(bool enable)
{
	vibration = enable;
}

bool GetFastMode(void)
{
	return fastMode;
}

void SetFastMode(bool enable)
{
	fastMode = enable;
}

void ToggleFastMode(void)
{
	if(useWorkerApp)
		return;
	
	fastMode = !fastMode;
	DrawOptionsMenu();
}

void SetWorkerApp(bool enable)
{
	useWorkerApp = enable;
	SetFastMode(false);
	if(OptionsMenuIsVisible())
		DrawOptionsMenu();
}

void ToggleWorkerApp(void)
{
	if(useWorkerApp)
	{
		AttemptToKillWorkerApp();
	}
	else
	{
		AttemptToLaunchWorkerApp();
	}
}

bool GetWorkerApp(void)
{
	return useWorkerApp;
}

void SetWorkerCanLaunch(bool enable)
{
	workerCanLaunch = enable;
	SendWorkerCanLaunch();
}

void ToggleWorkerCanLaunch(void)
{
	if(!useWorkerApp)
		return;
	
	SetWorkerCanLaunch(!workerCanLaunch);
	DrawOptionsMenu();
}

bool GetWorkerCanLaunch(void)
{
	return workerCanLaunch;
}

void OptionsMenuAppear(Window *window);
void OptionsMenuDisappear(Window *window);

MenuDefinition optionsMenuDef = 
{
	.menuEntries = 
	{
		{"Quit", "Return to main menu", PopMenu},
		{"Toggle", "Toggle Vibration", ToggleVibration},
		{"Toggle", "Speed up events", ToggleFastMode},
#if ALLOW_WORKER_APP
		{"Toggle", "Run in background", ToggleWorkerApp},
		{"Toggle", "Launch from background", ToggleWorkerCanLaunch}
#endif
	},
	.appear = OptionsMenuAppear,
	.disappear = OptionsMenuDisappear,
	.mainImageId = -1
};

void OptionsMenuAppear(Window *window)
{
	MenuAppear(window);
	DrawOptionsMenu();
	optionsMenuVisible = true;
}

void OptionsMenuDisappear(Window *window)
{
	MenuDisappear(window);
	optionsMenuVisible = false;
}

void ShowOptionsMenu(void)
{
	PushNewMenu(&optionsMenuDef);
}

bool OptionsMenuIsVisible(void)
{
	return optionsMenuVisible;
}
