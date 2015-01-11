/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.cpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include "TileSettings.hpp"
#include "Tile.hpp"
#include "TileGroup.hpp"
#include "../nShared/LSModule.hpp"
#include <Thumbcache.h>
#include <CommonControls.h>
#include "../Utilities/Math.h"
#include <algorithm>
#include <thread>


Tile::Tile(Drawable* parent, PCITEMID_CHILD item, IShellFolder2 *shellFolder, int width, int height, TileSettings &tileSettings, LoadThumbnailResponse &thumbnail)
    : Drawable(parent, L"Icon")
    , mTileSettings(tileSettings)
    , mMouseOver(false)
    , mGhosted(false)
{
    WCHAR name[MAX_PATH];

    mPositionID = 0;
    mShellFolder = shellFolder;
    mItem = (PITEMID_CHILD)malloc(item->mkid.cb + 2);
    memcpy(mItem, item, item->mkid.cb + 2);

    GetDisplayName(SHGDN_NORMAL, name, MAX_PATH);

    mWindow->Initialize(mTileSettings.mTileWindowSettings, &mTileSettings.mTileStateRender);
    mWindow->SetText(name);
    mWindow->Resize((float)width, (float)height);

    SetThumbnail(thumbnail);

    mWindow->Show();
}


Tile::~Tile() {
    free(mItem);
}


HRESULT Tile::CompareID(PCITEMID_CHILD id) {
    return mShellFolder->CompareIDs(0, mItem, id);
}


LRESULT WINAPI Tile::HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
  switch(msg) {
  case WM_MOUSEMOVE:
    if (!mMouseOver) {
      mMouseOver = true;
      mTileSettings.mTileStateRender.ActivateState(State::Hover, mWindow);
    }
    return 0;

  case WM_MOUSELEAVE:
    mMouseOver = false;
    mTileSettings.mTileStateRender.ClearState(State::Hover, mWindow);
    return 0;

  case WM_LBUTTONDBLCLK:
    WCHAR command[MAX_LINE_LENGTH];
    GetDisplayName(SHGDN_FORPARSING, command, MAX_LINE_LENGTH);
    ShellExecuteW(nullptr, nullptr, command, nullptr, nullptr, SW_SHOW);
    return 0;

  case WM_LBUTTONDOWN:
    if (GetKeyState(VK_CONTROL) >= 0) {
      ((TileGroup*)mParent)->DeselectAll();
      mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
    } else {
      mTileSettings.mTileStateRender.ToggleState(State::Selected, mWindow);
    }
    return 0;

case WM_RBUTTONDOWN:
    if (GetKeyState(VK_CONTROL) >= 0 && !IsSelected())
    {
      ((TileGroup*)mParent)->DeselectAll();
    }
    mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
    ((TileGroup*)mParent)->ContextMenu();
    return 0;

  default:
    return DefWindowProc(wnd, msg, wParam, lParam);
  }
}


PCITEMID_CHILD Tile::GetItem() {
  return mItem;
}


bool Tile::IsInRect(D2D1_RECT_F rect) {
  return RectIntersectArea(rect, mWindow->GetDrawingRect()) > 0;
}


void Tile::Select() {
  mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
}


void Tile::Deselect(){
  mTileSettings.mTileStateRender.ClearState(State::Selected, mWindow);
}


bool Tile::IsSelected() {
  return mTileSettings.mTileStateRender.IsStateActive(State::Selected, mWindow);
}


int Tile::GetPositionID() {
  return mPositionID;
}


void Tile::SetPosition(int id, int x, int y) {
  mPositionID = id;
  mWindow->Move((float)x, (float)y);
}


void Tile::Hide() {
  mWindow->Hide();
}


/// <summary>
/// Updates the icon.
/// </summary>
/// <param name="repaint">Repaints the icon.</param>
void Tile::UpdateIcon(bool repaint) {
  mWindow->ClearOverlays();
  if (repaint) {
    mWindow->Repaint();
  }
}


/// <summary>
/// Updates the displayed name for this icon.
/// </summary>
/// <param name="newItem">The item ID for the new icon.</param>
void Tile::Rename(PCITEMID_CHILD newItem) {
  mItem = (PITEMID_CHILD)realloc(mItem, newItem->mkid.cb + 2);
  memcpy(mItem, newItem, newItem->mkid.cb + 2);
    
  WCHAR newName[MAX_PATH];
  GetDisplayName(SHGDN_NORMAL, newName, MAX_PATH);
  mWindow->SetText(newName);
  mWindow->Repaint();
}


/// <summary>
/// Enabled ghots mode -- i.e. when the tile is "cut"
/// </summary>
bool Tile::IsGhosted() {
  return mGhosted;
}


/// <summary>
/// Enabled ghots mode -- i.e. when the tile is "cut"
/// </summary>
void Tile::SetGhost() {
  mIconOverlay->GetBrush()->SetOpacity(mTileSettings.mGhostOpacity);
  mGhosted = true;
}


/// <summary>
/// Enabled ghots mode -- i.e. when the tile is "cut"
/// </summary>
void Tile::ClearGhost() {
  mIconOverlay->GetBrush()->SetOpacity(1.0f);
  mGhosted = false;
}


/// <summary>
/// Called by the parent when our icon has been loaded.
/// </summary>
void Tile::SetThumbnail(LoadThumbnailResponse &thumbnail) {
  mWindow->ClearOverlays();
  D2D1_RECT_F pos;
  pos.top = 0;
  pos.bottom = pos.top + mTileSettings.mIconSize;
  pos.left = (mWindow->GetSize().width - (float)mTileSettings.mIconSize) / 2;
  pos.right = pos.left + mTileSettings.mIconSize;

  if (thumbnail.size.width > thumbnail.size.height) {
    float scale = thumbnail.size.height / thumbnail.size.width;
    pos.top = mTileSettings.mIconSize*(1 - scale) / 2;
    pos.bottom = mTileSettings.mIconSize*(1 + scale) / 2;
  } else if (thumbnail.size.width < thumbnail.size.height) {
    float scale = thumbnail.size.width / thumbnail.size.height;
    pos.left = mTileSettings.mIconSize*(1 - scale) / 2;
    pos.right = mTileSettings.mIconSize*(1 + scale) / 2;
  }

  if (thumbnail.type == LoadThumbnailResponse::Type::HBITMAP) {
    mIconOverlay = mWindow->AddOverlay(pos, thumbnail.thumbnail.bitmap);
  } else if (thumbnail.type == LoadThumbnailResponse::Type::HICON) {
    mIconOverlay = mWindow->AddOverlay(pos, thumbnail.thumbnail.icon);
  } else {
    ASSERT(false);
  }
  mWindow->Repaint();
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT Tile::GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf) {
  STRRET ret;
  HRESULT hr;

  hr = mShellFolder->GetDisplayNameOf(mItem, flags, &ret);

  if (SUCCEEDED(hr)) {
    hr = StrRetToBufW(&ret, mItem, buf, cchBuf);
  }

  return hr;
}
