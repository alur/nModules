/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.hpp
 *  The nModules Project
 *
 *  A group of icons. Essentially an *nIcon.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Tile.hpp"
#include "TileSettings.hpp"
#include "SelectionRectangle.hpp"

#include "../nCoreCom/Core.h"

#include "../nShared/Drawable.hpp"
#include "../nShared/LayoutSettings.hpp"
#include "../nShared/Settings.hpp"
#include "../nShared/Window.hpp"

#include "../Utilities/StringUtils.h"

#include <set>
#include <ShlObj.h>
#include <unordered_set>

class TileGroup : public Drawable, public FileSystemLoaderResponseHandler {
public:
  enum class State {
    Base,
    Count
  };

public:
  explicit TileGroup(LPCTSTR prefix);
  ~TileGroup();

  // FileSystemLoaderResponseHandler
public:
  LPARAM FolderLoaded(UINT64, LoadFolderResponse*) override;
  LPARAM ItemLoaded(UINT64, LoadItemResponse*) override;

public:
  LRESULT WINAPI HandleMessage(HWND, UINT msg, WPARAM, LPARAM, LPVOID);

private:
  LRESULT HandleChangeNotify(HANDLE changeHandle, DWORD processId);

public:
  void SetFolder(LPWSTR path);

  void SelectAll();
  void DeselectAll();
  void ContextMenu();
  void HandleClipboardChange();
  void ClearAllGhosting(bool repaint);
  UINT GetIconLoadedMessage() const;

private:
  HRESULT GetDisplayNameOf(PCITEMID_CHILD pidl, SHGDNF flags, LPWSTR buf, UINT cchBuf) const;
  HRESULT GetFolderPath(LPWSTR buf, UINT cchBuf) const;
  void AddIcon(PCITEMID_CHILD pidl);
  void RemoveIcon(PCITEMID_CHILD pidl);
  void UpdateIcon(PCITEMID_CHILD pidl);
  void UpdateAllIcons();
  void RenameIcon(PCITEMID_CHILD oldID, PCITEMID_CHILD newID);
  int GetIconPosition(PCITEMID_CHILD);
  Tile *FindIcon(PCITEMID_CHILD) const;

  void ImportFolderContents();
  void AddNameFilter(LPCWSTR name);
  void LoadSettings();

public:
  bool WantsItem(PCITEMID_CHILD) const;

private:
  LayoutSettings mLayoutSettings;
  TileSettings mTileSettings;

  int mTileWidth;
  int mTileHeight;

  // Items which should not be shown on the desktop
  StringKeyedSets<std::wstring>::UnorderedSet mHiddenItems;

private:
  StateRender<State> mStateRender;

  // Positioning
  std::set<int> mEmptySpots;
  int mNextPositionID;

  // All icons currently part of this group.
  std::list<Tile*> mTiles;

  // Return value of the latest SHChangeNofityRegister call.
  ULONG mChangeNotifyUID;
  UINT mChangeNotifyMsg;
  UINT mIconLoadedMessage;

  // IShellFolder for the folder we are currently in.
  IShellFolder2 *mWorkingFolder;

  // IShellFolder for the root of the file system.
  IShellFolder2 *mRootFolder;

  // True if we have files marked as cut.
  bool mClipBoardCutFiles;

  // Copy & Paste
private:
  void DoPaste();
  void DoUndo();
  void DoCopy(bool cut);
  void DeleteSelectedFiles();
  void OpenSelectedFiles();
  void RenameSelectedFiles();

  // Rectangle selection
private:
  void StartRectangleSelection(D2D1_POINT_2U point);
  void EndRectangleSelection(D2D1_POINT_2U point);
  void MoveRectangleSelection(D2D1_POINT_2U point);
  D2D1_POINT_2U mRectangleStart;
  bool mInRectangleSelection;
  SelectionRectangle mSelectionRectagle;

  // Context menu temps
private:
  IContextMenu2 *mContextMenu2;
  IContextMenu3 *mContextMenu3;
};
