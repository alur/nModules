/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.hpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"
#include <ShlObj.h>

class Icon : public Drawable {
public:
    explicit Icon(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder);
    virtual ~Icon();

    HRESULT CompareID(PCITEMID_CHILD id);
    void SetPosition(int x, int y, bool noRedraw = false);
    LRESULT WINAPI HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID);
    HRESULT GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf);

    // Hides this icon.
    void Hide();

    // Renames this item.
    void Rename(PCITEMID_CHILD newItem);

    // Updates the icon.
    void UpdateIcon(bool repaint = true);

    // Shows the right-click menu for the icon.
    void ShowContextMenu();

private:
    // Pointer to the shellfolder this item is in.
    IShellFolder2* shellFolder;

    // The PID of this icon.
    PITEMID_CHILD item;

    // Sets the icon.
    void SetIcon();

    // 
    DrawableWindow::STATE hoverState, selectedState, focusedState;

    //
    DrawableWindow::OVERLAY iconOverlay;

    // True if the mouse is currently above the icon.
    bool mouseOver;

    //
    bool mouseDown;

    // The last place the mouse was 
    POINT mouseDownPosition;
};
