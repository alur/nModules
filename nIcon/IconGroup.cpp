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


struct DoubleNullStrCollection {
    DoubleNullStrCollection() {
        cchSize = 2;
        str = decltype(str)(calloc(cchSize, sizeof(str[0])));
    }

    ~DoubleNullStrCollection() {
        free(str);
    }

    size_t GetSTRSize() {
        return sizeof(str[0])*cchSize;
    }

    void AddStr(LPCWSTR str) {
        size_t cchStrLen = wcslen(str) + 1;
        this->str = LPWSTR(realloc(this->str, sizeof(WCHAR)*(cchSize + cchStrLen)));
        memcpy(&this->str[cchSize-2], str, (cchStrLen)*sizeof(WCHAR));
        cchSize += cchStrLen;
        this->str[cchSize-2] = L'\0';
        this->str[cchSize-1] = L'\0';
    }

    size_t cchSize;
    LPWSTR str;
};


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


/// <summary>
/// 
/// </summary>
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
    
    int iconPosition = GetIconPosition(pidl);
    RECT pos = mLayoutSettings.RectFromID(iconPosition, mTileWidth, mTileHeight, this->window->GetDrawingSettings()->width, this->window->GetDrawingSettings()->height);

    IconTile *icon = new IconTile(this, pidl, mWorkingFolder, mTileWidth, mTileHeight);
    icon->SetPosition(iconPosition, (int)pos.left, (int)pos.top, noRedraw);
    mIcons.push_back(icon);
}


/// <summary>
/// 
/// </summary>
void IconGroup::RemoveIcon(PCITEMID_CHILD pidl) {
    mIcons.remove_if([pidl, this] (IconTile *icon) -> bool {
        if (icon->CompareID(pidl) == 0) {
            mEmptySpots.insert(icon->GetPositionID());
            delete icon;
            return true;
        }
        return false;
    });
    this->window->Repaint();
}


/// <summary>
/// 
/// </summary>
void IconGroup::UpdateIcon(PCITEMID_CHILD pidl) {
    auto icon = FindIcon(pidl);
    if (icon != nullptr) {
        icon->UpdateIcon();
    }
}


/// <summary>
/// 
/// </summary>
void IconGroup::RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID) {
    auto icon = FindIcon(oldID);
    if (icon != nullptr) {
        icon->Rename(newID);
    }
}


/// <summary>
/// 
/// </summary>
int IconGroup::GetIconPosition(PCITEMID_CHILD /* pidl */) {
    if (!mEmptySpots.empty()) {
        int id = *mEmptySpots.begin();
        mEmptySpots.erase(mEmptySpots.begin());
        return id;
    }
    return mNextPositionID++;
}


/// <summary>
/// 
/// </summary>
IconTile* IconGroup::FindIcon(PCITEMID_CHILD pidl) {
    for (auto icon : mIcons) {
        if (icon->CompareID(pidl) == 0) {
            return icon;
        }
    }
    return nullptr;
}


/// <summary>
/// Updates all icons.
/// </summary>
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
/// Retrives the parsing path for the current folder.
/// </summary>
HRESULT IconGroup::GetFolderPath(LPWSTR buf, UINT cchBuf) {
    IPersistFolder2 *ipsf2;
    LPITEMIDLIST curFolder;
    STRRET curFolderName;
    HRESULT hr;

    if (SUCCEEDED(hr = mWorkingFolder->QueryInterface(IID_IPersistFolder2, (LPVOID*) &ipsf2))) {
        hr = ipsf2->GetCurFolder(&curFolder);

        if (SUCCEEDED(hr)) {
            hr = mRootFolder->GetDisplayNameOf(curFolder, SHGDN_FORPARSING, &curFolderName);
        }
        
        if (SUCCEEDED(hr)) {
            hr = StrRetToBufW(&curFolderName, curFolder, buf, cchBuf);
        }

        ipsf2->Release();
    }

    return hr;
}


/// <summary>
/// Deselects all items.
/// </summary>
void IconGroup::DeselectAll() {
    for (IconTile *tile : mIcons) {
        tile->Deselect(false);
    }
    this->window->Repaint();
}


/// <summary>
/// Deselects all items.
/// </summary>
void IconGroup::SelectAll() {
    for (IconTile *tile : mIcons) {
        tile->Select(false);
    }
    this->window->Repaint();
}


/// <summary>
/// Brings up the context menu for the currently selected icons.
/// </summary>
void IconGroup::ContextMenu() {
    IContextMenu *contextMenu;
    HMENU menu;
    std::vector<LPCITEMIDLIST> items;

    for (IconTile *tile : mIcons) {
        if (tile->IsSelected()) {
            items.push_back(tile->GetItem());
        }
    }

    if (items.empty()) {
        return;
    }

    mWorkingFolder->GetUIObjectOf(nullptr, UINT(items.size()), &items[0], IID_IContextMenu, nullptr, reinterpret_cast<LPVOID*>(&contextMenu));

    menu = CreatePopupMenu();
    contextMenu->QueryContextMenu(menu, 0, 0, 0, CMF_NORMAL);

    POINT pt;
    GetCursorPos(&pt);
    HRESULT hr;
    int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->window->GetWindowHandle(), nullptr);
    if (command != 0) {
        CHAR verb[MAX_LINE_LENGTH];
        contextMenu->GetCommandString(command, GCS_VERBA, nullptr, LPSTR(verb), _countof(verb));

        // TODO::Figure out why InvokeCommand fails if i go for unicode.
        CMINVOKECOMMANDINFOEX info;
        ZeroMemory(&info, sizeof(info));
        info.cbSize = sizeof(info);
        //info.fMask = CMIC_MASK_UNICODE;
        info.hwnd = this->window->GetWindowHandle();
        info.lpVerb = verb;
        hr = contextMenu->InvokeCommand(LPCMINVOKECOMMANDINFO(&info));
    }

    DestroyMenu(menu);
    contextMenu->Release();
}


/// <summary>
/// Attempts to paste the contents of the clipboard to the desktop
/// </summary>
void IconGroup::DoPaste() {
    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (OpenClipboard(this->window->GetWindowHandle())) {
            LPDROPFILES data = LPDROPFILES(GetClipboardData(CF_HDROP));
            bool move = false;
            WCHAR target[MAX_PATH];

            // Check if the file should be moved rather than copied.
            UINT cfDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
            if (IsClipboardFormatAvailable(cfDropEffect)) {
                move = (*(DWORD*)GetClipboardData(cfDropEffect) & DROPEFFECT_MOVE) == DROPEFFECT_MOVE;
            }

            GetFolderPath(target, _countof(target));

            // TODO::Handle data->fWide == 0
            SHFILEOPSTRUCTW shFileOp;
            ZeroMemory(&shFileOp, sizeof(shFileOp));
            shFileOp.wFunc = move ? FO_MOVE : FO_COPY;
            shFileOp.hwnd = this->window->GetWindowHandle();
            shFileOp.pFrom = LPCWSTR((BYTE*)data + data->pFiles);
            shFileOp.pTo = target;
            shFileOp.fFlags = FOF_NOCONFIRMMKDIR;

            SHFileOperationW(&shFileOp);
            if (move && !shFileOp.fAnyOperationsAborted) {
                EmptyClipboard();
            }

            CloseClipboard();
        }
    }
    else if (IsClipboardFormatAvailable(CF_TEXT)) {
    }
    else if (IsClipboardFormatAvailable(CF_BITMAP)) {
    }
    else if (IsClipboardFormatAvailable(CF_WAVE)) {
    }
}


/// <summary>
/// Attempts to paste the contents of the clipboard to the desktop.
/// </summary>
void IconGroup::DoCopy(bool cut) {
    // Generate a double-null terminated list of files to copy.
    DoubleNullStrCollection files;
    WCHAR buffer[MAX_PATH];
    for (IconTile *tile : mIcons) {
        if (tile->IsSelected()) {
            tile->GetDisplayName(SHGDN_FORPARSING, buffer, _countof(buffer));
            files.AddStr(buffer);
        }
    }

    // Put the list in global memory.
    DROPFILES dropObj = { sizeof(DROPFILES), { 0, 0 }, 0, 1 };
    HGLOBAL clipData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(dropObj) + files.GetSTRSize());
    LPBYTE data = (LPBYTE)GlobalLock(clipData);
    memcpy(data, &dropObj, sizeof(dropObj));
    memcpy(data + sizeof(dropObj), files.str, files.GetSTRSize());
    GlobalUnlock(clipData);

    // Specify whether the files are cut or copied.
    HGLOBAL dropEffect = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
    PDWORD dropData = (PDWORD)GlobalLock(dropEffect);
    *dropData = cut ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
    GlobalUnlock(dropEffect);

    // Put it all in the clipboard.
    if (OpenClipboard(this->window->GetWindowHandle())) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, clipData);
        SetClipboardData(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT), dropEffect);
        CloseClipboard();
    }
}


/// <summary>
/// Deletes all selected files.
/// </summary>
void IconGroup::DeleteSelectedFiles() {
    // Generate a double-null terminated list of files to copy.
    DoubleNullStrCollection files;
    WCHAR buffer[MAX_PATH];
    for (IconTile *tile : mIcons) {
        if (tile->IsSelected()) {
            tile->GetDisplayName(SHGDN_FORPARSING, buffer, _countof(buffer));
            files.AddStr(buffer);
        }
    }

    // Delete the files.
    SHFILEOPSTRUCTW shFileOp;
    ZeroMemory(&shFileOp, sizeof(shFileOp));
    shFileOp.wFunc = FO_DELETE;
    shFileOp.hwnd = nullptr;
    shFileOp.pFrom = files.str;
    shFileOp.pTo = nullptr;
    shFileOp.fFlags = FOF_ALLOWUNDO;
    SHFileOperationW(&shFileOp);
}


/// <summary>
/// Opens the selected files.
/// </summary>
void IconGroup::OpenSelectedFiles() {
    for (IconTile *tile : mIcons) {
        if (tile->IsSelected()) {
            WCHAR command[MAX_LINE_LENGTH];
            tile->GetDisplayName(SHGDN_FORPARSING, command, MAX_LINE_LENGTH);
            ShellExecuteW(nullptr, nullptr, command, nullptr, nullptr, SW_SHOW);
        }
    }
}


/// <summary>
/// Handles window messages.
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
        switch (message)
        {
        case WM_KEYDOWN:
            {
                switch (wParam)
                {
                case 'A':
                    {
                        if (GetKeyState(VK_CONTROL) < 0) {
                            SelectAll();
                        }
                    }
                    break;

                case 'C':
                    {
                        if (GetKeyState(VK_CONTROL) < 0) {
                            DoCopy(false);
                        }
                    }
                    break;

                case 'V':
                    {
                        if (GetKeyState(VK_CONTROL) < 0) {
                            DoPaste();
                        }
                    }
                    break;

                case 'X':
                    {
                        if (GetKeyState(VK_CONTROL) < 0) {
                            DoCopy(true);
                        }
                    }
                    break;

                case VK_DELETE:
                    {
                        DeleteSelectedFiles();
                    }
                    break;

                case VK_RETURN:
                    {
                        OpenSelectedFiles();
                    }
                    break;

                case VK_F2:
                    {

                    }
                    break;
                }
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            {
                DeselectAll();
            }
            break;
        }

        this->eventHandler->HandleMessage(window, message, wParam, lParam); 
        return DefWindowProc(window, message, wParam, lParam);
    }
}
