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
#include "IconTile.hpp"
#include "IconGroup.hpp"
#include "../nShared/LSModule.hpp"
#include <Thumbcache.h>
#include <CommonControls.h>
#include "../Utilities/Math.h"


IconTile::IconTile(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder, int width, int height, TileSettings &tileSettings)
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
    mWindow->Resize(width, height);

    SetIcon();

    mWindow->Show();
}


IconTile::~IconTile() {
    free(mItem);
}


HRESULT IconTile::CompareID(PCITEMID_CHILD id) {
    return mShellFolder->CompareIDs(0, mItem, id);
}


LRESULT WINAPI IconTile::HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch(msg)
    {
    case WM_MOUSEMOVE:
        {
            if (!mMouseOver)
            {
                mMouseOver = true;
                mTileSettings.mTileStateRender.ActivateState(State::Hover, mWindow);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mMouseOver = false;
            mTileSettings.mTileStateRender.ClearState(State::Hover, mWindow);
        }
        return 0;

    case WM_LBUTTONDBLCLK:
        {
            WCHAR command[MAX_LINE_LENGTH];
            GetDisplayName(SHGDN_FORPARSING, command, MAX_LINE_LENGTH);
            ShellExecuteW(nullptr, nullptr, command, nullptr, nullptr, SW_SHOW);
        }
        return 0;

    case WM_LBUTTONDOWN:
        {
            if (GetKeyState(VK_CONTROL) >= 0)
            {
                ((IconGroup*)mParent)->DeselectAll();
                mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
            }
            else
            {
                mTileSettings.mTileStateRender.ToggleState(State::Selected, mWindow);
            }
        }
        return 0;

    case WM_RBUTTONDOWN:
        {
            if (GetKeyState(VK_CONTROL) >= 0 && !IsSelected())
            {
                ((IconGroup*)mParent)->DeselectAll();
            }
            mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
            ((IconGroup*)mParent)->ContextMenu();
        }
        return 0;

    default:
        return DefWindowProc(wnd, msg, wParam, lParam);
    }
}


PCITEMID_CHILD IconTile::GetItem() {
    return mItem;
}


bool IconTile::IsInRect(D2D1_RECT_F rect)
{
    return Math::RectIntersectArea(rect, mWindow->GetDrawingRect()) > 0;
}


void IconTile::Select(bool repaint)
{
    mTileSettings.mTileStateRender.ActivateState(State::Selected, mWindow);
}


void IconTile::Deselect(bool repaint)
{
    mTileSettings.mTileStateRender.ClearState(State::Selected, mWindow);
}


bool IconTile::IsSelected()
{
    return mTileSettings.mTileStateRender.IsStateActive(State::Selected, mWindow);
}


int IconTile::GetPositionID() {
    return mPositionID;
}


void IconTile::SetPosition(int id, int x, int y) {
    mPositionID = id;
    mWindow->Move(x, y);
}


void IconTile::Hide() {
    mWindow->Hide();
}


/// <summary>
/// Updates the icon.
/// </summary>
/// <param name="repaint">Repaints the icon.</param>
void IconTile::UpdateIcon(bool repaint) {
    mWindow->ClearOverlays();
    SetIcon();
    if (repaint) {
        mWindow->Repaint();
    }
}


/// <summary>
/// Updates the displayed name for this icon.
/// </summary>
/// <param name="newItem">The item ID for the new icon.</param>
void IconTile::Rename(PCITEMID_CHILD newItem) {
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
bool IconTile::IsGhosted() {
    return mGhosted;
}


/// <summary>
/// Enabled ghots mode -- i.e. when the tile is "cut"
/// </summary>
void IconTile::SetGhost() {
    mIconOverlay->GetBrush()->SetOpacity(mTileSettings.mGhostOpacity);
    mGhosted = true;
}


/// <summary>
/// Enabled ghots mode -- i.e. when the tile is "cut"
/// </summary>
void IconTile::ClearGhost() {
    mIconOverlay->GetBrush()->SetOpacity(1.0f);
    mGhosted = false;
}


/// <summary>
/// Sets the icon of this item.
/// </summary>
void IconTile::SetIcon() {
    IExtractImage *extractImage = nullptr;
    IExtractIconW *extractIcon = nullptr;
    IThumbnailProvider *thumbnailProvider = nullptr;
    HICON icon = nullptr;
    WCHAR iconFile[MAX_PATH];
    int iconIndex = 0;
    UINT flags;
    D2D1_RECT_F pos;
    HRESULT hr;

    pos.top = 0;
    pos.bottom = pos.top + mTileSettings.mIconSize;
    pos.left = (mWindow->GetSize().width - (float)mTileSettings.mIconSize)/2;
    pos.right = pos.left + mTileSettings.mIconSize;

    // First, lets try IThumbnailProvider
    hr = mShellFolder->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST *)&mItem, IID_IThumbnailProvider, nullptr, reinterpret_cast<LPVOID*>(&thumbnailProvider));
    if (SUCCEEDED(hr)) {
        HBITMAP hBMP = nullptr;
        WTS_ALPHATYPE alphaType;

        hr = thumbnailProvider->GetThumbnail(mTileSettings.mIconSize, &hBMP, &alphaType);

        if (SUCCEEDED(hr)) {
            BITMAP bmp;
            GetObjectW(hBMP, sizeof(BITMAP), &bmp);

            if (bmp.bmWidth > bmp.bmHeight) {
                float scale = float(bmp.bmHeight) / float(bmp.bmWidth);

                pos.top = mTileSettings.mIconSize*(1 - scale)/2;
                pos.bottom = mTileSettings.mIconSize*(1 + scale)/2;
            }
            else if (bmp.bmWidth < bmp.bmHeight) {
                float scale = float(bmp.bmWidth) / float(bmp.bmHeight);

                pos.left = mTileSettings.mIconSize*(1 - scale)/2;
                pos.right = mTileSettings.mIconSize*(1 + scale)/2;
            }

            mIconOverlay = mWindow->AddOverlay(pos, hBMP);
        }

        // Let go of the interface.
        SAFERELEASE(thumbnailProvider);
    }
    
    // If that fails, lets try IExtractImage
    if (hr != S_OK) {
        hr = mShellFolder->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST *)&mItem, IID_IExtractImage, nullptr, reinterpret_cast<LPVOID*>(&extractImage));

        if (SUCCEEDED(hr)) {
            HBITMAP hBMP = nullptr;
            WCHAR location[MAX_PATH];
            SIZE size = { mTileSettings.mIconSize, mTileSettings.mIconSize };
            DWORD flags = 0;

            hr = extractImage->GetLocation(location, _countof(location), nullptr, &size, 0, &flags);
        
            if (SUCCEEDED(hr)) {
                hr = extractImage->Extract(&hBMP);
            }

            if (SUCCEEDED(hr)) {
                mIconOverlay = mWindow->AddOverlay(pos, hBMP);
            }

            // Let go of the interface.
            SAFERELEASE(extractImage);
        }
    }

    // Finally, fall back to IExtractIcon
    if (hr != S_OK) {
        hr = mShellFolder->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST *)&mItem, IID_IExtractIconW, nullptr, reinterpret_cast<LPVOID*>(&extractIcon));

        // Get the location of the file containing the appropriate icon, and the index of the icon.
        if (SUCCEEDED(hr)) {
            hr = extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);
        }

        // Extract the icon.
        if (SUCCEEDED(hr)) {
            if (wcscmp(iconFile, L"*") == 0) { // * always leads to bogus icons (32x32) :/
                IImageList *imageList;
                SHGetImageList(mTileSettings.mIconSize > 48 ? SHIL_JUMBO : (mTileSettings.mIconSize > 32 ? SHIL_EXTRALARGE : (mTileSettings.mIconSize > 16 ? SHIL_LARGE : SHIL_SMALL)),
                    IID_IImageList, reinterpret_cast<LPVOID*>(&imageList));
                hr = imageList->GetIcon(iconIndex, ILD_TRANSPARENT, &icon);
                SAFERELEASE(imageList);
            }
            else {
                hr = extractIcon->Extract(iconFile, iconIndex, &icon, nullptr, MAKELONG(mTileSettings.mIconSize, 0));
            }
        }

        // If the extraction failed, fall back to a 32x32 icon.
        if (hr == S_FALSE) {
            hr = extractIcon->Extract(iconFile, iconIndex, &icon, nullptr, MAKELONG(32, 0));
        }

        // Add it as an overlay.
        if (hr == S_OK) {
            mIconOverlay = mWindow->AddOverlay(pos, icon);
        }

        // Let go of the interface.
        SAFERELEASE(extractIcon);
    }

    if (hr != S_OK) {
        mIconOverlay = mWindow->AddOverlay(pos, LoadIcon(nullptr, IDI_ERROR));
    }
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT IconTile::GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf) {
    STRRET ret;
    HRESULT hr;

    hr = mShellFolder->GetDisplayNameOf(mItem, flags, &ret);

    if (SUCCEEDED(hr)) {
        hr = StrRetToBufW(&ret, mItem, buf, cchBuf);
    }

    return hr;
}
