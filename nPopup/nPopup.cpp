/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nPopup module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"
#include "Popup.hpp"
#include "PopupItem.hpp"
#include "SeparatorItem.hpp"
#include "CommandItem.hpp"
#include "FolderItem.hpp"
#include "nPopup.h"

// The window classes we want to register
LPCSTR g_windowClasses[] = {"Popup", "Item", NULL};

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND /* hWndParent */, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule("nPopup", "Alurcard2", MAKE_VERSION(0,2,0,0), instance, g_lsMessages);
    
    if (!g_LSModule->Initialize(g_windowClasses)) {
        delete g_LSModule;
        return 1;
    }

    if (!g_LSModule->ConnectToCore(MAKE_VERSION(0,2,0,0))) {
        delete g_LSModule;
        return 1;
    }

    // Load settings
    LoadSettings();

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* hDllInstance */) {
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
LRESULT WINAPI LSMessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case LM_REFRESH:
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// 
/// </summary>
void LoadSettings() {
    LoadPopups();
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadPopups() {
    LPVOID f = LCOpen(NULL);
    bool success;
    Popup* popup;

    while (LoadPopup(f, &success, &popup)) {
        if (success) {
            //rootPopups.push_back(popup);
        }
    }

    LCClose(f);
}


/// <summary>
/// Loads a popup, !New
/// </summary>
bool LoadPopup(LPVOID f, bool * success, Popup** popup) {
    char line[MAX_LINE_LENGTH], token1[MAX_LINE_LENGTH], token2[MAX_LINE_LENGTH],
         token3[MAX_LINE_LENGTH], token4[MAX_LINE_LENGTH], token5[MAX_LINE_LENGTH];
    LPSTR tokens[] = { token1, token2, token3, token4, token5 };
    token1[0] = 0; token2[0] = 0; token3[0] = 0; token4[0] = 0; token5[0] = 0;

    // First of, find the first !New line
    if (!LCReadNextConfig(f, "*Popup", line, sizeof(line))) {
        *success = false;
        return false;
    }

    // Make sure that we have a !New line
    LCTokenize(line, tokens, 5, NULL);
    if (_stricmp(token3, "!New") != 0) {
        TRACE("Invalid popup line: %s", line);
        *success = false;
        return true;
    }

    // Create the popup
    *popup = new Popup(token2, token4, token5);

    // Get popup items.
    while (LCReadNextConfig(f, "*Popup", line, sizeof(line))) {
        // Read through the tokens
        LPCSTR linePointer = line;
        GetToken(linePointer, NULL, &linePointer, FALSE); // Drop *Popup
        GetToken(linePointer, token1, &linePointer, FALSE); //
        if (_stricmp(token1, "!Separator") == 0) {
            (*popup)->AddItem(new SeparatorItem());
        }
        else if (_stricmp(token1, "~New") == 0) {
            *success = true;
            return true;
        }
        else if (_stricmp(token1, "~Folder") == 0) {
            TRACE("Unexpected ~Folder, skipping *popup line.");
        }
        else if (_strnicmp(token1, ".icon=", strlen(".icon=")) == 0) {
            GetToken(linePointer, token2, &linePointer, FALSE); //
            GetToken(linePointer, token3, &linePointer, FALSE);
            if (_stricmp(token3, "Folder") == 0) {
                Popup* folderPopup = LoadFolder(f, token2, NULL); // TODO::Prefix
                if (folderPopup != NULL) {
                    (*popup)->AddItem(new FolderItem(token2, folderPopup, token1));
                }
            }
            // TODO:!PopupDynamic, ...
            else {
                // Command
                (*popup)->AddItem(new CommandItem(token2, token3, token1));
            }
        }
        else {
            // Assume it's a title.
            GetToken(linePointer, token2, &linePointer, FALSE); //
            if (_stricmp(token2, "Folder") == 0) {
                Popup* folderPopup = LoadFolder(f, token1, NULL); // TODO::Prefix
                if (folderPopup != NULL) {
                    (*popup)->AddItem(new FolderItem(token1, folderPopup));
                }
            }
            // TODO:!PopupDynamic, ...
            else {
                // Command
                (*popup)->AddItem(new CommandItem(token1, token2));
            }
        }
    }

    TRACE("Invalid popup detected, missing ~New");
    *success = false;
    delete *popup;
    return false;
}


/// <summary>
/// Loads a folder
/// </summary>
Popup* LoadFolder(LPVOID f, LPCSTR title, LPCSTR prefix) {
    /*char line[MAX_LINE_LENGTH], token1[MAX_LINE_LENGTH], token2[MAX_LINE_LENGTH],
         token3[MAX_LINE_LENGTH], token4[MAX_LINE_LENGTH], token5[MAX_LINE_LENGTH];
    LPSTR tokens[] = { token1, token2, token3, token4, token5 };
    Popup* ret = new Popup(title, NULL, prefix);

    while (LCReadNextConfig(f, "*Popup", line, sizeof(line))) {
    }*/
    return NULL;
}
