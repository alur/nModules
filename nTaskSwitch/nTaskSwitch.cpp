/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTaskSwitch.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTaskSwitch module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nTaskSwitch.h"
#include "../nShared/LSModule.hpp"
#include <map>
#include <strsafe.h>
#include "TaskSwitcher.hpp"

using std::map;

// The LSModule class
LSModule gLSModule("nTaskSwitch", "Alurcard2", MAKE_VERSION(0, 2, 0, 0));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

//
TaskSwitcher* g_TaskSwitcher;

//
#define HOTKEY_ALTTAB 1
#define HOTKEY_SHIFTALTTAB 2


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* path */) {
    if (!gLSModule.Initialize(parent, instance)) {
        return 1;
    }

    if (!gLSModule.ConnectToCore(MAKE_VERSION(0, 2, 0, 0))) {
        return 1;
    }

    // Load settings
    LoadSettings();

    RegisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_ALTTAB, MOD_ALT, VK_TAB);
    RegisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_SHIFTALTTAB, MOD_ALT | MOD_SHIFT, VK_TAB);

    g_TaskSwitcher = new TaskSwitcher();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    delete g_TaskSwitcher;

    UnregisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_ALTTAB);
    UnregisterHotKey(gLSModule.GetMessageWindow(), HOTKEY_SHIFTALTTAB);

    gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;

    case WM_HOTKEY:
        {
            switch (wParam) {
            case HOTKEY_ALTTAB:
                {
                    g_TaskSwitcher->HandleAltTab();
                }
                break;

            case HOTKEY_SHIFTALTTAB:
                {
                    g_TaskSwitcher->HandleAltShiftTab();
                }
                break;
            }
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads settings from the .RC files.
/// </summary>
void LoadSettings() {
}
