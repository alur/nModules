/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContentPopup.hpp
 *  The nModules Project
 *
 *  A popup which retrives its content from outside LiteStep's .rc files.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "ContentPopup.hpp"
#include "../nShared/Debugging.h"
#include "CommandItem.hpp"
#include <ShlObj.h>
#include <Shlwapi.h>
#include <algorithm>


ContentPopup::ContentPopup(ContentSource source, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->dynamic = false;
    this->path = NULL;
    this->source = source;
}


ContentPopup::ContentPopup(LPCSTR path, bool dynamic, LPCSTR title, LPCSTR bang, LPCSTR prefix) : Popup(title, bang, prefix) {
    this->loaded = false;
    this->path = _strdup(path);
    this->dynamic = dynamic;
}


ContentPopup::~ContentPopup() {
    if (path != NULL) {
        free((LPVOID)path);
    }
}


bool sorter(PopupItem* a, PopupItem* b) {
    return a->CompareTo(b);
}


void ContentPopup::PreShow() {
    if (!this->loaded) {
        LoadContent();
        this->loaded = true;
        std::sort(this->items.begin(), this->items.end(), sorter);
    }
    TRACE("ContentPopup::PreShow - %x", this);
}


void ContentPopup::PostClose() {
    if (this->dynamic) {
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            delete *iter;
        }
        this->items.clear();
    }
}


void ContentPopup::LoadContent() {
    switch (this->source) {
    case CONTENT_SOURCE_CONTROLPANEL:
        {
            LoadShellFolder(FOLDERID_ControlPanelFolder);
        }
        break;
    }
}


void ContentPopup::LoadShellFolder(GUID folder) {
    PIDLIST_ABSOLUTE idList;
    PIDLIST_RELATIVE idNext;
    IEnumIDList* enumIDList;
    IShellFolder2 *targetFolder, *rootFolder;
    STRRET ret;
    LPSTR name, command;
    IExtractIconW* extractIcon;
    HICON icon;
    WCHAR iconFile[MAX_PATH];
    int iconIndex;
    UINT flags;

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&rootFolder));
    SHGetKnownFolderIDList(folder, NULL, NULL, &idList);
    rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&targetFolder));

    // Enumerate the contents of this folder
    targetFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {
        if (SUCCEEDED(targetFolder->GetDisplayNameOf(idNext, SHGDN_NORMAL, &ret))) {
            StrRetToStr(&ret, NULL, &name);
            if (SUCCEEDED(targetFolder->GetDisplayNameOf(idNext, SHGDN_FORPARSING, &ret))) {
                StrRetToStr(&ret, NULL, &command);

                // Get the IExtractIcon interface for this item.
                if (SUCCEEDED(targetFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&idNext, IID_IExtractIconW, NULL, reinterpret_cast<LPVOID*>(&extractIcon)))) {

                    // Get the location of the file containing the appropriate icon, and the index of the icon.
                    extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

                    // Extract the icon.
                    HRESULT hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
                    if (hr == S_FALSE) {
                        // If the extraction failed, fall back to a 32x32 icon.
                        extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));
                    }

                    // Add it as an overlay.
                    AddItem(new CommandItem(this, name, command, icon));

                    // Let go of the interface.
                    extractIcon->Release();
                }
                else {
                    AddItem(new CommandItem(this, name, command));
                }

                CoTaskMemFree(command);
            }
            CoTaskMemFree(name);
        }
    }
    enumIDList->Release();

    // Let go fo the PIDLists
    CoTaskMemFree(idList);
    CoTaskMemFree(idNext);

    // Free the IShellFolders
    targetFolder->Release();
    rootFolder->Release();
}
