/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nPopup.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nPopup module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "CommandItem.hpp"
#include "ContainerItem.hpp"
#include "ContentPopup.hpp"
#include "FolderItem.hpp"
#include "FolderPopup.hpp"
#include "InfoItem.hpp"
#include "Popup.hpp"
#include "PopupItem.hpp"
#include "SeparatorItem.hpp"
#include "SuicidalContentPopup.hpp"
#include "Version.h"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/UnorderedMap.hpp"

#include <map>
#include <strsafe.h>
#include <unordered_map>


/// <summary>
/// The possibly types for a *Popup line.
/// </summary>
enum class PopupLineType
{
    EndNew, // ~New
    EndFolder, // ~Folder
    New, // !New
    Folder, // Folder
    Separator, // !Separator
    Info, // !Info
    Command, // Anything
    Content, // Retrieves its items from an external source
    ContentPath, // Retrieves its items from a path
    ContentPathDynamic, // Retrieves its items from a path, reloading the items each time it is opened
    Invalid, // An invalid line
    Container
};

/// <summary>
/// The level we are currently reading from.
/// </summary>
enum class PopupLevel
{
    Folder,
    New,
    Root
};


static PopupLineType ProcessPopupLine(
    LPCTSTR line, ContentPopup::ContentSource & source,
    LPTSTR title, UINT cchTitle,
    LPTSTR command, UINT cchCommand,
    LPTSTR icon, UINT cchIcon,
    LPTSTR prefix, UINT cchPrefix);
static void LoadPopups();
static bool LoadPopup(LPVOID f, PopupLevel level, Popup * & out, LPCTSTR parentPrefix);
void __cdecl HandlePopupBang(HWND owner, LPCTSTR bang, LPCTSTR args);


// The LSModule class.
LSModule gLSModule(_T(MODULE_NAME), _T(MODULE_AUTHOR), MakeVersion(MODULE_VERSION));

// The messages we want from the core.
static UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All root level popups.
static UnorderedCaselessCStringMap<Popup*> gRootPopups;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
/// <param name="parent"></param>
/// <param name="instance">Handle to this module's instance.</param>
/// <param name="path">Path to the LiteStep directory.</param>
/// <returns>0 on success, non-zero on error.</returns>
/// <remarks>
/// If this function returns non-zero, the module will be unloaded immediately,
/// without going through quitModule.
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
    LoadPopups();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
/// <param name="instance">Handle to this module's instance.</param>
EXPORT_CDECL(void) quitModule(HINSTANCE instance)
{
    UNREFERENCED_PARAMETER(instance);

    for (auto & popup : gRootPopups)
    {
        LiteStep::RemoveBangCommand(popup.second->GetBang());
        delete popup.second;
    }
    LiteStep::RemoveBangCommand(TEXT("!PopupDynamicFolder"));
    gLSModule.DeInitalize();
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
    switch(message)
    {
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
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Called when a popup bang is executed.
/// </summary>
void __cdecl HandlePopupBang(HWND /* owner */, LPCTSTR bang, LPCTSTR /* args */)
{
    Popup * popup = gRootPopups.Get(bang, nullptr);
    if (popup != nullptr)
    {
        popup->Show();
    }
}


/// <summary>
/// Adds a new root-level popup.
/// </summary>
void AddPopup(Popup * popup)
{
    gRootPopups.Emplace(popup->GetBang(), popup);
    LiteStep::AddBangCommandEx(popup->GetBang(), HandlePopupBang);
}


/// <summary>
/// Reads through the .rc files and creates Taskbars.
/// </summary>
void LoadPopups()
{
    LPVOID f = LiteStep::LCOpen(nullptr);
    Popup * popup;

    // Add pre-defined popups
    AddPopup(new ContentPopup(ContentPopup::ContentSource::ADMIN_TOOLS, _T("Admin Tools"), _T("!PopupAdminTools"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::CONTROL_PANEL, _T("Control Panel"), _T("!PopupControlPanel"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::MY_COMPUTER, _T("My Computer"), _T("!PopupMyComputer"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::NETWORK, _T("Network"), _T("!PopupNetwork"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::PRINTERS, _T("Printers"), _T("!PopupPrinters"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::PROGRAMS, _T("Programs"), _T("!PopupPrograms"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::RECENT_DOCUMENTS, _T("Recent Documents"), _T("!PopupRecentDocuments"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::RECYCLE_BIN, _T("Recycle Bin"), _T("!PopupRecycleBin"), _T("nPopup")));
    AddPopup(new ContentPopup(ContentPopup::ContentSource::START_MENU, _T("Start Menu"), _T("!PopupStartMenu"), _T("nPopup")));

    // Load .rc popups
    while (LoadPopup(f, PopupLevel::Root, popup, L"nPopup"))
    {
        if (popup != nullptr)
        {
            AddPopup(popup);
        }
    }

    // Add bang for handling dynamic popups
    LiteStep::AddBangCommand(L"!PopupDynamicFolder", [] (HWND, LPCTSTR args) -> void
    {
        TCHAR path[MAX_PATH], title[MAX_PATH], prefix[MAX_RCCOMMAND];
        LPTSTR buffers[] = { path, prefix, title };

        int nArgs = LiteStep::CommandTokenize(args, buffers, _countof(buffers), nullptr);

        switch (nArgs)
        {
        case 1:
            // TODO::Extract the name of the folder and use it as the title.
            return (new SuicidalContentPopup(path, L"", L"nPopup"))->Show();

        case 2:
            // TODO::Extract the name of the folder and use it as the title.
            return (new SuicidalContentPopup(path, L"", prefix))->Show();

        case 3:
            return (new SuicidalContentPopup(path, title, prefix))->Show();
        }
    });

    LiteStep::LCClose(f);
}


/// <summary>
/// Loads a popup.
/// </summary>
/// <return>Returns false when all lines have been read.</return>
bool LoadPopup(LPVOID f, PopupLevel level, Popup * & out, LPCTSTR parentPrefix)
{
    TCHAR line[MAX_LINE_LENGTH], title[MAX_LINE_LENGTH], command[MAX_LINE_LENGTH], icon[MAX_LINE_LENGTH], prefix[MAX_LINE_LENGTH];
    ContentPopup::ContentSource source;
    
    while (LiteStep::LCReadNextConfig(f, L"*Popup", line, _countof(line)))
    {
        PopupLineType type = ProcessPopupLine(line, source, title, _countof(title), command, _countof(command), icon, _countof(icon), prefix, _countof(prefix));
        if (level == PopupLevel::Root)
        {
            if (type == PopupLineType::New)
            {
                out = new FolderPopup(title, command, prefix[0] == L'\0' ? parentPrefix : prefix);
                return LoadPopup(f, PopupLevel::New, out, prefix[0] == L'\0' ? parentPrefix : prefix);
            }
            else
            {
                TRACE("Invalid popup line at the root level: %ls", line);
                out = nullptr;
                return true;
            }
        }
        else switch (type)
        {
        case PopupLineType::Folder:
            {
                Popup* popup = new FolderPopup(title, nullptr, prefix[0] == L'\0' ? parentPrefix : prefix);
                LoadPopup(f, PopupLevel::Folder, popup, prefix[0] == L'\0' ? parentPrefix : prefix);
                out->AddItem(new nPopup::FolderItem(out, title, popup, icon));
            }
            break;

        case PopupLineType::EndFolder:
            {
                if (level == PopupLevel::Folder)
                {
                    return true;
                }
                else
                {
                    TRACE("Unexpected ~Folder: %s", line);
                }
            }
            break;

        case PopupLineType::EndNew:
            {
                if (level == PopupLevel::New)
                {
                    return true;
                }
                else
                {
                    TRACE("Unexpected ~New: %s", line);
                }
            }
            break;

        case PopupLineType::Command:
            {
                out->AddItem(new CommandItem(out, title, command, icon));
            }
            break;

        case PopupLineType::Content:
            {
                out->AddItem(new nPopup::FolderItem(out, title, new ContentPopup(source, title, command, prefix[0] == L'\0' ? parentPrefix : prefix), icon));
            }
            break;

        case PopupLineType::ContentPath:
            {
                out->AddItem(new nPopup::FolderItem(out, title, new ContentPopup(command, false, title, L"", prefix[0] == L'\0' ? parentPrefix : prefix), icon));
            }
            break;

        case PopupLineType::ContentPathDynamic:
            {
                out->AddItem(new nPopup::FolderItem(out, title, new ContentPopup(command, true, title, L"", prefix[0] == L'\0' ? parentPrefix : prefix), icon));
            }
            break;

        case PopupLineType::Info:
            {
                out->AddItem(new InfoItem(out, title, icon));
            }
            break;

        case PopupLineType::Separator:
            {
                out->AddItem(new SeparatorItem(out));
            }
            break;

        case PopupLineType::Container:
            {
                out->AddItem(new ContainerItem(out, prefix));
            }
            break;

        case PopupLineType::New:
            {
                TRACE("Unexpected New: %s", line);
            }
            break;

        case PopupLineType::Invalid:
            {
                TRACE("Invalid popup line: %s", line);
            }
            break;

        default: // Failure
            {
                TRACE("Unrecougnized popup line: %s", line);
            }
            break;
        }
    }

    if (level != PopupLevel::Root)
    {
        TRACE("Unexpected end of *popup lines");
    }
    else
    {
        out = nullptr;
    }

    return false;
}


/// <summary>
/// Extracts information from a *Popup line.
/// </summary>
/// <returns>The type of *Popup line this is.</returns>
PopupLineType ProcessPopupLine(LPCTSTR line, ContentPopup::ContentSource & source,
                               LPTSTR title, UINT cchTitle,
                               LPTSTR command, UINT cchCommand,
                               LPTSTR icon, UINT cchIcon,
                               LPTSTR prefix, UINT cchPrefix)
{
    TCHAR token[MAX_LINE_LENGTH];
    LPCTSTR linePointer = line;

    LiteStep::GetToken(linePointer, nullptr, &linePointer, FALSE); // Drop *Popup
    
    // The first token will be ~Folder, ~New, !Separator, !Info, !Container, .icon=, or a title.
    LiteStep::GetToken(linePointer, token, &linePointer, FALSE);
    if (_tcsicmp(token, _T("~New")) == 0)
    {
        return PopupLineType::EndNew;
    }
    else if (_tcsicmp(token, _T("~Folder")) == 0)
    {
        return PopupLineType::EndFolder;
    }
    else if (_tcsicmp(token, _T("!Separator")) == 0)
    {
        return PopupLineType::Separator;
    }
    else if (_tcsicmp(token, _T("!Container")) == 0)
    {
        // The next token should be a prefix.
        if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) == FALSE)
        {
            return PopupLineType::Invalid;
        }
        StringCchCopy(prefix, cchPrefix, token);
        return PopupLineType::Container;
    }
    else
    {
        // If we have a .icon, copy it over and move forward.
        if (_tcsnicmp(token, _T(".icon="), 6) == 0)
        {
            StringCchCopy(icon, cchIcon, token+6);
            if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) == FALSE)
            {
                return PopupLineType::Invalid; // Ending after .icon=
            }
        }
        else
        {
            *icon = L'\0';
        }

        if (_tcsicmp(token, _T("!Info")) == 0)
        {
            if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) != FALSE)
            {
                StringCchCopy(title, cchTitle, token);
            }
            else
            {
                *title = L'\0';
            }
            return PopupLineType::Info;
        }
        else
        {
            StringCchCopy(title, cchTitle, token);
            // The token after the title is either !New, Folder, or a command.

            // Store a copy to here, if this turns out to be a command
            LPCTSTR commandPointer = linePointer;

            // This would be an empty command, or something, might as well mark it invalid.
            if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) == FALSE)
            {
                return PopupLineType::Invalid;
            }

            //
            PopupLineType type;

            if (_tcsicmp(token, _T("!New")) == 0)
            {
                // !New is followed by the bang command
                if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) == FALSE)
                {
                    return PopupLineType::Invalid;
                }
                StringCchCopy(command, cchCommand, token);
                type = PopupLineType::New;
            }
            else if (_tcsicmp(token, _T("Folder")) == 0)
            {
                type = PopupLineType::Folder;
            }
            else if (_tcsicmp(token, _T("!PopupAdminTools")) == 0) 
            {
                source = ContentPopup::ContentSource::ADMIN_TOOLS;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupControlPanel")) == 0)
            {
                source = ContentPopup::ContentSource::CONTROL_PANEL;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupMyComputer")) == 0)
            {
                source = ContentPopup::ContentSource::MY_COMPUTER;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupNetwork")) == 0)
            {
                source = ContentPopup::ContentSource::NETWORK;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupPrinters")) == 0)
            {
                source = ContentPopup::ContentSource::PRINTERS;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupPrograms")) == 0)
            {
                source = ContentPopup::ContentSource::PROGRAMS;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupRecentDocuments")) == 0)
            {
                source = ContentPopup::ContentSource::RECENT_DOCUMENTS;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupRecycleBin")) == 0)
            {
                source = ContentPopup::ContentSource::RECYCLE_BIN;
                type = PopupLineType::Content;
            }
            else if (_tcsicmp(token, _T("!PopupStartMenu")) == 0)
            {
                source = ContentPopup::ContentSource::START_MENU;
                type = PopupLineType::Content;
            }
            else if (_tcsnicmp(token, _T("!PopupFolder:"), _countof("!PopupFolder:") - 1) == 0)
            {
                source = ContentPopup::ContentSource::PATH;
                StringCchCopy(command, cchCommand, commandPointer + _countof("!PopupFolder:"));
                command[_tcslen(command)-1] = L'\0';
                type = PopupLineType::ContentPath;
            }
            else if (_tcsnicmp(token, _T("!PopupDynamicFolder:"), _countof("!PopupDynamicFolder:") - 1) == 0)
            {
                source = ContentPopup::ContentSource::PATH;
                StringCchCopy(command, cchCommand, commandPointer + _countof("!PopupDynamicFolder:"));
                command[_tcslen(command)-1] = L'\0';
                type = PopupLineType::ContentPathDynamic;
            }
            else
            {
                StringCchCopy(command, cchCommand, commandPointer);
                type = PopupLineType::Command;
            }

            // Everything, save commands, may be followed by a prefix.
            if (type != PopupLineType::Command)
            {
                if (LiteStep::GetToken(linePointer, token, &linePointer, FALSE) != FALSE)
                {
                    StringCchCopy(prefix, cchPrefix, token);
                }
                else
                {
                    prefix[0] = L'\0';
                }
            }

            return type;
        }
    }
}
