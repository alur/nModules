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
#include "IconTile.hpp"
#include "IconGroup.hpp"
#include "../nShared/LSModule.hpp"
#include <Thumbcache.h>
#include <CommonControls.h>
#include "../Utilities/Math.h"


IconTile::IconTile(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder, int width, int height) : Drawable(parent, L"Icon") {
    WCHAR name[MAX_PATH];

    mPositionID = 0;
    mShellFolder = shellFolder;
    mItem = (PITEMID_CHILD)malloc(item->mkid.cb + 2);
    memcpy(mItem, item, item->mkid.cb + 2);

    mIconSize = mSettings->GetInt(L"Size", 48);
    mGhostOpacity = mSettings->GetFloat(L"GhostOpacity", 0.6f);
    mGhosted = false;

    WindowSettings defaults;
    defaults.width = width;
    defaults.height = height;
    GetDisplayName(SHGDN_NORMAL, name, MAX_PATH);

    StateSettings baseStateDefaults;
    baseStateDefaults.backgroundBrush.color = Color::Create(0x00000000);
    baseStateDefaults.wordWrapping = DWRITE_WORD_WRAPPING_WRAP;
    baseStateDefaults.textOffsetTop = (float)mIconSize;
    baseStateDefaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;

    mWindow->Initialize(&defaults, &baseStateDefaults);
    mWindow->SetText(name);

    SetIcon();

    mWindow->Resize(width, height);

    //mWindow->SizeToText(64, 300, 64);

    StateSettings hoverDefaults(baseStateDefaults);
    hoverDefaults.backgroundBrush.color = Color::Create(0xAA87CEEB);
    hoverDefaults.outlineBrush.color = Color::Create(0x99FFFFFF);
    hoverDefaults.outlineWidth = 1.5f;
    mHoverState = mWindow->AddState(L"Hover", 100, &hoverDefaults);

    StateSettings selectedDefaults(hoverDefaults);
    selectedDefaults.backgroundBrush.color = Color::Create(0xCC87CEEB);
    selectedDefaults.outlineBrush.color = Color::Create(0xCCFFFFFF);
    selectedDefaults.outlineWidth = 1.5f;
    mSelectedState = mWindow->AddState(L"Selected", 150, &selectedDefaults);

    StateSettings focusedDefaults(hoverDefaults);
    focusedDefaults.backgroundBrush.color = Color::Create(0xAA87CEEB);
    focusedDefaults.outlineBrush.color = Color::Create(0x99FFFFFF);
    focusedDefaults.outlineWidth = 1.5f;
    mFocusedState = mWindow->AddState(L"Focused", 200, &focusedDefaults);
    
    mMouseOver = false;

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
                mWindow->ActivateState(mHoverState);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mMouseOver = false;
            mWindow->ClearState(mHoverState);
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
            if (GetKeyState(VK_CONTROL) >= 0) {
                ((IconGroup*)mParent)->DeselectAll();
                mWindow->ActivateState(mSelectedState);
            }
            else {
                mWindow->ToggleState(mSelectedState);
            }
        }
        return 0;

    case WM_RBUTTONDOWN:
        {
            if (GetKeyState(VK_CONTROL) >= 0 && !IsSelected()) {
                ((IconGroup*)mParent)->DeselectAll();
            }
            mWindow->ActivateState(mSelectedState);
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


bool IconTile::IsInRect(D2D1_RECT_F rect) {
    return Math::RectIntersectArea(rect, mWindow->GetDrawingRect()) > 0;
}


void IconTile::Select(bool repaint) {
    mWindow->ActivateState(mSelectedState, repaint);
}


void IconTile::Deselect(bool repaint) {
    mWindow->ClearState(mSelectedState, repaint);
}


bool IconTile::IsSelected() {
    return mSelectedState->active;
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
    mIconOverlay->GetBrush()->SetOpacity(mGhostOpacity);
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

    WindowSettings *drawingSettings = mWindow->GetDrawingSettings();

    pos.top = 0;
    pos.bottom = pos.top + mIconSize;
    pos.left = (drawingSettings->width - (float)mIconSize)/2;
    pos.right = pos.left + mIconSize;

    // First, lets try IThumbnailProvider
    hr = mShellFolder->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST *)&mItem, IID_IThumbnailProvider, nullptr, reinterpret_cast<LPVOID*>(&thumbnailProvider));
    if (SUCCEEDED(hr)) {
        HBITMAP hBMP = nullptr;
        WTS_ALPHATYPE alphaType;

        hr = thumbnailProvider->GetThumbnail(mIconSize, &hBMP, &alphaType);

        if (SUCCEEDED(hr)) {
            BITMAP bmp;
            GetObjectW(hBMP, sizeof(BITMAP), &bmp);

            if (bmp.bmWidth > bmp.bmHeight) {
                float scale = float(bmp.bmHeight) / float(bmp.bmWidth);

                pos.top = mIconSize*(1 - scale)/2;
                pos.bottom = mIconSize*(1 + scale)/2;
            }
            else if (bmp.bmWidth < bmp.bmHeight) {
                float scale = float(bmp.bmWidth) / float(bmp.bmHeight);

                pos.left = mIconSize*(1 - scale)/2;
                pos.right = mIconSize*(1 + scale)/2;
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
            SIZE size = { mIconSize, mIconSize };
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
                SHGetImageList(mIconSize > 48 ? SHIL_JUMBO : (mIconSize > 32 ? SHIL_EXTRALARGE : (mIconSize > 16 ? SHIL_LARGE : SHIL_SMALL)),
                    IID_IImageList, reinterpret_cast<LPVOID*>(&imageList));
                hr = imageList->GetIcon(iconIndex, ILD_TRANSPARENT, &icon);
                SAFERELEASE(imageList);
            }
            else {
                hr = extractIcon->Extract(iconFile, iconIndex, &icon, nullptr, MAKELONG(mIconSize, 0));
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
