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
#include "../nShared/LSModule.hpp"
#include <shellapi.h>


#define CHANGE_SOURCES SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery
#define CHANGE_EVENTS SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR \
        | SHCNE_RMDIR | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM \
        | SHCNE_UPDATEDIR | SHCNE_UPDATEIMAGE | SHCNE_ASSOCCHANGED


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
IconGroup::IconGroup(LPCTSTR prefix)
    : Drawable(prefix)
    , mChangeNotifyUID(0)
    , mContextMenu2(nullptr)
    , mContextMenu3(nullptr)
    , mClipBoardCutFiles(false)
    , mInRectangleSelection(false)
    , mNextPositionID(0)
{
    LoadSettings();

    WindowSettings defaults, windowSettings;
    defaults.width = 500;
    defaults.height = 300;
    defaults.registerWithCore = true;
    windowSettings.Load(mSettings, &defaults);

    StateRender<State>::InitData initData;
    initData[State::Base].defaults.backgroundBrush.color = Color::Create(0x00000000);
    mStateRender.Load(initData, mSettings);

    mSelectionRectagle.Init(mSettings);

    mWindow->Initialize(windowSettings, &mStateRender);
    //mWindow->AddDropRegion();
    mWindow->AddPostPainter(&mSelectionRectagle);
    // TODO::Add the selection rectangle as a brush owner
    mWindow->Show();

    mChangeNotifyMsg = mWindow->RegisterUserMessage(this);
    
    WCHAR path[MAX_PATH];
    mSettings->GetString(_T("Folder"), path, _countof(path), _T("Desktop"));
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
        mWindow->ReleaseUserMessage(mChangeNotifyMsg);
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
void IconGroup::LoadSettings()
{
    // Icon settings
    Settings *iconSettings = mSettings->CreateChild(_T("Icon"));
    int iconSize = iconSettings->GetInt(_T("Size"), 48);
    mTileSettings.Load(iconSettings);
    delete iconSettings;

    // Tile settings
    Settings *tileSettings = mSettings->CreateChild(_T("Tile"));
    mTileHeight = tileSettings->GetInt(_T("Height"), iconSize + 20);
    mTileWidth = tileSettings->GetInt(_T("Width"), iconSize + 20);
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
    mLayoutSettings.Load(mSettings, &layoutDefaults);

    if (!mSettings->GetBool(_T("DontHideDesktopSystemIcons"), false))
    {
        AddNameFilter(L".controlPanel");
        AddNameFilter(L".libraries");
        AddNameFilter(L".network");
        AddNameFilter(L".homegroup");
        AddNameFilter(L".user");
    }

    //
    TCHAR buffer[64];
    StringCchPrintf(buffer, 64, _T("*%sHide"), mSettings->GetPrefix());
    LiteStep::IterateOverLines(buffer, [this] (LPCWSTR line) -> void {
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
    StopWatch stopWatch;

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
    Window::UpdateLock lock(mWindow);

    mWorkingFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &enumIDList);
    while (enumIDList->Next(1, &idNext, NULL) != S_FALSE)
    {
        AddIcon(idNext);
    }
    enumIDList->Release();

    mWindow->Repaint();

    // Register for change notifications
    SHChangeNotifyEntry watchEntries[] = { idList, FALSE };
    mChangeNotifyUID = SHChangeNotifyRegister(
        mWindow->GetWindowHandle(),
        CHANGE_SOURCES,
        CHANGE_EVENTS,
        mChangeNotifyMsg,
        1,
        watchEntries);

    // Let go fo the PIDLists
    CoTaskMemFree(idList);
    CoTaskMemFree(idNext);

    TRACE("IconGroup::SetFolder took %.5fs", stopWatch.Clock());
}


/// <summary>
/// Add's the icon with the specified ID to the view
/// </summary>
void IconGroup::AddIcon(PCITEMID_CHILD pidl) {
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
    RECT pos = mLayoutSettings.RectFromID(iconPosition, mTileWidth, mTileHeight, mWindow->GetDrawingSettings()->width, mWindow->GetDrawingSettings()->height);

    IconTile *icon = new IconTile(this, pidl, mWorkingFolder, mTileWidth, mTileHeight, mTileSettings);
    icon->SetPosition(iconPosition, (int)pos.left, (int)pos.top);
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
    mWindow->Repaint();
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
    mWindow->Repaint();
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
    mWindow->Repaint();
}


/// <summary>
/// Deselects all items.
/// </summary>
void IconGroup::SelectAll() {
    for (IconTile *tile : mIcons) {
        tile->Select(false);
    }
    mWindow->Repaint();
}


/// <summary>
/// Brings up the context menu for the currently selected icons.
/// </summary>
void IconGroup::ContextMenu()
{
    IContextMenu *contextMenu;
    HMENU menu;
    std::vector<LPCITEMIDLIST> items;
                HRESULT hr;

    for (IconTile *tile : mIcons)
    {
        if (tile->IsSelected())
        {
            items.push_back(tile->GetItem());
        }
    }

    if (items.empty())
    {
        return;
    }

    if (SUCCEEDED(hr = mWorkingFolder->GetUIObjectOf(nullptr, UINT(items.size()), &items[0], IID_IContextMenu, nullptr, reinterpret_cast<LPVOID*>(&contextMenu))))
    {
        menu = CreatePopupMenu();
        if (menu)
        {
            if (SUCCEEDED(hr = contextMenu->QueryContextMenu(menu, 0, 1, 1024, CMF_NORMAL | CMF_CANRENAME)))
            {
                POINT pt;
                GetCursorPos(&pt);
                contextMenu->QueryInterface(IID_IContextMenu2, (LPVOID*)&mContextMenu2);
                contextMenu->QueryInterface(IID_IContextMenu3, (LPVOID*)&mContextMenu3);
                int command = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, mWindow->GetWindowHandle(), nullptr);
                SAFERELEASE(mContextMenu2);
                SAFERELEASE(mContextMenu3);
                if (command != 0)
                {
                    //WCHAR verb[MAX_LINE_LENGTH];
                    //if (SUCCEEDED(hr = contextMenu->GetCommandString(command, GCS_VERBW, nullptr, LPSTR(verb), _countof(verb))))
                    {
                        // TODO::Figure out why InvokeCommand fails if i go for unicode.
                        CMINVOKECOMMANDINFOEX info;
                        ZeroMemory(&info, sizeof(info));
                        info.cbSize = sizeof(info);
                        info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
                        if (GetKeyState(VK_CONTROL) < 0)
                        {
                            info.fMask |= CMIC_MASK_CONTROL_DOWN;
                        }
                        if (GetKeyState(VK_SHIFT) < 0)
                        {
                            info.fMask |= CMIC_MASK_SHIFT_DOWN;
                        }

                        info.hwnd = mWindow->GetWindowHandle();
                        //info.lpVerb = verb;
                        //info.lpVerbW = verb;
                        info.lpVerb = MAKEINTRESOURCEA(command - 1);
                        info.lpVerbW = MAKEINTRESOURCEW(command - 1);
                        info.ptInvoke = pt;
                        hr = contextMenu->InvokeCommand(LPCMINVOKECOMMANDINFO(&info));
                    }
                }
            }

            DestroyMenu(menu);
        }

        contextMenu->Release();
    }
}


/// <summary>
/// Attempts to undo the last file operation.
/// </summary>
void IconGroup::DoUndo() {
}


/// <summary>
/// Attempts to paste the contents of the clipboard to the desktop
/// </summary>
void IconGroup::DoPaste() {
    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (OpenClipboard(mWindow->GetWindowHandle())) {
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
            shFileOp.hwnd = mWindow->GetWindowHandle();
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
/// <param name="cut">True to cut, false to copy.</param>
void IconGroup::DoCopy(bool cut) {
    // Generate a double-null terminated list of files to copy.
    DoubleNullStrCollection files;
    WCHAR buffer[MAX_PATH];
    
    ClearAllGhosting(!cut);

    for (IconTile *tile : mIcons) {
        if (tile->IsSelected()) {
            tile->GetDisplayName(SHGDN_FORPARSING, buffer, _countof(buffer));
            if (cut) {
                tile->SetGhost();
            }
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
    if (OpenClipboard(mWindow->GetWindowHandle())) {
        EmptyClipboard();
        SetClipboardData(CF_HDROP, clipData);
        SetClipboardData(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT), dropEffect);
        CloseClipboard();
    }

    //
    if (cut) {
        mClipBoardCutFiles = true;
        mWindow->Repaint();
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
    shFileOp.fFlags = FOF_ALLOWUNDO | FOF_WANTNUKEWARNING;
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
/// Renames the selected files.
/// </summary>
void IconGroup::RenameSelectedFiles() {
}


/// <summary>
/// 
/// </summary>
void IconGroup::ClearAllGhosting(bool repaint) {
    if (mClipBoardCutFiles) {
        mClipBoardCutFiles = false;
        for (IconTile *tile : mIcons) {
            if (tile->IsGhosted()) {
                tile->ClearGhost();
            }
        }
        if (repaint) {
            mWindow->Repaint();
        }
    }
}


/// <summary>
/// 
/// </summary>
void IconGroup::StartRectangleSelection(D2D1_POINT_2U point) {
    mRectangleStart = point;
    mInRectangleSelection = true;
    mWindow->DisableMouseForwarding();
    mWindow->SetMouseCapture();
}


/// <summary>
/// 
/// </summary>
void IconGroup::EndRectangleSelection(D2D1_POINT_2U point) {
    mInRectangleSelection = false;
    mSelectionRectagle.Hide();
    mWindow->ReleaseMouseCapture();
    mWindow->Repaint(&mSelectionRectagle.GetRect());

    D2D1_RECT_F rect = D2D1::RectF(
        float(min(mRectangleStart.x, point.x)),
        float(min(mRectangleStart.y, point.y)),
        float(max(mRectangleStart.x, point.x)),
        float(max(mRectangleStart.y, point.y))
    );

    for (IconTile *tile : mIcons) {
        if (tile->IsInRect(rect)) {
            tile->Select();
        }
        else {
            tile->Deselect();
        }
    }

    mWindow->EnableMouseForwarding();
    mWindow->Repaint(&rect);
}


/// <summary>
/// 
/// </summary>
void IconGroup::MoveRectangleSelection(D2D1_POINT_2U point) {
    D2D1_RECT_F rect = D2D1::RectF(
        float(min(mRectangleStart.x, point.x)),
        float(min(mRectangleStart.y, point.y)),
        float(max(mRectangleStart.x, point.x)),
        float(max(mRectangleStart.y, point.y))
    );

    mWindow->Repaint(&mSelectionRectagle.GetRect());

    mSelectionRectagle.SetRect(rect);

    for (IconTile *tile : mIcons) {
        if (tile->IsInRect(rect)) {
            tile->Select();
        }
        else {
            tile->Deselect();
        }
    }

    mSelectionRectagle.Show();
    mWindow->Repaint(&rect);
}


/// <summary>
/// Handles changes to the clipboard.
/// </summary>
void IconGroup::HandleClipboardChange() {
    ClearAllGhosting(true);
    /*
    if (IsClipboardFormatAvailable(CF_HDROP)) {
        if (OpenClipboard(mWindow->GetWindowHandle())) {
            CloseClipboard();
        }
        else if (mClipBoardCutFiles) {

        }
    }
    else if (mClipBoardCutFiles) {
        mClipBoardCutFiles = false;
        for (auto tile : mIcons) {
            if (tile->IsGhosted()) {
                tile->ClearGhost();
            }
        }
    }
    */
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
                    UpdateIcon(ILFindLastID(idList[0]));
                }
                break;

            case SHCNE_MKDIR:
            case SHCNE_CREATE:
                {
                    AddIcon(ILFindLastID(idList[0]));
                }
                break;

            case SHCNE_RMDIR:
            case SHCNE_DELETE:
                {
                    RemoveIcon(ILFindLastID(idList[0]));
                }
                break;

            case SHCNE_RENAMEITEM:
            case SHCNE_RENAMEFOLDER:
                {
                    RenameIcon(ILFindLastID(idList[0]), ILFindLastID(idList[1]));
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
        if (mContextMenu3)
        {
            LRESULT result;
            if (SUCCEEDED(mContextMenu3->HandleMenuMsg2(message, wParam, lParam, &result)))
            {
                return result;
            }
        }
        
        if (mContextMenu2)
        {
            if (SUCCEEDED(mContextMenu2->HandleMenuMsg(message, wParam, lParam)))
            {
                return 0;
            }
        }

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

                case 'Z':
                    {
                        if (GetKeyState(VK_CONTROL) < 0) {
                            DoUndo();
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
                        RenameSelectedFiles();
                    }
                    break;
                    
                case VK_F5:
                    {
                        UpdateAllIcons();
                    }
                    break;
                    
                case VK_F6:
                    {
                        for (IconTile *tile : mIcons) {
                            if (tile->IsSelected()) {
                                tile->SetGhost();
                            }
                        }
                    }
                    break;

                case VK_UP:
                case VK_DOWN:
                case VK_LEFT:
                case VK_RIGHT:
                    {

                    }
                    break;
                }
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            {
                if (!mInRectangleSelection) {
                    StartRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                }
                else {
                    EndRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                }
            }
            break;

        case WM_MOUSEMOVE:
            {
                if (mInRectangleSelection) {
                    MoveRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                }
            }
            break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
            {
                if (mInRectangleSelection) {
                    EndRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                }
                else if (GetKeyState(VK_CONTROL) >= 0) {
                    DeselectAll();
                }
            }
            break;

        case Window::WM_TOPPARENTLOST:
            {
                mChangeNotifyMsg = 0;
                if (mChangeNotifyUID != 0) {
                    SHChangeNotifyDeregister(mChangeNotifyUID);
                    mChangeNotifyUID = 0;
                }
            }
            break;

        case Window::WM_NEWTOPPARENT:
            {
                IPersistFolder2 *ipsf2;
                LPITEMIDLIST curFolder;
                HRESULT hr;

                mChangeNotifyMsg = mWindow->RegisterUserMessage(this);

                if (SUCCEEDED(hr = mWorkingFolder->QueryInterface(IID_IPersistFolder2, (LPVOID*) &ipsf2))) {
                    if (SUCCEEDED(hr = ipsf2->GetCurFolder(&curFolder))) {
                        // (Re)Register for change notifications
                        SHChangeNotifyEntry watchEntries[] = { curFolder, FALSE };
                        mChangeNotifyUID = SHChangeNotifyRegister(
                            window,
                            CHANGE_SOURCES,
                            CHANGE_EVENTS,
                            mChangeNotifyMsg,
                            1,
                            watchEntries);

                        // Let go fo the PIDLists
                        CoTaskMemFree(curFolder);
                    }
                    ipsf2->Release();
                }
            }
            break;
        }

        mEventHandler->HandleMessage(window, message, wParam, lParam); 
        return DefWindowProc(window, message, wParam, lParam);
    }
}
