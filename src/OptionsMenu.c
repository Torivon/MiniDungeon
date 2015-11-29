#include <pebble.h>

#include "Menu.h"
#include "OptionsMenu.h"
#include "UILayers.h"
#include "WorkerControl.h"

static bool vibration = true;
static int fastMode = false;
static int useWorkerApp = false;
static bool optionsMenuVisible = false;

void DrawOptionsMenu(void)
{
	ShowMainWindowRow(0, "Options", "");
	ShowMainWindowRow(1, "Vibration", vibration ? "On" : "Off");
	ShowMainWindowRow(2, "Fast Mode", fastMode ? "On" : "Off");
#if ALLOW_WORKER_APP
	ShowMainWindowRow(3, "Background", useWorkerApp ? "On" : "Off");
#if ALLOW_TEST_MENU
	ShowMainWindowRow(4, WorkerIsRunning() ? "Running" : "Not Running", "");
#endif
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
	fastMode = !fastMode;
	DrawOptionsMenu();
}

void SetWorkerApp(bool enable)
{
	useWorkerApp = enable;
	if(useWorkerApp)
	{
		int result = 0;
#if ALLOW_WORKER_APP
		SetFastMode(false);
		if(!WorkerIsRunning())
		{
			result = LaunchWorkerApp();
			if(result == APP_WORKER_RESULT_ASKING_CONFIRMATION)
			{
				ActivateWorkerAppLaunchCheck();
			}
		}
		else
		{
			SetWorkerReady(true);
		}
#endif
	}
	else
	{
#if ALLOW_WORKER_APP
		KillWorkerApp();
		SetWorkerReady(false);
#endif
	}
}

void ToggleWorkerApp(void)
{
	SetWorkerApp(!useWorkerApp);
	DrawOptionsMenu();
}

bool GetWorkerApp(void)
{
	return useWorkerApp;
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
		{"Toggle", "Events in background", ToggleWorkerApp}
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
