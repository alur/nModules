/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nIcon.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nIcon module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nIcon.h"
#include "IconGroup.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/ErrorHandler.h"
#include "Version.h"


// The messages we want from the core
const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All current icon groups
map<string, IconGroup*> gIconGroups;

// The LiteStep module class
LSModule gLSModule(MODULE_NAME, MODULE_AUTHOR, MakeVersion(MODULE_VERSION));


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

    OleInitialize(nullptr);

    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    // Remove all groups
    for (auto group : gIconGroups) {
        delete group.second;
    }
    gIconGroups.clear();

    gLSModule.DeInitalize();

    OleUninitialize();
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
            Refresh();
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings() {
    LiteStep::IterateOverLines("*nIcon", [] (LPCSTR line) -> void {
        LiteStep::IterateOverTokens(line, [] (LPCSTR token) -> void {
            CreateGroup(token);
        });
    });
}


/// <summary>
/// Updates settings from the RC.
/// </summary>
void Refresh() {

}


/// <summary>
/// Creates a new group with the specified name.
/// </summary>
/// <param name="groupName">The name of the group to create.</param>
void CreateGroup(LPCSTR groupName) {
    if (gIconGroups.find(groupName) == gIconGroups.end()) {
        gIconGroups[groupName] = new IconGroup(groupName);
    }
    else {
        ErrorHandler::Error(ErrorHandler::Level::Critical, TEXT("Attempt to (re)create the already existing group %s!"), groupName);
    }
}
