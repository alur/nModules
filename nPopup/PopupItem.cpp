/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Popup.hpp"
#include "../nShared/Macros.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"


extern LSModule gLSModule;


PopupItem::PopupItem(Drawable* parent, LPCSTR prefix, bool independent) : Drawable(parent, prefix, independent) {
    this->iconSettings = this->settings->CreateChild("Icon");
    this->itemType = PopupItemType::SEPARATOR;
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


void PopupItem::SetWidth(int width) {
    this->window->Resize(width, this->window->GetDrawingSettings()->height);
}


bool PopupItem::CheckMerge(LPCWSTR name) {
    return this->itemType == PopupItemType::FOLDER && _wcsicmp(name, this->window->GetText()) == 0;
}


bool PopupItem::ParseDotIcon(LPCSTR dotIcon) {
    if (dotIcon == NULL || ((Popup*)this->parent)->noIcons) {
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
    
    HICON icon = ExtractIcon(gLSModule.GetInstance(), dotIcon, nIndex);

    if (icon == NULL) {
        return false;
    }

    AddIcon(icon);
    DestroyIcon(icon);
    return true;
}


void PopupItem::AddIcon(HICON icon) {
    D2D1_RECT_F f;
    f.top = this->iconSettings->GetFloat("Y", 2.0f);
    f.bottom = f.top + this->iconSettings->GetFloat("Size", 16.0f);
    f.left = this->iconSettings->GetFloat("X", 2.0f);
    f.right = f.left + this->iconSettings->GetFloat("Size", 16.0f);
    this->iconOverlay = this->window->AddOverlay(f, icon);
}


bool PopupItem::CompareTo(PopupItem* b) {
    return this->itemType > b->itemType || this->itemType == b->itemType && _wcsicmp(this->window->GetText(), b->window->GetText()) < 0;
}


void PopupItem::SetIcon(IExtractIconW* extractIcon) {
    HICON icon = NULL;
    WCHAR iconFile[MAX_PATH];
    int iconIndex = 0;
    UINT flags;
    HRESULT hr;

    if (!((Popup*)this->parent)->noIcons) {

        // Get the location of the file containing the appropriate icon, and the index of the icon.
        hr = extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

        // Extract the icon.
        if (SUCCEEDED(hr)) {
            hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
        }
    
        // If the extraction failed, fall back to a 32x32 icon.
        if (hr == S_FALSE) {
            hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));
        }

        // And then to a 16x16
        if (hr == S_FALSE) {
            hr = extractIcon->Extract(iconFile, iconIndex, NULL, &icon, MAKELONG(0, 16));
        }

        if (hr == S_FALSE) {
            icon = ExtractIconW(gLSModule.GetInstance(), iconFile, iconIndex);
        }

        if (SUCCEEDED(hr) && icon != NULL) {
            AddIcon(icon);
        }
        else {
            TRACEW(L"Failed to extract icon %s,%i", iconFile, iconIndex);

            // Try to fall back to the default icon.
            icon = ExtractIconW(gLSModule.GetInstance(), L"shell32.dll", 1);

            if (icon != NULL) {
                AddIcon(icon);
                hr = S_FALSE;
            }
        }

        if (hr == S_FALSE && icon != NULL) {
            DestroyIcon(icon);
        }
    }

    // Let go of the interface.
    SAFERELEASE(extractIcon);
}
