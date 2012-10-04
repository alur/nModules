/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "IconGroup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/Debugging.h"
#include "../nShared/LSModule.hpp"

// 
extern LSModule* g_LSModule;

#define WM_SHCHANGE_NOTIFY WM_USER


/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup(LPCSTR pszPrefix) {
    WCHAR path[MAX_PATH];

    // Initalize all variables.
    this->changeNotifyUID = 0;

    this->settings = new Settings(pszPrefix);

    DrawableSettings* defaults = new DrawableSettings();
    defaults->color = 0x2000FF00;

    this->settings->GetString("Folder", path, sizeof(path), "Desktop");

    this->window = new DrawableWindow(FindWindow("DesktopBackgroundClass", ""), (LPCSTR)g_LSModule->GetWindowClass(1), g_LSModule->GetInstance(), this->settings, defaults, this);
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

    for (vector<Icon*>::const_iterator iter = icons.begin(); iter != icons.end(); iter++) {
        delete *iter;
    }
    icons.clear();

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
void IconGroup::AddIcon(PCITEMID_CHILD pidl) {
    D2D1_RECT_F pos;
    PositionIcon(pidl, &pos);
    Icon* icon = new Icon(pidl, this->workingFolder, this->window, this->settings);
    icon->SetPosition((int)pos.left, (int)pos.top);
    icons.push_back(icon);
}

PCITEMID_CHILD IconGroup::GetLastPIDLItem(LPITEMIDLIST pidl) {
    LPITEMIDLIST ret = pidl;
    USHORT lastCB;
    while (ret->mkid.cb != 0) {
        lastCB = ret->mkid.cb;
        ret = LPITEMIDLIST(((LPBYTE)ret)+lastCB);
    }
    return LPITEMIDLIST(((LPBYTE)ret)-lastCB);
}


void IconGroup::PositionIcon(PCITEMID_CHILD pidl, D2D1_RECT_F* position) {
    static float pos = 5;
    position->bottom = 64;
    position->left = 0 + pos;
    position->right = 64 + pos;
    position->top = 5;
    pos += 80;
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT IconGroup::GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf) {
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
LRESULT WINAPI IconGroup::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
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
                        AddIcon(GetLastPIDLItem(idList[0]));
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
                        AddIcon(GetLastPIDLItem(idList[0]));
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
        return this->window->HandleMessage(window, msg, wParam, lParam);
    }
}
