/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.cpp                                                 August, 2012
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "IconGroup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/Debugging.h"

// 
extern HINSTANCE g_hInstance;
extern LPCSTR g_szGroupHandler;


#define WM_SHCHANGE_NOTIFY WM_USER

/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup() {
    WCHAR path[MAX_PATH];

    // Initalize all variables.
    this->changeNotifyUID = 0;

    this->settings = new Settings("DesktopIcons");

    DrawableSettings* defaults = new DrawableSettings();
    defaults->x = 1970;
    defaults->y = 50;
    defaults->width = 1820;
    defaults->height = 1100;
    defaults->color = 0x2200FF00;

    this->settings->GetString("Folder", path, sizeof(path), "Desktop");

    this->window = new DrawableWindow(FindWindow("DesktopBackgroundClass", ""), g_szGroupHandler, g_hInstance, this->settings, defaults);
    SetWindowLongPtr(this->window->GetWindow(), 0, (LONG_PTR)this);
    this->window->Show();

    SetFolder(path);
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (this->changeNotifyUID != 0) {
        SHChangeNotifyDeregister(this->changeNotifyUID);
    }
    SAFERELEASE(this->workingFolder);
    SAFERELEASE(this->rootFolder);
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
}


/// <summary>
/// Destructor
/// </summary>
void IconGroup::SetFolder(LPWSTR folder) {
    PIDLIST_RELATIVE idList, idNext;
    IEnumIDList* enumIDList;

    // Just in case we are switching folders, deregister for old notifications
    if (this->changeNotifyUID != 0) {
        SHChangeNotifyDeregister(this->changeNotifyUID);
    }

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&this->rootFolder));

    // Get the folder we are interested in
    if (_wcsicmp(folder, L"desktop") == 0) {
        SHGetKnownFolderIDList(FOLDERID_Desktop, 0, NULL, &idList);
        SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&this->workingFolder));
    }
    else {
        this->rootFolder->ParseDisplayName(NULL, NULL, folder, NULL, &idList, NULL);
        this->rootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&this->workingFolder));
    }

    // Enumerate the contents of this folder
    this->workingFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {
        AddIcon(idNext);
    }
    enumIDList->Release();

    // Register for change notifications
    SHChangeNotifyEntry watchEntries[] = { idList, TRUE };
    this->changeNotifyUID = SHChangeNotifyRegister(
        this->window->GetWindow(),
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR | SHCNE_RMDIR | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM,
        WM_SHCHANGE_NOTIFY,
        1,
        watchEntries);

    // Let go fo the PIDLists
    CoTaskMemFree(idList);
    CoTaskMemFree(idNext);
}


/// <summary>
/// Add's the icon with the specified ID to the view
/// </summary>
void IconGroup::AddIcon(PCUITEMID_CHILD pidl) {
    IExtractIconW* extractIcon;
    HICON icon;
    WCHAR iconFile[MAX_PATH];
    int iconIndex;
    UINT flags;
    D2D1_RECT_F pos;

    PositionIcon(pidl, &pos);

    // Get the IExtractIcon interface for this item.
    this->workingFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&pidl, IID_IExtractIconW, NULL, reinterpret_cast<LPVOID*>(&extractIcon));

    // Get the location of the file containing the appropriate icon, and the index of the icon.
    extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

    // Extract the icon.
    extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));

    // Add it as an overlay.
    window->AddOverlay(pos, icon);

    // Let go of the interface.
    extractIcon->Release();
}


void IconGroup::PositionIcon(PCUITEMID_CHILD pidl, D2D1_RECT_F* position) {
    static float pos = 0;
    position->bottom = 64;
    position->left = 0 + pos;
    position->right = 64 + pos;
    position->top = 0;
    pos += 72;
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT IconGroup::GetDisplayNameOf(PCUITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf) {
    STRRET ret;
    HRESULT hr = S_OK;

    CHECKHR(hr, this->rootFolder->GetDisplayNameOf(pidl, flags, &ret));
    switch (ret.uType) {
    case STRRET_CSTR:
    case STRRET_OFFSET:
        return E_NOTIMPL;
    case STRRET_WSTR:
        StringCchCopyW(buf, cchBuf, ret.pOleStr);
        CoTaskMemFree(ret.pOleStr);
    }

    CHECKHR_END();

    return hr;
}


/// <summary>
/// 
/// </summary>
LRESULT WINAPI IconGroup::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SHCHANGE_NOTIFY:
        {
            long event;
            PIDLIST_ABSOLUTE* idList;
            WCHAR file1[MAX_PATH], file2[MAX_PATH];
            HANDLE notifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &idList, &event);

            if (notifyLock) {
                if (idList[0]) {
                    GetDisplayNameOf(idList[0], SHGDN_NORMAL, file1, sizeof(file1)/sizeof(WCHAR));
                    if (idList[1]) {
                        GetDisplayNameOf(idList[1], SHGDN_NORMAL, file2, sizeof(file2)/sizeof(WCHAR));
                    }
                }

                switch (event) {
                // The attributes of an item or folder has changed.
                case SHCNE_ATTRIBUTES:
                    {
                        TRACEW(L"The attributes of %s has changed", file1);
                    }
                    break;

                // A non-folder item has been created.
                case SHCNE_CREATE:
                    {
                        TRACEW(L"File created: %s", file1);
                    }
                    break;

                // A non-folder item has been deleted.
                case SHCNE_DELETE:
                    {
                        TRACEW(L"File deleted: %s", file1);
                    }
                    break;

                // A folder has been created.
                case SHCNE_MKDIR:
                    {
                        TRACEW(L"The folder %s was created", file1);
                    }
                    break;

                // A non-folder item has been renamed.
                case SHCNE_RENAMEITEM:
                    {
                        TRACEW(L"Non-Folder renamed: %s -> %s", file1, file2);
                    }
                    break;

                // A folder has been renamed.
                case SHCNE_RENAMEFOLDER:
                    {
                        TRACEW(L"Folder renamed: %s -> %s", file1, file2);
                    }
                    break;
                }

                SHChangeNotification_Unlock(notifyLock);
            }
        }
        return 0;

    default:
        return this->window->HandleMessage(msg, wParam, lParam);
    }
}
