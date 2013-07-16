/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconTile.hpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include <ShlObj.h>

class IconTile : public Drawable {
public:
    explicit IconTile(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder, int width, int height);
    virtual ~IconTile();

public:
    HRESULT CompareID(PCITEMID_CHILD id);
    void SetPosition(int x, int y, bool noRedraw = false);
    LRESULT WINAPI HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID);
    HRESULT GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf);

public:
    // Hides this icon.
    void Hide();

    // Renames this item.
    void Rename(PCITEMID_CHILD newItem);

    // Updates the icon.
    void UpdateIcon(bool repaint = true);

    // Shows the right-click menu for the icon.
    void ShowContextMenu();

private:
    // Sets the icon.
    void SetIcon();

private:
    // Pointer to the shellfolder this item is in.
    IShellFolder2* mShellFolder;

    // The PID of this icon.
    PITEMID_CHILD mItem;

    //
    int mIconSize;

    // 
    DrawableWindow::STATE mHoverState, mSelectedState, mFocusedState;

    //
    DrawableWindow::OVERLAY mIconOverlay;

    // True if the mouse is currently above the icon.
    bool mMouseOver;

    //
    bool mMouseDown;

    // Where the mouse was when the 
    POINT mMouseDownPosition;
};
