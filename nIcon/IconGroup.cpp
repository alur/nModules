/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <windowsx.h>
#include <strsafe.h>
#include "IconGroup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/Debugging.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/PIDL.h"

// 
extern LSModule* g_LSModule;


/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup(LPCSTR prefix) : Drawable(prefix) {
    WCHAR path[MAX_PATH];

    // Initalize all variables.
    this->changeNotifyUID = 0;

    DrawableSettings* defaults = new DrawableSettings();
    defaults->color = 0x2000FF00;

    this->settings->GetString("Folder", path, sizeof(path), "Desktop");

    this->window->Initialize(defaults);
    //SetParent(this->window->GetWindow(), FindWindow("DesktopBackgroundClass", NULL));
    this->window->Show();

    this->changeNotifyMsg = this->window->RegisterUserMessage(this);

    SetFolder(path);
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (this->changeNotifyUID != 0) {
        SHChangeNotifyDeregister(this->changeNotifyUID);
    }

    if (this->changeNotifyMsg) {
        this->window->ReleaseUserMessage(this->changeNotifyMsg);
    }

    for (vector<Icon*>::const_iterator iter = icons.begin(); iter != icons.end(); iter++) {
        delete *iter;
    }
    icons.clear();

    SAFERELEASE(this->workingFolder);
    SAFERELEASE(this->rootFolder);
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
        this->changeNotifyMsg,
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
    // Don't add existing icons
    if (FindIcon(pidl) != this->icons.end()) return;

    D2D1_RECT_F pos;
    PositionIcon(pidl, &pos);
    Icon* icon = new Icon(this, pidl, this->workingFolder);
    icon->SetPosition((int)pos.left, (int)pos.top);
    icons.push_back(icon);
}


void IconGroup::RemoveIcon(PCITEMID_CHILD pidl) {
    vector<Icon*>::iterator icon = FindIcon(pidl);
    if (icon != icons.end()) {
        (*icon)->Hide();
        delete *icon;
        icons.erase(icon);
        this->window->Repaint();
    }
}


void IconGroup::PositionIcon(PCITEMID_CHILD pidl, D2D1_RECT_F* position) {
    static float pos = 5;
    position->bottom = 64;
    position->left = 0 + pos;
    position->right = 64 + pos;
    position->top = 5;
    pos += 80;
}


vector<Icon*>::iterator IconGroup::FindIcon(PCITEMID_CHILD pidl) {
    vector<Icon*>::iterator icon;
    for (icon = this->icons.begin(); icon != this->icons.end(); ++icon) {
        if ((*icon)->CompareID(pidl) == 0) {
            return icon;
        }
    }
    return icon;
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
LRESULT WINAPI IconGroup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == this->changeNotifyMsg) {
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

            case SHCNE_MKDIR:
            case SHCNE_CREATE:
                {
                    AddIcon(PIDL::GetLastPIDLItem(idList[0]));
                }
                break;

            case SHCNE_RMDIR:
            case SHCNE_DELETE:
                {
                    RemoveIcon(PIDL::GetLastPIDLItem(idList[0]));
                }
                break;

            // A non-folder item has been renamed.
            case SHCNE_RENAMEITEM:
            case SHCNE_RENAMEFOLDER:
                {
                    TRACEW(L"Renamed: %s -> %s", file1, file2);
                }
                break;
            }

            SHChangeNotification_Unlock(notifyLock);
        }

        return 0;
    }
    else {
        this->eventHandler->HandleMessage(window, message, wParam, lParam); 
        return DefWindowProc(window, message, wParam, lParam);
    }
}
