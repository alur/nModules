/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Icon.cpp
 *  The nModules Project
 *
 *  A single icon in an IconGroup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/Macros.h"
#include "Icon.hpp"
#include "../nShared/LSModule.hpp"

// 
extern LSModule* g_LSModule;


Icon::Icon(Drawable* parent, PCITEMID_CHILD item, IShellFolder2* shellFolder) : Drawable(parent, "Icon") {
    this->shellFolder = shellFolder;
    this->item = (PITEMID_CHILD)malloc(item->mkid.cb + 2);
    memcpy(this->item, item, item->mkid.cb + 2);

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 64;
    defaults->height = 90;
    defaults->color = 0;
    GetDisplayName(SHGDN_NORMAL, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    defaults->textOffsetTop = 64;
    StringCchCopy(defaults->textAlign, sizeof(defaults->textAlign), "Center");

    this->window->Initialize(defaults);
    SetIcon();
    this->window->Show();
}


Icon::~Icon() {
    free(this->item);
}


LRESULT WINAPI Icon::HandleMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
    case WM_LBUTTONDBLCLK:
        {
            WCHAR command[MAX_LINE_LENGTH];
            this->GetDisplayName(SHGDN_FORPARSING, command, MAX_LINE_LENGTH);
            ShellExecuteW(NULL, NULL, command, NULL, NULL, SW_SHOW);
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

            int command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, this->window->GetWindow(), NULL);
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


void Icon::SetPosition(int x, int y) {
    this->window->Move(x, y);
}


void Icon::SetIcon() {
    IExtractIconW* extractIcon = NULL;
    HICON icon;
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
        this->window->AddOverlay(pos, icon);
    }

    // Let go of the interface.
    SAFERELEASE(extractIcon);
}

/// <summary>
/// Get's the display name of a particular PIDL
/// </summary>
HRESULT Icon::GetDisplayName(SHGDNF flags, LPWSTR buf, UINT cchBuf) {
    STRRET ret;
    HRESULT hr = S_OK;

    CHECKHR(hr, this->shellFolder->GetDisplayNameOf(this->item, flags, &ret));
    switch (ret.uType) {
    case STRRET_CSTR:
    case STRRET_OFFSET:
        return E_NOTIMPL;
    case STRRET_WSTR:
        StringCchCopyW(buf, cchBuf, ret.pOleStr);
        CoTaskMemFree(ret.pOleStr);
    }

    CHECKHR_END();

    return hr;
}
