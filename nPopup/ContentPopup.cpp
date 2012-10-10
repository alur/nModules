/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContentPopup.hpp
 *  The nModules Project
 *
 *  A popup which retrives its content from outside LiteStep's .rc files.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "ContentPopup.hpp"
#include "../nShared/Debugging.h"
#include "CommandItem.hpp"
#include "FolderItem.hpp"
#include <Shlwapi.h>
#include <algorithm>


ContentPopup::ContentPopup(ContentSource source, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->dynamic = false;
    this->source = source;
}


ContentPopup::ContentPopup(LPCSTR path, bool dynamic, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->dynamic = dynamic;
    this->source = ContentSource::PATH;
    paths.push_back(_strdup(path));
}


ContentPopup::~ContentPopup() {
    for (list<LPCSTR>::const_iterator iter = paths.begin(); iter != paths.end(); ++iter) {
        free((LPVOID)*iter);
    }
    paths.clear();
}


bool sorter(PopupItem* a, PopupItem* b) {
    return a->CompareTo(b);
}


void ContentPopup::PreShow() {
    if (!this->loaded) {
        LoadContent();
        std::sort(this->items.begin(), this->items.end(), sorter);
        this->loaded = true;
    }
}


void ContentPopup::PostClose() {
    if (this->dynamic) {
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            delete *iter;
        }
        this->items.clear();
        this->loaded = false;
    }
}


void ContentPopup::LoadContent() {
    switch (this->source) {
    case ADMIN_TOOLS:
        LoadShellFolder(FOLDERID_AdminTools);
        break;

    case CONTROL_PANEL:
        LoadShellFolder(FOLDERID_ControlPanelFolder);
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


void ContentPopup::LoadShellFolder(GUID folder) {
    PIDLIST_ABSOLUTE idList;
    IShellFolder *targetFolder, *rootFolder;

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&rootFolder));
    SHGetKnownFolderIDList(folder, NULL, NULL, &idList);
    rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&targetFolder));

    LoadFromIDList(targetFolder, idList);

    CoTaskMemFree(idList);
    rootFolder->Release();
}


void ContentPopup::LoadPath(LPCSTR path) {
    PIDLIST_ABSOLUTE idList;
    IShellFolder *targetFolder, *rootFolder;
    WCHAR widePath[MAX_PATH];

    mbstowcs(widePath, path, MAX_PATH);

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&rootFolder));
    rootFolder->ParseDisplayName(NULL, NULL, widePath, NULL, &idList, NULL);
    rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&targetFolder));

    LoadFromIDList(targetFolder, idList);

    CoTaskMemFree(idList);
    rootFolder->Release();
}


void ContentPopup::LoadFromIDList(IShellFolder *targetFolder, PIDLIST_ABSOLUTE idList) {
    PIDLIST_RELATIVE idNext;
    IEnumIDList* enumIDList;
    STRRET ret;
    LPSTR name, command;
    IExtractIconW* extractIcon;
    HICON icon;
    WCHAR iconFile[MAX_PATH];
    int iconIndex;
    UINT flags;
    SFGAOF attributes;
    char quotedCommand[MAX_LINE_LENGTH];

    // Enumerate the contents of this folder
    targetFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {


        if (SUCCEEDED(targetFolder->GetDisplayNameOf(idNext, SHGDN_NORMAL, &ret))) {
            StrRetToStr(&ret, NULL, &name);
            if (SUCCEEDED(targetFolder->GetDisplayNameOf(idNext, SHGDN_FORPARSING, &ret))) {
                StrRetToStr(&ret, NULL, &command);

                //
                targetFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&idNext, &attributes);

                // Get the IExtractIcon interface for this item.
                HRESULT hr = targetFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&idNext, IID_IExtractIconW, NULL, reinterpret_cast<LPVOID*>(&extractIcon));

                if (SUCCEEDED(hr)) {
                    // Get the location of the file containing the appropriate icon, and the index of the icon.
                    extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

                    // Extract the icon.
                    hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
                    if (hr == S_FALSE) {
                        // If the extraction failed, fall back to a 32x32 icon.
                        hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));

                        if (FAILED(hr)) {
                            hr = extractIcon->Extract(iconFile, iconIndex, NULL, &icon, MAKELONG(0, 16));
                        }
                    }

                    // Let go of the interface.
                    extractIcon->Release();
                }


                bool openable = ((attributes & SFGAO_FOLDER) == SFGAO_FOLDER) || ((attributes & SFGAO_BROWSABLE) == SFGAO_BROWSABLE);

                if (hr == S_OK) {
                    if (openable) {
                        AddItem(new FolderItem(this, name, new ContentPopup(command, false, name, NULL, this->settings->prefix), icon));
                    }
                    else {
                        StringCchPrintf(quotedCommand, sizeof(quotedCommand), "\"%s\"", command);
                        AddItem(new CommandItem(this, name, quotedCommand, icon));
                    }
                }
                else {
                    if (openable) {
                        AddItem(new FolderItem(this, name, new ContentPopup(command, false, name, NULL, this->settings->prefix)));
                    }
                    else {
                        StringCchPrintf(quotedCommand, sizeof(quotedCommand), "\"%s\"", command);
                        AddItem(new CommandItem(this, name, quotedCommand));
                    }
                }

                CoTaskMemFree(command);
            }
            CoTaskMemFree(name);
        }
    }
    enumIDList->Release();

    CoTaskMemFree(idNext);
    targetFolder->Release();
}