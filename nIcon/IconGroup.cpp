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
    defaults->width = 500;
    defaults->height = 300;

    StateSettings* defaultState = new StateSettings();
    defaultState->color = 0x00000000;

    this->settings->GetString("Folder", path, sizeof(path), "Desktop");

    this->window->Initialize(defaults, defaultState);
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
        AddIcon(idNext, true);
    }
    enumIDList->Release();

    this->window->Repaint();

    // Register for change notifications
    SHChangeNotifyEntry watchEntries[] = { idList, TRUE };
    this->changeNotifyUID = SHChangeNotifyRegister(
        this->window->GetWindowHandle(),
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR | SHCNE_RMDIR | SHCNE_RENAMEITEM
        | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM | SHCNE_UPDATEDIR | SHCNE_UPDATEIMAGE | SHCNE_ASSOCCHANGED,
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
void IconGroup::AddIcon(PCITEMID_CHILD pidl, bool noRedraw) {
    // Don't add existing icons
    if (FindIcon(pidl) != this->icons.end()) return;

    D2D1_RECT_F pos;
    PositionIcon(pidl, &pos);
    Icon* icon = new Icon(this, pidl, this->workingFolder);
    icon->SetPosition((int)pos.left, (int)pos.top, noRedraw);
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


void IconGroup::UpdateIcon(PCITEMID_CHILD pidl) {
    vector<Icon*>::iterator icon = FindIcon(pidl);
    if (icon != icons.end()) {
        (*icon)->UpdateIcon();
    }
}


void IconGroup::RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID) {
    vector<Icon*>::iterator icon = FindIcon(oldID);
    if (icon != icons.end()) {
        (*icon)->Rename(newID);
    }
}


void IconGroup::PositionIcon(PCITEMID_CHILD /* pidl */, D2D1_RECT_F* position) {
    static float xPos = 5;
    static float yPos = 5;
    position->bottom = 64 + yPos;
    position->left = 0 + xPos;
    position->right = 64 + xPos;
    position->top = yPos;
    xPos += 80;
    if (xPos > this->window->GetDrawingSettings()->width - 80) {
        xPos = 5;
        yPos += 100;
    }
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


void IconGroup::UpdateAllIcons() {
    for (vector<Icon*>::iterator icon = this->icons.begin(); icon != this->icons.end(); ++icon) {
        (*icon)->UpdateIcon(false);
    }
    this->window->Repaint();
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
LRESULT WINAPI IconGroup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    if (message == this->changeNotifyMsg) {
        long event;
        PIDLIST_ABSOLUTE* idList;
        HANDLE notifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &idList, &event);

        if (notifyLock) {
            switch (event) {
            case SHCNE_ATTRIBUTES:
            case SHCNE_UPDATEITEM:
            case SHCNE_UPDATEDIR:
                {
                    UpdateIcon(PIDL::GetLastPIDLItem(idList[0]));
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

            case SHCNE_RENAMEITEM:
            case SHCNE_RENAMEFOLDER:
                {
                    RenameIcon(PIDL::GetLastPIDLItem(idList[0]), PIDL::GetLastPIDLItem(idList[1]));
                }
                break;

            case SHCNE_ASSOCCHANGED:
            case SHCNE_UPDATEIMAGE:
                {
                    UpdateAllIcons();
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
