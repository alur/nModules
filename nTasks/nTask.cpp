/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nTask.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nTask module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/ErrorHandler.h"
#include "Constants.h"
#include "nTask.h"
#include "Taskbar.hpp"
#include "Version.h"
#include "WindowManager.h"


using std::map;

// The LSModule class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

// All the taskbars we currently have loaded
TaskbarMap gTaskbars;

//
void CreateTestWindow();


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR /* path */)
{
    if (!gLSModule.Initialize(parent, instance))
    {
        return 1;
    }
    
    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION)))
    {
        return 1;
    }

    // Load settings
    LoadSettings();

    // Start the window manager
    WindowManager::Start();

    // Register a bang command for creating our test window
    LiteStep::AddBangCommand(_T("!nTaskTestWindow"), [] (HWND, LPCTSTR) -> void
    {
        CreateTestWindow();
    });

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* hDllInstance */)
{
    LiteStep::RemoveBangCommand(_T("!nTaskTestWindow"));

    // Stop the window manager
    WindowManager::Stop();

    // Remove all taskbars
    for (TaskbarMap::value_type taskbar : gTaskbars)
    {
        delete taskbar.second;
    }
    gTaskbars.clear();

    gLSModule.DeInitalize();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
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
            switch(wParam)
            {
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
            for (auto &taskbar : gTaskbars)
            {
                taskbar.second->GetWindow()->FullscreenActivated((HMONITOR) wParam, (HWND) lParam);
            }
        }
        return 0;

    case LM_FULLSCREENDEACTIVATED:
        {
            for (auto &taskbar : gTaskbars)
            {
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
void LoadSettings()
{
    LiteStep::IterateOverLineTokens(_T("*nTaskbar"), CreateTaskbar);
}


/// <summary>
/// Creates a new taskbar
/// </summary>
void CreateTaskbar(LPCTSTR taskbarName)
{
    if (gTaskbars.find(taskbarName) == gTaskbars.end())
    {
        gTaskbars[taskbarName] = new Taskbar(taskbarName);
    }
    else
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Attempt to (re)create the already existing taskbar %s!"), taskbarName);
    }
}
