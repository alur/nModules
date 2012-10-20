/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContentPopup.hpp
 *  The nModules Project
 *
 *  A popup which retrives its content from outside LiteStep's .rc files.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "ContentPopup.hpp"
#include "../nShared/Debugging.h"
#include "CommandItem.hpp"
#include "FolderItem.hpp"
#include <Shlwapi.h>
#include <algorithm>
#include "../nShared/PIDL.h"


ContentPopup::ContentPopup(ContentSource source, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->dynamic = false;
    this->source = source;
}


ContentPopup::ContentPopup(LPCSTR path, bool dynamic, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->dynamic = false;
    this->source = ContentSource::PATH;

    char processedPath[MAX_PATH], originalPath[MAX_PATH];
    LPCSTR splitter, end = strchr(path, '\0');
    do {
        splitter = strchr(path, '|');
        StringCchCopyN(originalPath, MAX_PATH, path, (splitter != NULL ? splitter : end) - path);
        PathCanonicalize(processedPath, originalPath);
        PathRemoveBackslash(processedPath);

        this->paths.push_back(_strdup(processedPath));
        path = ++splitter;
    } while (splitter != (LPCSTR)0x1);
}


ContentPopup::~ContentPopup() {
    TRACEW(L"~ContentPopup %s", this->window->GetDrawingSettings()->text);
    for (WATCHFOLDERMAP::const_iterator iter = this->watchedFolders.begin(); iter != this->watchedFolders.end(); ++iter) {
        iter->second.second->Release();
        this->window->ReleaseUserMessage(iter->first);
        SHChangeNotifyDeregister(iter->second.first);
    }
    this->watchedFolders.clear();

    for (list<LPCSTR>::const_iterator iter = this->paths.begin(); iter != this->paths.end(); ++iter) {
        free((LPVOID)*iter);
    }
    this->paths.clear();
}


bool sorter(PopupItem* a, PopupItem* b) {
    return a->CompareTo(b);
}


void ContentPopup::PreShow() {
    TRACEW(L"ContentPopup::PreShow() %s", this->window->GetDrawingSettings()->text);
    if (!this->loaded) {
        LoadContent();
        std::sort(this->items.begin(), this->items.end(), sorter);
        this->loaded = true;
    }
}


void ContentPopup::PostClose() {
    TRACEW(L"ContentPopup::PostClose() %s", this->window->GetDrawingSettings()->text);
    if (this->dynamic) {
        for (WATCHFOLDERMAP::const_iterator iter = this->watchedFolders.begin(); iter != this->watchedFolders.end(); ++iter) {
            iter->second.second->Release();
            this->window->ReleaseUserMessage(iter->first);
            SHChangeNotifyDeregister(iter->second.first);
        }
        this->watchedFolders.clear();

        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); ++iter) {
            delete *iter;
        }
        this->items.clear();

        this->loaded = false;
    }
}


void ContentPopup::LoadContent() {
    TRACEW(L"ContentPopup::LoadContent() %s", this->window->GetDrawingSettings()->text);
    switch (this->source) {
    case ADMIN_TOOLS:
        LoadShellFolder(FOLDERID_AdminTools);
        break;

    case CONTROL_PANEL:
        LoadShellFolder(FOLDERID_ControlPanelFolder, true);
        break;

    case MY_COMPUTER:
        LoadShellFolder(FOLDERID_ComputerFolder);
        break;

    case NETWORK:
        LoadShellFolder(FOLDERID_NetworkFolder);
        break;
    
    case PATH:
        for (list<LPCSTR>::const_iterator iter = paths.begin(); iter != paths.end(); ++iter) {
            LoadPath(*iter);
        }
        break;

    case PRINTERS:
        LoadShellFolder(FOLDERID_PrintersFolder);
        break;

    case RECENT_DOCUMENTS:
        LoadShellFolder(FOLDERID_Recent);
        break;

    case RECYCLE_BIN:
        LoadShellFolder(FOLDERID_RecycleBinFolder);
        break;

    case START_MENU:
        LoadShellFolder(FOLDERID_StartMenu);
        LoadShellFolder(FOLDERID_CommonStartMenu);
        break;

    case PROGRAMS:
        LoadShellFolder(FOLDERID_Programs);
        LoadShellFolder(FOLDERID_CommonPrograms);
        break;
    }
}


void ContentPopup::LoadShellFolder(GUID folder, bool dontExpandFolders) {
    PIDLIST_ABSOLUTE idList;
    IShellFolder *targetFolder, *rootFolder;

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&rootFolder));
    SHGetKnownFolderIDList(folder, NULL, NULL, &idList);
    rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&targetFolder));
    rootFolder->Release();

    //
    LoadFromIDList(targetFolder, idList, dontExpandFolders);

    if (idList != NULL) {
        CoTaskMemFree(idList);
    }
}


void ContentPopup::LoadPath(LPCSTR path) {
    PIDLIST_ABSOLUTE idList = NULL;
    IShellFolder *targetFolder, *rootFolder;
    WCHAR widePath[MAX_PATH];

    MultiByteToWideChar(CP_ACP, NULL, path, -1, widePath, MAX_PATH);

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&rootFolder));
    rootFolder->ParseDisplayName(NULL, NULL, widePath, NULL, &idList, NULL);
    rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&targetFolder));
    rootFolder->Release();
    
    LoadFromIDList(targetFolder, idList, false);

    if (idList != NULL) {
        CoTaskMemFree(idList);
    }
}


void ContentPopup::LoadFromIDList(IShellFolder *targetFolder, PIDLIST_ABSOLUTE idList, bool dontExpandFolders) {
    PIDLIST_RELATIVE idNext = NULL;
    IEnumIDList* enumIDList;

    if (targetFolder == NULL || idList == NULL) {
        return;
    }

    // Enumerate the contents of this folder
    if (SUCCEEDED(targetFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList))) {
        while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {
            LoadSingleItem(targetFolder, idNext, dontExpandFolders);
        }
        enumIDList->Release();
    

        // Register for change notifications
        SHChangeNotifyEntry watchEntries[] = { idList, TRUE };
        UINT message = this->window->RegisterUserMessage(this);
        ULONG shnrUID = SHChangeNotifyRegister(
            this->window->GetWindow(),
            SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
            SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR | SHCNE_RMDIR | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM,
            message,
            1,
            watchEntries);

        this->watchedFolders.insert(WATCHFOLDERMAP::value_type(message, std::pair<UINT, IShellFolder*>(shnrUID, targetFolder)));
    }

    if (idNext) {
        CoTaskMemFree(idNext);
    }
}


void ContentPopup::LoadSingleItem(IShellFolder *targetFolder, PIDLIST_RELATIVE itemID, bool dontExpandFolders) {
    STRRET ret;
    LPSTR name, command;
    IExtractIconW* extractIcon;
    SFGAOF attributes;
    char quotedCommand[MAX_LINE_LENGTH];
    bool openable;
    HRESULT hr;
    PopupItem* item;

    if (SUCCEEDED(targetFolder->GetDisplayNameOf(itemID, SHGDN_NORMAL, &ret))) {
        StrRetToStr(&ret, NULL, &name);
        if (SUCCEEDED(targetFolder->GetDisplayNameOf(itemID, SHGDN_FORPARSING, &ret))) {
            StrRetToStr(&ret, NULL, &command);

            // 
            attributes = SFGAO_BROWSABLE | SFGAO_FOLDER;
            hr = targetFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&itemID, &attributes);
            openable = SUCCEEDED(hr) && !dontExpandFolders && (((attributes & SFGAO_FOLDER) == SFGAO_FOLDER) || ((attributes & SFGAO_BROWSABLE) == SFGAO_BROWSABLE));

            if (openable) {
                item = new nPopup::FolderItem(this, name, new ContentPopup(command, this->dynamic, name, NULL, this->settings->prefix));
            }
            else {
                StringCchPrintf(quotedCommand, sizeof(quotedCommand), "\"%s\"", command);
                item = new CommandItem(this, name, quotedCommand);
            }

            if (!this->noIcons) {
                // Get the IExtractIcon interface for this item.
                hr = targetFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&itemID, IID_IExtractIconW, NULL, reinterpret_cast<LPVOID*>(&extractIcon));

                if (SUCCEEDED(hr)) {
                    item->SetIcon(extractIcon);
                }
            }

            AddItem(item);

            CoTaskMemFree(command);
        }
        CoTaskMemFree(name);
    }
}


LRESULT WINAPI ContentPopup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message >= WM_USER) {
        WATCHFOLDERMAP::const_iterator folder = this->watchedFolders.find(message);
        if (folder != this->watchedFolders.end()) {
            long event;
            PIDLIST_ABSOLUTE* idList;
            HANDLE notifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &idList, &event);

            if (notifyLock) {
                // TODO::Have to handle deletes and renames as well
                switch (event) {
                case SHCNE_CREATE:
                case SHCNE_MKDIR:
                    {
                        LoadSingleItem(folder->second.second, (PIDLIST_RELATIVE)PIDL::GetLastPIDLItem(idList[0]), false);
                        std::sort(this->items.begin(), this->items.end(), sorter);
                    }
                    break;

                // A non-folder item has been renamed.
                case SHCNE_RENAMEITEM:
                    {
                        //TRACEW(L"Non-Folder renamed: %s -> %s", file1, file2);
                    }
                    break;

                // A folder has been renamed.
                case SHCNE_RENAMEFOLDER:
                    {
                        //TRACEW(L"Folder renamed: %s -> %s", file1, file2);
                    }
                    break;
                }

                SHChangeNotification_Unlock(notifyLock);
            }
            return 0;
        }
    }
    return Popup::HandleMessage(window, message, wParam, lParam); 
}
