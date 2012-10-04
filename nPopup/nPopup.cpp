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
#include <strsafe.h>
#include <map>
#include "Popup.hpp"
#include "PopupItem.hpp"
#include "SeparatorItem.hpp"
#include "CommandItem.hpp"
#include "FolderItem.hpp"
#include "InfoItem.hpp"
#include "nPopup.h"


// The window classes we want to register
LPCSTR g_windowClasses[] = {"Popup", "Item", NULL};

// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// All root level popups
vector<Popup*> rootPopups;


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
    for (vector<Popup*>::const_iterator iter = rootPopups.begin(); iter != rootPopups.end(); iter++) {
        RemoveBangCommand((*iter)->GetBang());
        delete *iter;
    }

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
void __cdecl HandlePopupBang(HWND owner, LPCSTR bang, LPCSTR args) {
    for (vector<Popup*>::const_iterator iter = rootPopups.begin(); iter != rootPopups.end(); iter++) {
        if (_stricmp(bang, (*iter)->GetBang()) == 0) {
            Popup* popup = *iter;
            popup->Show();

            break;
        }
    }
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
    Popup* popup;

    while (LoadPopup(f, POPUPLEVEL_ROOT, &popup)) {
        if (popup != NULL) {
            rootPopups.push_back(popup);
            AddBangCommandEx(popup->GetBang(), HandlePopupBang);
        }
    }

    LCClose(f);
}


/// <summary>
/// Loads a popup
/// </summary>
/// <return>Returns false when all lines have been read.</return>
bool LoadPopup(LPVOID f, POPUPLEVEL level, Popup** out) {
    char line[MAX_LINE_LENGTH], title[MAX_LINE_LENGTH], command[MAX_LINE_LENGTH], icon[MAX_LINE_LENGTH], prefix[MAX_LINE_LENGTH];
    
    while (LCReadNextConfig(f, "*Popup", line, sizeof(line))) {
        POPUPLINETYPE type = ProcessPopupLine(line, title, sizeof(title), command, sizeof(command), icon, sizeof(icon), prefix, sizeof(prefix));
        if (level == POPUPLEVEL_ROOT) {
            if (type == POPUPLINETYPE_NEW) {
                // hmm..., need to deal with bangs.
                *out = new Popup(title, command, prefix);
                return LoadPopup(f, POPUPLEVEL_NEW, out);
            }
            else {
                TRACE("Invalid popup line at the root level: %s", line);
                *out = NULL;
                return true;
            }
        }
        else switch (type) {
        case POPUPLINETYPE_FOLDER:
            {
                Popup* popup = new Popup(title, NULL, prefix);
                LoadPopup(f, POPUPLEVEL_FOLDER, &popup);
                (*out)->AddItem(new FolderItem(title, popup, icon));
            }
            break;

        case POPUPLINETYPE_ENDFOLDER:
            {
                if (level == POPUPLEVEL_FOLDER) {
                    return true;
                }
                else {
                    TRACE("Unexpected ~Folder: %s", line);
                }
            }
            break;

        case POPUPLINETYPE_ENDNEW:
            {
                if (level == POPUPLEVEL_NEW) {
                    return true;
                }
                else {
                    TRACE("Unexpected ~New: %s", line);
                }
            }
            break;

        case POPUPLINETYPE_COMMAND:
            {
                (*out)->AddItem(new CommandItem(title, command, icon));
            }
            break;

        case POPUPLINETYPE_INFO:
            {
                (*out)->AddItem(new InfoItem(title, icon));
            }
            break;

        case POPUPLINETYPE_SEPARATOR:
            {
                (*out)->AddItem(new SeparatorItem());
            }
            break;

        case POPUPLINETYPE_NEW:
            {
                TRACE("Unexpected New: %s", line);
            }
            break;

        default: // Failure
            {
                TRACE("Unrecougnized popup line: %s", line);
            }
            break;
        }
    }

    if (level != POPUPLEVEL_ROOT) {
        TRACE("Unexpected end of *popup lines");
    }
    else {
        *out = NULL;
    }

    return false;
}


/// <summary>
/// Extracts information from a *Popup line.
/// </summary>
/// <return>The type of *Popup line this is.</return>
POPUPLINETYPE ProcessPopupLine(LPCSTR line,
                               LPSTR title, UINT cchTitle,
                               LPSTR command, UINT cchCommand,
                               LPSTR icon, UINT cchIcon,
                               LPSTR prefix, UINT cchPrefix) {
    char token[MAX_LINE_LENGTH];
    LPCSTR linePointer = line;
    GetToken(linePointer, NULL, &linePointer, FALSE); // Drop *Popup
    
    // The first token will be ~Folder, ~New, !Separator, !Info, .icon=, or a title.
    GetToken(linePointer, token, &linePointer, FALSE);
    if (_stricmp(token, "~New") == 0) {
        return POPUPLINETYPE_ENDNEW;
    }
    else if (_stricmp(token, "~Folder") == 0) {
        return POPUPLINETYPE_ENDFOLDER;
    }
    else if (_stricmp(token, "!Separator") == 0) {
        return POPUPLINETYPE_SEPARATOR;
    }
    else {
        // If we have a .icon, copy it over and move forward.
        if (_strnicmp(token, ".icon=", 6) == 0) {
            StringCchCopy(icon, cchIcon, token);
            GetToken(linePointer, token, &linePointer, FALSE);
        }
        else {
            icon[0] = '\0';
        }

        if (_stricmp(token, "!Info") == 0) {
            GetToken(linePointer, token, &linePointer, FALSE);
            StringCchCopy(title, cchTitle, token);
            return POPUPLINETYPE_INFO;
        }
        else {
            StringCchCopy(title, cchTitle, token);
            // The token after the title is either !New, Folder, or a command. (TODO::Dynamic stuff).

            // Store a copy to here, if this turns out to be a command
            LPCSTR commandPointer = linePointer;
            GetToken(linePointer, token, &linePointer, FALSE);

            if (_stricmp(token, "!New") == 0) {
                // !New is followed by the bang command
                GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(command, cchCommand, token);
                // Which may be followed by a prefix
                GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(prefix, cchPrefix, token);
                return POPUPLINETYPE_NEW;
            }
            else if (_stricmp(token, "Folder") == 0) {
                // Folder may be followed by a prefix
                GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(prefix, cchPrefix, token);
                return POPUPLINETYPE_FOLDER;
            }
            else {
                StringCchCopy(command, cchCommand, commandPointer);
                return POPUPLINETYPE_COMMAND;
            }
        }
    }
}
