/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTask.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "nTask.h"
#include "Taskbar.hpp"
#include "WindowManager.h"
#include "Constants.h"
#include "Version.h"


using std::map;

// The LSModule class
LSModule gLSModule(MODULE_NAME, MODULE_AUTHOR, MakeVersion(MODULE_VERSION));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

// All the labels we currently have loaded
map<string, Taskbar*> g_Taskbars;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* path */) {
    if (!gLSModule.Initialize(parent, instance)) {
        return 1;
    }
    
    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
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
    for (auto iter : g_Taskbars) {
        delete iter.second;
    }
    g_Taskbars.clear();

    gLSModule.DeInitalize();
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

            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
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

    case LM_FULLSCREENACTIVATED:
        {
            for (auto &taskbar : g_Taskbars) {
                taskbar.second->GetWindow()->FullscreenActivated((HMONITOR) wParam, (HWND) lParam);
            }
        }
        return 0;

    case LM_FULLSCREENDEACTIVATED:
        {
            for (auto &taskbar : g_Taskbars) {
                taskbar.second->GetWindow()->FullscreenDeactivated((HMONITOR) wParam);
            }
        }
        return 0;

    case LM_REFRESH:
        {
        }
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
    case LM_TASK_SETPROGRESSSTATE:
    case LM_TASK_SETPROGRESSVALUE:
    case LM_TASK_MARKASACTIVE:
    case LM_TASK_REGISTERTAB:
    case LM_TASK_UNREGISTERTAB:
    case LM_TASK_SETACTIVETAB:
    case LM_TASK_SETTABORDER:
    case LM_TASK_SETTABPROPERTIES:
    case LM_TASK_SETOVERLAYICON:
    case LM_TASK_SETOVERLAYICONDESC:
    case LM_TASK_SETTHUMBNAILTOOLTIP:
    case LM_TASK_SETTHUMBNAILCLIP:
    case LM_TASK_THUMBBARADDBUTTONS:
    case LM_TASK_THUMBBARUPDATEBUTTONS:
    case LM_TASK_THUMBBARSETIMAGELIST:
        {
        }
        return WindowManager::ShellMessage(window, message, wParam, lParam);
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[MAX_RCCOMMAND];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, "*nTaskbar", szLine, sizeof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*nTaskbar")+1, szTokens, 1, NULL);
        g_Taskbars.insert(g_Taskbars.begin(), std::pair<string, Taskbar*>(szLabel, new Taskbar(szLabel)));
    }
    LiteStep::LCClose(f);
}
