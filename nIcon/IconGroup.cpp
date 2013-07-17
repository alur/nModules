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
#include <Shlwapi.h>
#include <algorithm>
#include <functional>
#include "IconGroup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/Debugging.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/PIDL.h"


/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup(LPCSTR prefix) : Drawable(prefix) {
    // Initalize all variables.
    this->mChangeNotifyUID = 0;

    LoadSettings();

    DrawableSettings defaults;
    defaults.width = 500;
    defaults.height = 300;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0x00000000;

    mNextPositionID = 0;

    this->window->Initialize(&defaults, &defaultState);
    this->window->AddDropRegion();
    this->window->Show();

    mChangeNotifyMsg = this->window->RegisterUserMessage(this);
    
    WCHAR path[MAX_PATH];
    this->settings->GetString("Folder", path, sizeof(path), "Desktop");
    SetFolder(path);
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (mChangeNotifyUID != 0) {
        SHChangeNotifyDeregister(mChangeNotifyUID);
    }

    if (mChangeNotifyMsg) {
        this->window->ReleaseUserMessage(mChangeNotifyMsg);
    }

    for (auto icon : mIcons) {
        delete icon;
    }
    mIcons.clear();

    SAFERELEASE(mWorkingFolder);
    SAFERELEASE(mRootFolder);
}


/// <summary>
/// 
/// </summary>
void IconGroup::LoadSettings() {
    // Icon settings
    Settings *iconSettings = this->settings->CreateChild("Icon");
    int iconSize = iconSettings->GetInt("Size", 48);
    delete iconSettings;

    // Tile settings
    Settings *tileSettings = this->settings->CreateChild("Tile");
    mTileHeight = tileSettings->GetInt("Height", iconSize + 20);
    mTileWidth = tileSettings->GetInt("Width", iconSize + 20);
    delete tileSettings;

    //
    LayoutSettings layoutDefaults;
    layoutDefaults.mColumnSpacing = 10;
    layoutDefaults.mRowSpacing = 10;
    layoutDefaults.mPadding.left = 5;
    layoutDefaults.mPadding.top = 5;
    layoutDefaults.mPadding.right = 5;
    layoutDefaults.mPadding.bottom = 5;
    layoutDefaults.mStartPosition = LayoutSettings::StartPosition::TopLeft;
    layoutDefaults.mPrimaryDirection = LayoutSettings::Direction::Horizontal;
    mLayoutSettings.Load(this->settings, &layoutDefaults);

    if (!settings->GetBool("DontHideDesktopSystemIcons", false)) {
        AddNameFilter(L".controlPanel");
        AddNameFilter(L".libraries");
        AddNameFilter(L".network");
        AddNameFilter(L".homegroup");
        AddNameFilter(L".user");
    }

    //
    char buffer[64];
    StringCchPrintfA(buffer, 64, "*%sHide", settings->prefix);
    LiteStep::IterateOverLinesW(buffer, [this] (LPCWSTR line) -> void {
        LiteStep::IterateOverTokens(line, std::bind(&IconGroup::AddNameFilter, this, std::placeholders::_1));
    });
}


void IconGroup::AddNameFilter(LPCWSTR name) {
    if (_wcsicmp(name, L".computer") == 0) {
        mHiddenItems.push_back(L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
    }
    else if (_wcsicmp(name, L".recycleBin") == 0) {
        mHiddenItems.push_back(L"::{645FF040-5081-101B-9F08-00AA002F954E}");
    }
    else if (_wcsicmp(name, L".controlPanel") == 0) {
        mHiddenItems.push_back(L"::{26EE0668-A00A-44D7-9371-BEB064C98683}");
        mHiddenItems.push_back(L"::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}");
    }
    else if (_wcsicmp(name, L".libraries") == 0) {
        mHiddenItems.push_back(L"::{031E4825-7B94-4DC3-B131-E946B44C8DD5}");
    }
    else if (_wcsicmp(name, L".network") == 0) {
        mHiddenItems.push_back(L"::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}");
    }
    else if (_wcsicmp(name, L".homegroup") == 0) {
        mHiddenItems.push_back(L"::{B4FB3F98-C1EA-428D-A78A-D1F5659CBA93}");
    }
    else if (_wcsicmp(name, L".user") == 0) {
        LPWSTR path;
        SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &path);
        mHiddenItems.push_back(path);
        CoTaskMemFree(path);
    }
    else {
        mHiddenItems.push_back(name);
    }
}


/// <summary>
/// 
/// </summary>
void IconGroup::SetFolder(LPWSTR folder) {
    PIDLIST_RELATIVE idList, idNext;
    IEnumIDList* enumIDList;

    // Just in case we are switching folders, deregister for old notifications
    if (mChangeNotifyUID != 0) {
        SHChangeNotifyDeregister(mChangeNotifyUID);
    }

    // Get the root ISHellFolder
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&mRootFolder));

    // Get the folder we are interested in
    if (_wcsicmp(folder, L"desktop") == 0) {
        SHGetKnownFolderIDList(FOLDERID_Desktop, 0, NULL, &idList);
        SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&mWorkingFolder));
    }
    else {
        mRootFolder->ParseDisplayName(NULL, NULL, folder, NULL, &idList, NULL);
        mRootFolder->BindToObject(idList, NULL, IID_IShellFolder, reinterpret_cast<LPVOID*>(&mWorkingFolder));
    }

    // Enumerate the contents of this folder
    mWorkingFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE) {
        AddIcon(idNext, true);
    }
    enumIDList->Release();

    this->window->Repaint();

    // Register for change notifications
    SHChangeNotifyEntry watchEntries[] = { idList, FALSE };
    mChangeNotifyUID = SHChangeNotifyRegister(
        this->window->GetWindowHandle(),
        SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery,
        SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR | SHCNE_RMDIR | SHCNE_RENAMEITEM
        | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM | SHCNE_UPDATEDIR | SHCNE_UPDATEIMAGE | SHCNE_ASSOCCHANGED,
        mChangeNotifyMsg,
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
    if (FindIcon(pidl) != nullptr) return;

    // Check if the icon should be supressed
    WCHAR buffer[MAX_PATH], buffer2[MAX_PATH];
    GetDisplayNameOf(pidl, SHGDN_FORPARSING, buffer, _countof(buffer));
    GetDisplayNameOf(pidl, SHGDN_NORMAL, buffer2, _countof(buffer2));
    for (auto hidden : mHiddenItems) {
        if (_wcsicmp(hidden.c_str(), buffer) == 0) return;
    }

    D2D1_RECT_F pos;
    PositionIcon(pidl, &pos);

    IconTile* icon = new IconTile(this, pidl, mWorkingFolder, mTileWidth, mTileHeight);
    icon->SetPosition((int)pos.left, (int)pos.top, noRedraw);
    mIcons.push_back(icon);
}


void IconGroup::RemoveIcon(PCITEMID_CHILD pidl) {
    mIcons.remove_if([pidl] (IconTile *icon) -> bool {
        if (icon->CompareID(pidl) == 0) {
            delete icon;
            return true;
        }
        return false;
    });
    this->window->Repaint();
}


void IconGroup::UpdateIcon(PCITEMID_CHILD pidl) {
    auto icon = FindIcon(pidl);
    if (icon != nullptr) {
        icon->UpdateIcon();
    }
}


void IconGroup::RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID) {
    auto icon = FindIcon(oldID);
    if (icon != nullptr) {
        icon->Rename(newID);
    }
}


void IconGroup::PositionIcon(PCITEMID_CHILD /* pidl */, D2D1_RECT_F *position) {
    RECT r = mLayoutSettings.RectFromID(mNextPositionID++, mTileWidth, mTileHeight, this->window->GetDrawingSettings()->width, this->window->GetDrawingSettings()->height);
    *position = D2D1::RectF(FLOAT(r.left), FLOAT(r.top), FLOAT(r.right), FLOAT(r.bottom));
}


IconTile* IconGroup::FindIcon(PCITEMID_CHILD pidl) {
    for (auto icon : mIcons) {
        if (icon->CompareID(pidl) == 0) {
            return icon;
        }
    }
    return nullptr;
}


void IconGroup::UpdateAllIcons() {
    for (auto icon : mIcons) {
        icon->UpdateIcon(false);
    }
    this->window->Repaint();
}



/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT IconGroup::GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf) {
    STRRET ret;
    HRESULT hr;

    hr = mRootFolder->GetDisplayNameOf(pidl, flags, &ret);

    if (SUCCEEDED(hr)) {
        hr = StrRetToBufW(&ret, pidl, buf, cchBuf);
    }

    return hr;
}


/// <summary>
/// 
/// </summary>
LRESULT WINAPI IconGroup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    if (message == mChangeNotifyMsg) {
        long event;
        PIDLIST_ABSOLUTE* idList;
        WCHAR file1[MAX_PATH] = L"", file2[MAX_PATH] = L"";
        HANDLE notifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &idList, &event);

        if (notifyLock) {
            if (idList[0]) {
                GetDisplayNameOf(idList[0], SHGDN_FORPARSING, file1, sizeof(file1)/sizeof(WCHAR));
                if (idList[1]) {
                    GetDisplayNameOf(idList[1], SHGDN_FORPARSING, file2, sizeof(file2)/sizeof(WCHAR));
                }
            }
            TRACEW(L"ChangeNotifyEvent[%d]: %s %s %l", event, file1, file2);

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
