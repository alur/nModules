/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nLabel module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Label.hpp"
#include "Version.h"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/ErrorHandler.h"

#include "../Utilities/StringUtils.h"

#include <strsafe.h>
#include <unordered_map>


using std::tstring;


static void CreateLabel(LPCTSTR labelName);
static void DestroyLabels();
static void LoadSettings();


// The LSModule class.
LSModule gLSModule(TEXT(MODULE_NAME), TEXT(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core.
static UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, LM_FULLSCREENACTIVATED,
    LM_FULLSCREENDEACTIVATED, 0 };

// All the top-level labels we currently have loaded.
// These do not include overlay labels.
static StringKeyedMaps<tstring, Label>::UnorderedMap gTopLevelLabels;

// All the labels we currently have loaded. Labels add and remove themselves from this list.
StringKeyedMaps<tstring, Label*>::UnorderedMap gAllLabels;


/// <summary>
/// Creates a new label.
/// </summary>
/// <param name="labelName">The RC settings prefix of the label to create.</param>
void CreateLabel(LPCTSTR labelName)
{
    if (gAllLabels.find(labelName) == gAllLabels.end())
    {
        gTopLevelLabels.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(labelName),
            std::forward_as_tuple(labelName)
        );
    }
    else
    {
        ErrorHandler::Error(
            ErrorHandler::Level::Critical,
            TEXT("Attempt to (re)create the already existing label %s!"),
            labelName
        );
    }
}


/// <summary>
/// Destroys all labels.
/// </summary>
void DestroyLabels()
{
    gTopLevelLabels.clear();
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
/// <param name="parent"></param>
/// <param name="instance">Handle to this module's instance.</param>
/// <param name="path">Path to the LiteStep directory.</param>
/// <returns>0 on success, non-zero on error.</returns>
/// <remarks>
/// If this function returns non-zero, the module will be unloaded immediately, without
/// going through quitModule.
/// </remarks>
EXPORT_CDECL(int) initModuleW(HWND parent, HINSTANCE instance, LPCWSTR path)
{
    UNREFERENCED_PARAMETER(path);

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
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings()
{
    LiteStep::IterateOverLineTokens(TEXT("*nLabel"), CreateLabel);
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
            SendMessage(
                LiteStep::GetLitestepWnd(),
                LM_REGISTERMESSAGE,
                (WPARAM)window,
                (LPARAM)gLSMessages
            );
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(
                LiteStep::GetLitestepWnd(),
                LM_UNREGISTERMESSAGE,
                (WPARAM)window,
                (LPARAM)gLSMessages
            );
        }
        return 0;

    case LM_FULLSCREENACTIVATED:
        {
            for (auto & label : gTopLevelLabels)
            {
                label.second.GetWindow()->FullscreenActivated((HMONITOR)wParam, (HWND)lParam);
            }
        }
        return 0;

    case LM_FULLSCREENDEACTIVATED:
        {
            for (auto & label : gTopLevelLabels)
            {
                label.second.GetWindow()->FullscreenDeactivated((HMONITOR)wParam);
            }
        }
        return 0;

    case LM_REFRESH:
        {
            DestroyLabels();
            LoadSettings();
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
/// <param name="instance">Handle to this module's instance.</param>
EXPORT_CDECL(void) quitModule(HINSTANCE instance)
{
    UNREFERENCED_PARAMETER(instance);

    DestroyLabels();
    gLSModule.DeInitalize();
}
