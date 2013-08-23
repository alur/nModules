/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nLabel module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/ErrorHandler.h"
#include "Label.hpp"
#include "nLabel.h"
#include <map>
#include <strsafe.h>
#include "Version.h"

using std::map;

// The LSModule class
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

// All the top-level labels we currently have loaded.
// These do not include overlay labels
map<tstring, Label*> gTopLevelLabels;

// All the labels we currently have loaded. Labels add and remove themselfs from this list.
map<tstring, Label*> gAllLabels;


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

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all labels
    for (auto label : gTopLevelLabels)
    {
        delete label.second;
    }
    
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

    case LM_FULLSCREENACTIVATED:
        {
            for (auto &label : gTopLevelLabels)
            {
                label.second->GetWindow()->FullscreenActivated((HMONITOR) wParam, (HWND) lParam);
            }
        }
        return 0;

    case LM_FULLSCREENDEACTIVATED:
        {
            for (auto &label : gTopLevelLabels)
            {
                label.second->GetWindow()->FullscreenDeactivated((HMONITOR) wParam);
            }
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings()
{
    LiteStep::IterateOverLineTokens(_T("*nLabel"), [] (LPCTSTR labelName) -> void
    {
        CreateLabel(labelName);
    });
}


/// <summary>
/// Creates a new label
/// </summary>
void CreateLabel(LPCTSTR labelName)
{
    if (gAllLabels.find(labelName) == gAllLabels.end())
    {
        gTopLevelLabels[labelName] = new Label(labelName);
    }
    else
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Attempt to (re)create the already existing label %s!"), labelName);
    }
}
