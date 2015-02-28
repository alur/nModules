//-------------------------------------------------------------------------------------------------
// /nIcon/TileGroup.cpp
// The nModules Project
//
// A group of icons tiles. Represents the window which contains all icons.
//-------------------------------------------------------------------------------------------------
#include "TileGroup.hpp"

#include "../nCoreCom/Core.h"

#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"

#include "../Utilities/DoubleNullStringList.hpp"

#include <algorithm>
#include <functional>
#include <shellapi.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <windowsx.h>

#define CHANGE_SOURCES SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery
#define CHANGE_EVENTS SHCNE_CREATE | SHCNE_DELETE | SHCNE_ATTRIBUTES | SHCNE_MKDIR \
        | SHCNE_RMDIR | SHCNE_RENAMEITEM | SHCNE_RENAMEFOLDER | SHCNE_UPDATEITEM \
        | SHCNE_UPDATEDIR | SHCNE_UPDATEIMAGE | SHCNE_ASSOCCHANGED

static const WindowSettings sWindowDefaults([] (WindowSettings &defaults) {
  defaults.width = 500;
  defaults.height = 300;
  defaults.registerWithCore = true;
});

static const LayoutSettings sLayoutDefaults([] (LayoutSettings &defaults) {
  defaults.mColumnSpacing = 10;
  defaults.mRowSpacing = 10;
  defaults.mPadding.left = 5;
  defaults.mPadding.top = 5;
  defaults.mPadding.right = 5;
  defaults.mPadding.bottom = 5;
  defaults.mStartPosition = LayoutSettings::StartPosition::TopLeft;
  defaults.mPrimaryDirection = LayoutSettings::Direction::Horizontal;
});

static const StateRender<TileGroup::State>::InitData sInitData([](StateRender<TileGroup::State>::InitData &initData) {
  initData[TileGroup::State::Base].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0x00000000);
});


/// <summary>
/// Constructor
/// </summary>
TileGroup::TileGroup(LPCTSTR prefix)
  : Drawable(prefix)
  , mChangeNotifyUID(0)
  , mContextMenu2(nullptr)
  , mContextMenu3(nullptr)
  , mClipBoardCutFiles(false)
  , mInRectangleSelection(false)
  , mNextPositionID(0)
  , mRootFolder(nullptr)
{
  LoadSettings();

  WindowSettings windowSettings;
  windowSettings.Load(mSettings, &sWindowDefaults);
  mStateRender.Load(sInitData, mSettings);

  mSelectionRectagle.Init(mSettings);

  mWindow->Initialize(windowSettings, &mStateRender);
  //mWindow->AddDropRegion();
  mWindow->AddPostPainter(&mSelectionRectagle);
  // TODO::Add the selection rectangle as a brush owner
  mWindow->Show();

  mIconLoadedMessage = mWindow->RegisterUserMessage(this);
  mChangeNotifyMsg = mWindow->RegisterUserMessage(this);

  // Get the root ISHellFolder
  SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&mRootFolder));

  WCHAR path[MAX_PATH];
  mSettings->GetString(L"Folder", path, _countof(path), L"Desktop");
  SetFolder(path);
}


/// <summary>
/// Destructor
/// </summary>
TileGroup::~TileGroup() {
  if (mChangeNotifyUID != 0) {
    SHChangeNotifyDeregister(mChangeNotifyUID);
  }

  if (mChangeNotifyMsg) {
    mWindow->ReleaseUserMessage(mChangeNotifyMsg);
  }

  if (mIconLoadedMessage) {
    mWindow->ReleaseUserMessage(mIconLoadedMessage);
  }

  for (auto tile : mTiles) {
    delete tile;
  }

  SAFERELEASE(mWorkingFolder);
  SAFERELEASE(mRootFolder);
}


/// <summary>
///
/// </summary>
void TileGroup::LoadSettings() {
  // Icon settings
  Settings *iconSettings = mSettings->CreateChild(L"Icon");
  int iconSize = iconSettings->GetInt(L"Size", 48);
  mTileSettings.Load(iconSettings);
  delete iconSettings;

  // Tile settings
  Settings *tileSettings = mSettings->CreateChild(L"Tile");
  mTileHeight = tileSettings->GetInt(L"Height", iconSize + 20);
  mTileWidth = tileSettings->GetInt(L"Width", iconSize + 20);
  delete tileSettings;

  mLayoutSettings.Load(mSettings, &sLayoutDefaults);

  if (!mSettings->GetBool(L"DontHideDesktopSystemIcons", false)) {
    AddNameFilter(L".controlPanel");
    AddNameFilter(L".libraries");
    AddNameFilter(L".network");
    AddNameFilter(L".homegroup");
    AddNameFilter(L".user");
    AddNameFilter(L".onedrive");
  }

  mSettings->IterateOverCommandTokens(
    L"Hide", std::bind(&TileGroup::AddNameFilter, this, std::placeholders::_1));
}


/// <summary>
///
/// </summary>
void TileGroup::AddNameFilter(LPCWSTR name) {
  if (_wcsicmp(name, L".computer") == 0) {
    mHiddenItems.emplace(L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}");
  } else if (_wcsicmp(name, L".recycleBin") == 0) {
    mHiddenItems.emplace(L"::{645FF040-5081-101B-9F08-00AA002F954E}");
  } else if (_wcsicmp(name, L".controlPanel") == 0) {
    mHiddenItems.emplace(L"::{26EE0668-A00A-44D7-9371-BEB064C98683}");
    mHiddenItems.emplace(L"::{5399E694-6CE5-4D6C-8FCE-1D8870FDCBA0}");
  } else if (_wcsicmp(name, L".libraries") == 0) {
    mHiddenItems.emplace(L"::{031E4825-7B94-4DC3-B131-E946B44C8DD5}");
  } else if (_wcsicmp(name, L".network") == 0) {
    mHiddenItems.emplace(L"::{F02C1A0D-BE21-4350-88B0-7367FC96EF3C}");
  } else if (_wcsicmp(name, L".homegroup") == 0) {
    mHiddenItems.emplace(L"::{B4FB3F98-C1EA-428D-A78A-D1F5659CBA93}");
  } else if (_wcsicmp(name, L".onedrive") == 0) {
    LPWSTR path;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SkyDrive, 0, nullptr, &path))) {
      mHiddenItems.emplace(path);
      CoTaskMemFree(path);
    }
  } else if (_wcsicmp(name, L".user") == 0) {
    LPWSTR path;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &path))) {
      mHiddenItems.emplace(path);
      CoTaskMemFree(path);
    }
  } else {
    mHiddenItems.emplace(name);
  }
}


/// <summary>
///
/// </summary>
void TileGroup::SetFolder(LPWSTR folder) {
  PIDLIST_ABSOLUTE idList;

  // Just in case we are switching folders, deregister for old notifications
  if (mChangeNotifyUID != 0) {
    SHChangeNotifyDeregister(mChangeNotifyUID);
    mChangeNotifyUID = 0;
  }

  // Get the folder we are interested in
  if (_wcsicmp(folder, L"desktop") == 0) {
    SHGetKnownFolderIDList(FOLDERID_Desktop, 0, nullptr, &idList);
    SHGetDesktopFolder(reinterpret_cast<IShellFolder**>(&mWorkingFolder));
  } else {
    mRootFolder->ParseDisplayName(nullptr, nullptr, folder, nullptr, &idList, nullptr);
    mRootFolder->BindToObject(idList, nullptr, IID_IShellFolder, reinterpret_cast<LPVOID*>(&mWorkingFolder));
  }

  // Enumerate the contents of this folder
  LoadFolderRequest request;
  request.blackList = mHiddenItems;
  request.folder = mWorkingFolder;
  request.targetIconWidth = mTileSettings.mIconSize;
  nCore::LoadFolder(request, this);

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
}


LPARAM TileGroup::FolderLoaded(UINT64 id, LoadFolderResponse *response) {
  Window::UpdateLock lock(mWindow);
  for (auto item : response->items) {
    ItemLoaded(id, &item);
  }
  return 0;
}


LPARAM TileGroup::ItemLoaded(UINT64 id, LoadItemResponse *item) {
  int iconPosition = GetIconPosition(item->id);
  RECT pos = mLayoutSettings.RectFromID(iconPosition, mTileWidth, mTileHeight, int(mWindow->GetSize().width + 0.5f), int(mWindow->GetSize().height + 0.5f));
  Tile *icon = new Tile(this, item->id, mWorkingFolder, mTileWidth, mTileHeight, mTileSettings, item->thumbnail);
  icon->SetPosition(iconPosition, (int)pos.left, (int)pos.top);
  mTiles.push_back(icon);

  return 0;
}


/// <summary>
/// Add's the icon with the specified ID to the view
/// </summary>
void TileGroup::AddIcon(PCITEMID_CHILD pidl) {
  // Don't add existing icons
  if (FindIcon(pidl) != nullptr) return;

  // Check if the icon should be supressed
  WCHAR buffer[MAX_PATH];
  GetDisplayNameOf(pidl, SHGDN_FORPARSING, buffer, _countof(buffer));
  if (mHiddenItems.find(buffer) != mHiddenItems.end()) return;

  LoadItemRequest request;
  request.folder = mWorkingFolder;
  request.targetIconWidth = mTileSettings.mIconSize;
  request.id = ILClone(pidl);
  nCore::LoadFolderItem(request, this);
}


/// <summary>
///
/// </summary>
void TileGroup::RemoveIcon(PCITEMID_CHILD pidl) {
  mTiles.remove_if([pidl, this] (Tile *tile) -> bool {
    if (tile->CompareID(pidl) == 0) {
      mEmptySpots.insert(tile->GetPositionID());
      delete tile;
      return true;
    }
    return false;
  });
  mWindow->Repaint();
}


/// <summary>
///
/// </summary>
void TileGroup::UpdateIcon(PCITEMID_CHILD pidl) {
  auto icon = FindIcon(pidl);
  if (icon != nullptr) {
    icon->UpdateIcon();
  }
}


/// <summary>
///
/// </summary>
void TileGroup::RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID) {
  auto icon = FindIcon(oldID);
  if (icon != nullptr) {
    icon->Rename(newID);
  }
}


/// <summary>
///
/// </summary>
int TileGroup::GetIconPosition(PCITEMID_CHILD /* pidl */) {
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
Tile *TileGroup::FindIcon(PCITEMID_CHILD pidl) const {
  for (auto tile : mTiles) {
    if (tile->CompareID(pidl) == 0) {
      return tile;
    }
  }
  return nullptr;
}


/// <summary>
/// Updates all icons.
/// </summary>
void TileGroup::UpdateAllIcons() {
  for (auto icon : mTiles) {
    icon->UpdateIcon(false);
  }
  mWindow->Repaint();
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT TileGroup::GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf) const {
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
HRESULT TileGroup::GetFolderPath(LPWSTR buf, UINT cchBuf) const {
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
void TileGroup::DeselectAll() {
  for (Tile *tile : mTiles) {
    tile->Deselect();
  }
  mWindow->Repaint();
}


/// <summary>
/// Deselects all items.
/// </summary>
void TileGroup::SelectAll() {
  for (Tile *tile : mTiles) {
    tile->Select();
  }
  mWindow->Repaint();
}


/// <summary>
/// Brings up the context menu for the currently selected icons.
/// </summary>
void TileGroup::ContextMenu() {
  IContextMenu *contextMenu;
  HMENU menu;
  std::vector<LPCITEMIDLIST> items;
  HRESULT hr;

  for (Tile *tile : mTiles) {
    if (tile->IsSelected()) {
      items.push_back(tile->GetItem());
    }
  }

  if (items.empty()) {
    return;
  }

  if (SUCCEEDED(hr = mWorkingFolder->GetUIObjectOf(nullptr, UINT(items.size()), &items[0], IID_IContextMenu, nullptr, reinterpret_cast<LPVOID*>(&contextMenu)))) {
    menu = CreatePopupMenu();
    if (menu) {
      if (SUCCEEDED(hr = contextMenu->QueryContextMenu(menu, 0, 1, 1024, CMF_NORMAL | CMF_CANRENAME))) {
        POINT pt;
        GetCursorPos(&pt);
        contextMenu->QueryInterface(IID_IContextMenu2, (LPVOID*)&mContextMenu2);
        contextMenu->QueryInterface(IID_IContextMenu3, (LPVOID*)&mContextMenu3);
        int command = TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, mWindow->GetWindowHandle(), nullptr);
        SAFERELEASE(mContextMenu2);
        SAFERELEASE(mContextMenu3);
        if (command != 0) {
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
void TileGroup::DoUndo() {
}


/// <summary>
/// Attempts to paste the contents of the clipboard to the desktop
/// </summary>
void TileGroup::DoPaste() {
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
void TileGroup::DoCopy(bool cut) {
  // Generate a double-null terminated list of files to copy.
  DoubleNullStringList files;
  WCHAR buffer[MAX_PATH];

  ClearAllGhosting(!cut);

  for (Tile *tile : mTiles) {
    if (tile->IsSelected()) {
      tile->GetDisplayName(SHGDN_FORPARSING, buffer, _countof(buffer));
      if (cut) {
        tile->SetGhost();
      }
      files.Push(buffer);
    }
  }

  // Put the list in global memory.
  DROPFILES dropObj = { sizeof(DROPFILES), { 0, 0 }, 0, 1 };
  HGLOBAL clipData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(dropObj) + files.GetStringSize());
  LPBYTE data = (LPBYTE)GlobalLock(clipData);
  memcpy(data, &dropObj, sizeof(dropObj));
  memcpy(data + sizeof(dropObj), files.GetStringList(), files.GetStringSize());
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
void TileGroup::DeleteSelectedFiles() {
  // Generate a double-null terminated list of files to copy.
  DoubleNullStringList files;
  WCHAR buffer[MAX_PATH];
  for (Tile *tile : mTiles) {
    if (tile->IsSelected()) {
      tile->GetDisplayName(SHGDN_FORPARSING, buffer, _countof(buffer));
      files.Push(buffer);
    }
  }

  // Delete the files.
  SHFILEOPSTRUCTW shFileOp;
  ZeroMemory(&shFileOp, sizeof(shFileOp));
  shFileOp.wFunc = FO_DELETE;
  shFileOp.hwnd = nullptr;
  shFileOp.pFrom = files.GetStringList();
  shFileOp.pTo = nullptr;
  shFileOp.fFlags = FOF_ALLOWUNDO | FOF_WANTNUKEWARNING;
  SHFileOperationW(&shFileOp);
}


/// <summary>
/// Opens the selected files.
/// </summary>
void TileGroup::OpenSelectedFiles() {
  for (Tile *tile : mTiles) {
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
void TileGroup::RenameSelectedFiles() {
}


/// <summary>
///
/// </summary>
void TileGroup::ClearAllGhosting(bool repaint) {
  if (mClipBoardCutFiles) {
    mClipBoardCutFiles = false;
    for (Tile *tile : mTiles) {
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
void TileGroup::StartRectangleSelection(D2D1_POINT_2U point) {
  mRectangleStart = point;
  mInRectangleSelection = true;
  mWindow->DisableMouseForwarding();
  mWindow->SetMouseCapture();
}


/// <summary>
///
/// </summary>
void TileGroup::EndRectangleSelection(D2D1_POINT_2U point) {
  mInRectangleSelection = false;
  mSelectionRectagle.Hide();
  mWindow->ReleaseMouseCapture();
  mWindow->Repaint(&mSelectionRectagle.GetRect());

  D2D1_RECT_F rect = D2D1::RectF(
    float(std::min(mRectangleStart.x, point.x)),
    float(std::min(mRectangleStart.y, point.y)),
    float(std::max(mRectangleStart.x, point.x)),
    float(std::max(mRectangleStart.y, point.y))
  );

  for (Tile *tile : mTiles) {
    if (tile->IsInRect(rect)) {
      tile->Select();
    } else {
      tile->Deselect();
    }
  }

  mWindow->EnableMouseForwarding();
  mWindow->Repaint(&rect);
}


/// <summary>
///
/// </summary>
void TileGroup::MoveRectangleSelection(D2D1_POINT_2U point) {
  D2D1_RECT_F rect = D2D1::RectF(
    float(std::min(mRectangleStart.x, point.x)),
    float(std::min(mRectangleStart.y, point.y)),
    float(std::max(mRectangleStart.x, point.x)),
    float(std::max(mRectangleStart.y, point.y))
  );

  mWindow->Repaint(&mSelectionRectagle.GetRect());

  mSelectionRectagle.SetRect(rect);

  for (Tile *tile : mTiles) {
    if (tile->IsInRect(rect)) {
      tile->Select();
    } else {
      tile->Deselect();
    }
  }

  mSelectionRectagle.Show();
  mWindow->Repaint(&rect);
}


/// <summary>
/// Handles changes to the clipboard.
/// </summary>
void TileGroup::HandleClipboardChange() {
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

UINT TileGroup::GetIconLoadedMessage() const {
  return mIconLoadedMessage;
}


/// <summary>
/// Handles change notifications for the current folder.
/// </summary>
LRESULT TileGroup::HandleChangeNotify(HANDLE changeHandle, DWORD processId) {
  long event;
  PIDLIST_ABSOLUTE *idList;
  HANDLE notifyLock = SHChangeNotification_Lock(changeHandle, processId, &idList, &event);

  if (notifyLock) {
    switch (event) {
    case SHCNE_ATTRIBUTES:
    case SHCNE_UPDATEITEM:
    case SHCNE_UPDATEDIR:
      UpdateIcon(ILFindLastID(idList[0]));
      break;

    case SHCNE_MKDIR:
    case SHCNE_CREATE:
      AddIcon(ILFindLastID(idList[0]));
      break;

    case SHCNE_RMDIR:
    case SHCNE_DELETE:
      RemoveIcon(ILFindLastID(idList[0]));
      break;

    case SHCNE_RENAMEITEM:
    case SHCNE_RENAMEFOLDER:
      RenameIcon(ILFindLastID(idList[0]), ILFindLastID(idList[1]));
      break;

    case SHCNE_ASSOCCHANGED:
    case SHCNE_UPDATEIMAGE:
      UpdateAllIcons();
      break;
    }

    SHChangeNotification_Unlock(notifyLock);
  }
  return 0;
}


/// <summary>
/// Handles window messages.
/// </summary>
LRESULT WINAPI TileGroup::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
  if (message == mChangeNotifyMsg) {
    return HandleChangeNotify(HANDLE(wParam), DWORD(lParam));
  }

  if (mContextMenu3) {
    LRESULT result;
    if (SUCCEEDED(mContextMenu3->HandleMenuMsg2(message, wParam, lParam, &result))) {
      return result;
    }
  }

  if (mContextMenu2) {
    if (SUCCEEDED(mContextMenu2->HandleMenuMsg(message, wParam, lParam))) {
      return 0;
    }
  }

  switch (message) {
  case WM_KEYDOWN:
    switch (wParam) {
    case 'A':
      if (GetKeyState(VK_CONTROL) < 0) {
        SelectAll();
      }
      break;

    case 'C':
      if (GetKeyState(VK_CONTROL) < 0) {
        DoCopy(false);
      }
      break;

    case 'V':
      if (GetKeyState(VK_CONTROL) < 0) {
        DoPaste();
      }
      break;

    case 'X':
      if (GetKeyState(VK_CONTROL) < 0) {
        DoCopy(true);
      }
      break;

    case 'Z':
      if (GetKeyState(VK_CONTROL) < 0) {
        DoUndo();
      }
      break;

    case VK_DELETE:
      DeleteSelectedFiles();
      break;

    case VK_RETURN:
      OpenSelectedFiles();
      break;

    case VK_F2:
      RenameSelectedFiles();
      break;

    case VK_F5:
      UpdateAllIcons();
      break;

    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
      break;
    }
    break;

  case WM_LBUTTONDOWN:
  case WM_RBUTTONDOWN:
    if (!mInRectangleSelection) {
      StartRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
    } else {
      EndRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
    }
    break;

  case WM_MOUSEMOVE:
    if (mInRectangleSelection) {
      MoveRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
    }
    break;

  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
    if (mInRectangleSelection) {
      EndRectangleSelection(D2D1::Point2U(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
    } else if (GetKeyState(VK_CONTROL) >= 0) {
      DeselectAll();
    }
    break;

  case Window::WM_TOPPARENTLOST:
    mChangeNotifyMsg = 0;
    if (mChangeNotifyUID != 0) {
      SHChangeNotifyDeregister(mChangeNotifyUID);
      mChangeNotifyUID = 0;
    }
    break;

  case Window::WM_NEWTOPPARENT:
    {
      // TODO(Erik): This is unacceptable. We have to pick up change notifications while we don't
      // have a top parent.
      IPersistFolder2 *ipsf2;
      LPITEMIDLIST curFolder;
      HRESULT hr;

      mChangeNotifyMsg = mWindow->RegisterUserMessage(this);

      if (SUCCEEDED(hr = mWorkingFolder->QueryInterface(IID_IPersistFolder2, (LPVOID*)&ipsf2))) {
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
