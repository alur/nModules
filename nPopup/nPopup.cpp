/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nPopup module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
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
#include "ContentPopup.hpp"
#include "FolderPopup.hpp"
#include "nPopup.h"


// The LSModule class
LSModule* g_LSModule;

// The messages we want from the core
UINT g_lsMessages[] = { LM_GETREVID, LM_REFRESH, NULL };

// All root level popups
vector<Popup*> rootPopups;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* szPath */) {
    g_LSModule = new LSModule(parent, "nPopup", "Alurcard2", MAKE_VERSION(0,2,0,0), instance);
    
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

    return 0;
}


/// <summary>
/// Called by the core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* hDllInstance */) {
    for (vector<Popup*>::const_iterator iter = rootPopups.begin(); iter != rootPopups.end(); iter++) {
        LiteStep::RemoveBangCommand((*iter)->GetBang());
        delete *iter;
    }

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
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// 
/// </summary>
void __cdecl HandlePopupBang(HWND /* owner */, LPCSTR bang, LPCSTR args) {
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
/// Adds a new root-level popup
/// </summary>
void AddPopup(Popup* popup) {
    rootPopups.push_back(popup);
    LiteStep::AddBangCommandEx(popup->GetBang(), HandlePopupBang);
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadPopups() {
    LPVOID f = LiteStep::LCOpen(NULL);
    Popup* popup;

    // Add pre-defined popups
    AddPopup(new ContentPopup(ContentPopup::ContentSource::ADMIN_TOOLS, "Admin Tools", "!PopupAdminTools", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::CONTROL_PANEL, "Control Panel", "!PopupControlPanel", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::MY_COMPUTER, "My Computer", "!PopupMyComputer", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::NETWORK, "Network", "!PopupNetwork", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::PRINTERS, "Printers", "!PopupPrinters", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::PROGRAMS, "Programs", "!PopupPrograms", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::RECENT_DOCUMENTS, "Recent Documents", "!PopupRecentDocuments", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::RECYCLE_BIN, "Recycle Bin", "!PopupRecycleBin", "nPopup"));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::START_MENU, "Start Menu", "!PopupStartMenu", "nPopup"));

    // Load .rc popups
    while (LoadPopup(f, POPUPLEVEL_ROOT, &popup)) {
        if (popup != NULL) {
            AddPopup(popup);
        }
    }

    LiteStep::LCClose(f);
}


/// <summary>
/// Loads a popup
/// </summary>
/// <return>Returns false when all lines have been read.</return>
bool LoadPopup(LPVOID f, POPUPLEVEL level, Popup** out) {
    char line[MAX_LINE_LENGTH], title[MAX_LINE_LENGTH], command[MAX_LINE_LENGTH], icon[MAX_LINE_LENGTH], prefix[MAX_LINE_LENGTH];
    ContentPopup::ContentSource source;
    
    while (LiteStep::LCReadNextConfig(f, "*Popup", line, sizeof(line))) {
        POPUPLINETYPE type = ProcessPopupLine(line, &source, title, sizeof(title), command, sizeof(command), icon, sizeof(icon), prefix, sizeof(prefix));
        if (level == POPUPLEVEL_ROOT) {
            if (type == POPUPLINETYPE_NEW) {
                *out = new FolderPopup(title, command, prefix);
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
                Popup* popup = new FolderPopup(title, NULL, prefix);
                LoadPopup(f, POPUPLEVEL_FOLDER, &popup);
                (*out)->AddItem(new nPopup::FolderItem(*out, title, popup, icon));
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
                (*out)->AddItem(new CommandItem(*out, title, command, icon));
            }
            break;

        case POPUPLINETYPE_CONTENT:
            {
                (*out)->AddItem(new nPopup::FolderItem(*out, title, new ContentPopup(source, title, command, prefix), icon));
            }
            break;

        case POPUPLINETYPE_CONTENTPATH:
            {
                (*out)->AddItem(new nPopup::FolderItem(*out, title, new ContentPopup(command, false, title, "", prefix), icon));
            }
            break;

        case POPUPLINETYPE_CONTENTPATHDYNAMIC:
            {
                (*out)->AddItem(new nPopup::FolderItem(*out, title, new ContentPopup(command, true, title, "", prefix), icon));
            }
            break;

        case POPUPLINETYPE_INFO:
            {
                (*out)->AddItem(new InfoItem(*out, title, icon));
            }
            break;

        case POPUPLINETYPE_SEPARATOR:
            {
                (*out)->AddItem(new SeparatorItem(*out));
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
POPUPLINETYPE ProcessPopupLine(LPCSTR line, ContentPopup::ContentSource* source,
                               LPSTR title, UINT cchTitle,
                               LPSTR command, UINT cchCommand,
                               LPSTR icon, UINT cchIcon,
                               LPSTR prefix, UINT cchPrefix) {
    char token[MAX_LINE_LENGTH];
    LPCSTR linePointer = line;
    LiteStep::GetToken(linePointer, NULL, &linePointer, FALSE); // Drop *Popup
    
    // The first token will be ~Folder, ~New, !Separator, !Info, .icon=, or a title.
    LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
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
            StringCchCopy(icon, cchIcon, token+6);
            LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
        }
        else {
            icon[0] = '\0';
        }

        if (_stricmp(token, "!Info") == 0) {
            LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
            StringCchCopy(title, cchTitle, token);
            return POPUPLINETYPE_INFO;
        }
        else {
            StringCchCopy(title, cchTitle, token);
            // The token after the title is either !New, Folder, or a command. (TODO::Dynamic stuff).

            // Store a copy to here, if this turns out to be a command
            LPCSTR commandPointer = linePointer;
            LiteStep::GetToken(linePointer, token, &linePointer, FALSE);

            if (_stricmp(token, "!New") == 0) {
                // !New is followed by the bang command
                LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(command, cchCommand, token);
                // Which may be followed by a prefix
                LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(prefix, cchPrefix, token);
                return POPUPLINETYPE_NEW;
            }
            else if (_stricmp(token, "Folder") == 0) {
                // Folder may be followed by a prefix
                LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
                StringCchCopy(prefix, cchPrefix, token);
                return POPUPLINETYPE_FOLDER;
            }
            else if (_stricmp(token, "!PopupAdminTools") == 0) {
                *source = ContentPopup::ContentSource::ADMIN_TOOLS;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupControlPanel") == 0) {
                *source = ContentPopup::ContentSource::CONTROL_PANEL;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupMyComputer") == 0) {
                *source = ContentPopup::ContentSource::MY_COMPUTER;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupNetwork") == 0) {
                *source = ContentPopup::ContentSource::NETWORK;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupPrinters") == 0) {
                *source = ContentPopup::ContentSource::PRINTERS;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupPrograms") == 0) {
                *source = ContentPopup::ContentSource::PROGRAMS;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupRecentDocuments") == 0) {
                *source = ContentPopup::ContentSource::RECENT_DOCUMENTS;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupRecycleBin") == 0) {
                *source = ContentPopup::ContentSource::RECYCLE_BIN;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_stricmp(token, "!PopupStartMenu") == 0) {
                *source = ContentPopup::ContentSource::START_MENU;
                return POPUPLINETYPE_CONTENT;
            }
            else if (_strnicmp(token, "!PopupFolder:", strlen("!PopupFolder:")) == 0) {
                *source = ContentPopup::ContentSource::PATH;
                StringCchCopy(command, cchCommand, commandPointer + strlen("!PopupFolder:") + 1);
                command[strlen(command)-1] = 0;
                return POPUPLINETYPE_CONTENTPATH;
            }
            else if (_strnicmp(token, "!PopupDynamicFolder:", strlen("!PopupDynamicFolder:")) == 0) {
                *source = ContentPopup::ContentSource::PATH;
                StringCchCopy(command, cchCommand, commandPointer + strlen("!PopupDynamicFolder:") + 1);
                command[strlen(command)-1] = 0;
                return POPUPLINETYPE_CONTENTPATHDYNAMIC;
            }
            else {
                StringCchCopy(command, cchCommand, commandPointer);
                return POPUPLINETYPE_COMMAND;
            }
        }
    }
}
