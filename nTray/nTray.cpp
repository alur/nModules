/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTray.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTray module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nTray.h"
#include <map>
#include "Tray.hpp"
#include "TrayManager.h"
#include "../nShared/LSModule.hpp"


using std::map;
using std::string;


#define TIMER_INIT_COMPLETED 1

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, LM_SYSTRAY, LM_SYSTRAYINFOEVENT, NULL };

// Handle to the tray notify window
HWND g_hWndTrayNotify;

// All the trays we currently have loaded
map<string, Tray*> g_Trays;

// The LiteStep module class
LSModule* g_LSModule;

// True for the first 100ms of nTrays life. Speeds up loading.
bool g_InitPhase;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* szPath */) {
    g_InitPhase = true;

    g_LSModule = new LSModule(parent, "nTray", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);
    
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

    // Let the core know that we want the system tray icons
    g_hWndTrayNotify = (HWND)SendMessage(LiteStep::GetLitestepWnd(), LM_SYSTRAYREADY, NULL, NULL);

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all trays
    for (map<string, Tray*>::const_iterator iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
        delete iter->second;
    }
    g_Trays.clear();

    TrayManager::Stop();

    if (g_LSModule) {
        delete g_LSModule;
    }
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
            SetTimer(window, TIMER_INIT_COMPLETED, 100, NULL);
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)g_lsMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;

    case WM_TIMER:
        {
            switch(wParam) {
            case TIMER_INIT_COMPLETED:
                {
                    KillTimer(window, TIMER_INIT_COMPLETED);
                    g_InitPhase = false;
                    TrayManager::InitCompleted();
                }
                return 0;
            }
        }
        return 0;

    case LM_SYSTRAY:
    case LM_SYSTRAYINFOEVENT:
        return TrayManager::ShellMessage(window, message, wParam, lParam);

    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates trays.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], name[256];
    LPSTR szTokens[] = { name };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, "*nTray", szLine, sizeof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*nTray")+1, szTokens, 1, NULL);
        g_Trays.insert(g_Trays.begin(), std::pair<string, Tray*>(string(name), new Tray(name)));
    }
    LiteStep::LCClose(f);
}
