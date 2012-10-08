/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTask.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nShared/LSModule.hpp"
#include "nTask.h"
#include "Taskbar.hpp"
#include "WindowManager.h"
#include "Constants.h"

using std::map;

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, LM_WINDOWCREATED, LM_WINDOWACTIVATED,
    LM_WINDOWDESTROYED, LM_LANGUAGE, LM_REDRAW, LM_GETMINRECT, LM_WINDOWREPLACED,
    LM_WINDOWREPLACING, LM_MONITORCHANGED, NULL };

// All the labels we currently have loaded
map<LPCSTR, Taskbar*> g_Taskbars;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule(parent, "nTask", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);
    
    if (!g_LSModule->Initialize()) {
        delete g_LSModule;
        return 1;
    }

    if (!g_LSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_LSModule;
        return 1;
    }

    // Load settings
    LoadSettings();

    // Start the window manager
    WindowManager::Start();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* hDllInstance */) {
    // Stop the window manager
    WindowManager::Stop();

    // Remove all taskbars
    for (map<LPCSTR, Taskbar*>::const_iterator iter = g_Taskbars.begin(); iter != g_Taskbars.end(); iter++) {
        delete iter->second;
    }
    g_Taskbars.clear();

    if (g_LSModule) {
        delete g_LSModule;
    }
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            // Add the existing windows in a little bit so we dont hinder startup.
            SetTimer(window, TIMER_ADD_EXISTING, 50, NULL);

            SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case WM_TIMER:
        {
            switch(wParam) {
            case TIMER_ADD_EXISTING:
                {
                    KillTimer(window, TIMER_ADD_EXISTING);
                    WindowManager::AddExisting();
                }
                return 0;

            default:
                {
                }
                return WindowManager::ShellMessage(window, message, wParam, lParam);
            }
        }
        break;

    case LM_REFRESH:
        return 0;

    case LM_GETMINRECT:
    case LM_LANGUAGE:
    case LM_REDRAW:
    case LM_WINDOWACTIVATED:
    case LM_WINDOWCREATED:
    case LM_WINDOWDESTROYED:
    case LM_WINDOWREPLACED:
    case LM_WINDOWREPLACING:
    case LM_MONITORCHANGED:
    case WM_DISPLAYCHANGE:
    case WM_ADDED_EXISTING:
        return WindowManager::ShellMessage(window, message, wParam, lParam);
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LCOpen(NULL);
    LPSTR name;

    while (LCReadNextConfig(f, "*nTaskbar", szLine, sizeof(szLine))) {
        LCTokenize(szLine+strlen("*nTaskbar")+1, szTokens, 1, NULL);
        name = _strdup(szLabel);
        g_Taskbars.insert(g_Taskbars.begin(), std::pair<LPCSTR, Taskbar*>(name, new Taskbar(name)));
    }
    LCClose(f);
}
