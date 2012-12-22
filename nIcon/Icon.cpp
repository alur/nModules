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
#include "../nShared/Macros.h"
#include "Icon.hpp"
#include "../nShared/LSModule.hpp"

// 
extern LSModule* g_LSModule;


Icon::Icon(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder) : Drawable(parent, "Icon") {
    WCHAR name[MAX_PATH];

    this->shellFolder = shellFolder;
    this->item = (PITEMID_CHILD)malloc(item->mkid.cb + 2);
    memcpy(this->item, item, item->mkid.cb + 2);

    DrawableSettings defaults;
    defaults.width = 64;
    defaults.height = 120;
    GetDisplayName(SHGDN_NORMAL, name, MAX_PATH);

    StateSettings baseStateDefaults;
    baseStateDefaults.backgroundBrush.color = 0;
    baseStateDefaults.wordWrap = true;
    baseStateDefaults.textOffsetTop = 64;
    StringCchCopy(baseStateDefaults.textAlign, sizeof(baseStateDefaults.textAlign), "Center");

    this->window->Initialize(&defaults, &baseStateDefaults);
    this->window->SetText(name);

    SetIcon();

    this->window->SizeToText(64, 300, 64);

    StateSettings hoverDefaults(baseStateDefaults);
    hoverDefaults.backgroundBrush.color = 0xAA87CEEB;
    hoverDefaults.outlineBrush.color = 0x99FFFFFF;
    hoverDefaults.outlineWidth = 1.5f;
    this->hoverState = this->window->AddState("Hover", 100, &hoverDefaults);

    StateSettings selectedDefaults(hoverDefaults);
    selectedDefaults.backgroundBrush.color = 0xCC87CEEB;
    selectedDefaults.outlineBrush.color = 0xCCFFFFFF;
    selectedDefaults.outlineWidth = 1.5f;
    this->selectedState = this->window->AddState("Selected", 150, &selectedDefaults);

    StateSettings focusedDefaults(hoverDefaults);
    focusedDefaults.backgroundBrush.color = 0xAA87CEEB;
    focusedDefaults.outlineBrush.color = 0x99FFFFFF;
    focusedDefaults.outlineWidth = 1.5f;
    this->focusedState = this->window->AddState("Focused", 200, &focusedDefaults);
    
    this->mouseOver = false;

    this->window->Show();
}


Icon::~Icon() {
    free(this->item);
}


HRESULT Icon::CompareID(PCITEMID_CHILD id) {
    return this->shellFolder->CompareIDs(0, this->item, id);
}


LRESULT WINAPI Icon::HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch(msg) {
    case WM_MOUSEMOVE:
        {
            if (!this->mouseOver) {
                this->mouseOver = true;
                this->window->ActivateState(this->hoverState);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->mouseOver = false;
            this->window->ClearState(this->hoverState);
        }
        return 0;

    case WM_LBUTTONDBLCLK:
        {
            WCHAR command[MAX_LINE_LENGTH];
            this->GetDisplayName(SHGDN_FORPARSING, command, MAX_LINE_LENGTH);
            ShellExecuteW(NULL, NULL, command, NULL, NULL, SW_SHOW);
        }
        return 0;

    case WM_LBUTTONDOWN:
        {
            this->window->ToggleState(this->selectedState);
        }
        return 0;

    case WM_RBUTTONDOWN:
        {
            IContextMenu* contextMenu;
            HMENU menu;

            this->shellFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&this->item, IID_IContextMenu, NULL, reinterpret_cast<LPVOID*>(&contextMenu));

            menu = CreatePopupMenu();
            contextMenu->QueryContextMenu(menu, 0, 0, 0, CMF_NORMAL);

            POINT pt;
            GetCursorPos(&pt);

            int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->window->GetWindowHandle(), NULL);
            if (command != 0) {
                CMINVOKECOMMANDINFO info = { 0 };
                char verb[MAX_LINE_LENGTH];
                contextMenu->GetCommandString(command, GCS_VERBA, NULL, verb, sizeof(verb));
                info.cbSize = sizeof(info);
                info.hwnd = NULL;
                info.lpVerb = verb;
                contextMenu->InvokeCommand(&info);
            }

            DestroyMenu(menu);
            contextMenu->Release();
        }
        return 0;

    default:
        return DefWindowProc(wnd, msg, wParam, lParam);
    }

}


void Icon::SetPosition(int x, int y, bool noRedraw) {
    this->window->Move(x, y);
    if (!noRedraw) {
        this->window->Repaint();
    }
}


void Icon::Hide() {
    this->window->Hide();
}


/// <summary>
/// Updates the icon.
/// </summary>
/// <param name="repaint">Repaints the icon.</param>
void Icon::UpdateIcon(bool repaint) {
    this->window->ClearOverlays();
    SetIcon();
    if (repaint) {
        this->window->Repaint();
    }
}


/// <summary>
/// Updates the displayed name for this icon.
/// </summary>
/// <param name="newItem">The item ID for the new icon.</param>
void Icon::Rename(PCITEMID_CHILD newItem) {
    this->item = (PITEMID_CHILD)realloc(this->item, newItem->mkid.cb + 2);
    memcpy(this->item, newItem, newItem->mkid.cb + 2);
    
    WCHAR newName[MAX_PATH];
    GetDisplayName(SHGDN_NORMAL, newName, MAX_PATH);
    this->window->SetText(newName);
    this->window->Repaint();
}


/// <summary>
/// Sets the icon of this item.
/// </summary>
void Icon::SetIcon() {
    IExtractIconW* extractIcon = NULL;
    HICON icon = NULL;
    WCHAR iconFile[MAX_PATH];
    int iconIndex = 0;
    UINT flags;
    D2D1_RECT_F pos;
    HRESULT hr;

    pos.top = 0; pos.left = 0; pos.right = 64; pos.bottom = 64;

    // Get the IExtractIcon interface for this item.
    hr = this->shellFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *)&this->item, IID_IExtractIconW, NULL, reinterpret_cast<LPVOID*>(&extractIcon));

    // Get the location of the file containing the appropriate icon, and the index of the icon.
    if (SUCCEEDED(hr)) {
        hr = extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);
    }

    // Extract the icon.
    if (SUCCEEDED(hr)) {
        hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
    }

    // If the extraction failed, fall back to a 32x32 icon.
    if (hr == S_FALSE) {
        hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));
    }

    // Add it as an overlay.
    if (hr == S_OK) {
        this->iconOverlay = this->window->AddOverlay(pos, icon);
    }

    // Let go of the interface.
    SAFERELEASE(extractIcon);
}


/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT Icon::GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf) {
    STRRET ret;
    HRESULT hr;

    hr = this->shellFolder->GetDisplayNameOf(this->item, flags, &ret);

    if (SUCCEEDED(hr)) {
        hr = StrRetToBufW(&ret, this->item, buf, cchBuf);
    }

    return hr;
}
