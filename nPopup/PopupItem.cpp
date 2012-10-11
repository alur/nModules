/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "PopupItem.hpp"
#include "../nShared/Macros.h"
#include "../nShared/LSModule.hpp"

extern LSModule* g_LSModule;


PopupItem::PopupItem(Drawable* parent, LPCSTR prefix) : Drawable(parent, prefix) {
    this->iconSettings = this->settings->CreateChild("Icon");
}


PopupItem::~PopupItem() {
    SAFEDELETE(this->iconSettings);
}


void PopupItem::Position(int x, int y) {
    this->window->Move(x, y);
}


int PopupItem::GetHeight() {
    return this->window->GetDrawingSettings()->height;
}


bool PopupItem::ParseDotIcon(LPCSTR dotIcon) {
    if (dotIcon == NULL) {
        return false;
    }

    // TODO::May cause problems with paths which include a comma.
    LPSTR index = (LPSTR)strrchr(dotIcon, ',');
    int nIndex;
    if (index != NULL) {
        index++[0] = NULL;
        nIndex = atoi(index);
    }
    else {
        nIndex = 0;
    }
    
    HICON icon = ExtractIcon(g_LSModule->GetInstance(), dotIcon, nIndex);

    if (icon == NULL) {
        return false;
    }

    AddIcon(icon);
    DestroyIcon(icon);
    return true;
}


void PopupItem::AddIcon(HICON icon) {
    D2D1_RECT_F f;
    f.top = this->iconSettings->GetFloat("X", 2.0f);
    f.bottom = f.top + this->iconSettings->GetFloat("Size", 16.0f);
    f.left = this->iconSettings->GetFloat("Y", 2.0f);
    f.right = f.left + this->iconSettings->GetFloat("Size", 16.0f);
    this->window->AddOverlay(f, icon);
}


bool PopupItem::CompareTo(PopupItem* b) {
    return _wcsicmp(this->window->GetDrawingSettings()->text, b->window->GetDrawingSettings()->text) < 0;
}


void PopupItem::SetIcon(IExtractIconW* extractIcon) {
    HICON icon;
    WCHAR iconFile[MAX_PATH];
    int iconIndex;
    UINT flags;
    HRESULT hr;

    // Get the location of the file containing the appropriate icon, and the index of the icon.
    extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

    // Extract the icon.
    hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
    if (hr == S_FALSE) {
        // If the extraction failed, fall back to a 32x32 icon.
        hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));

        if (hr == S_FALSE) {
            hr = extractIcon->Extract(iconFile, iconIndex, NULL, &icon, MAKELONG(0, 16));
        }
    }

    if (SUCCEEDED(hr)) {
        AddIcon(icon);
    }

    // Let go of the interface.
    extractIcon->Release();
}
