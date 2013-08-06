/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Popup.hpp"
#include "../nShared/LSModule.hpp"
#include <shellapi.h>


extern LSModule gLSModule;


PopupItem::PopupItem(Drawable* parent, LPCTSTR prefix, bool independent) : Drawable(parent, prefix, independent) {
    this->iconSettings = mSettings->CreateChild(L"Icon");
    this->itemType = PopupItemType::SEPARATOR;
}


PopupItem::~PopupItem() {
    SAFEDELETE(this->iconSettings);
}


void PopupItem::Position(int x, int y) {
    mWindow->Move(x, y);
}


int PopupItem::GetHeight() {
    return mWindow->GetDrawingSettings()->height;
}


void PopupItem::SetWidth(int width) {
    mWindow->Resize(width, mWindow->GetDrawingSettings()->height);
}


bool PopupItem::CheckMerge(LPCWSTR name) {
    return this->itemType == PopupItemType::FOLDER && _wcsicmp(name, mWindow->GetText()) == 0;
}


bool PopupItem::ParseDotIcon(LPCTSTR dotIcon) {
    if (dotIcon == NULL || ((Popup*)mParent)->noIcons) {
        return false;
    }

    // TODO::May cause problems with paths which include a comma.
    LPTSTR index = (LPTSTR)_tcsrchr(dotIcon, L',');
    int nIndex;
    if (index != NULL) {
        index++[0] = NULL;
        nIndex = _ttoi(index);
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
    f.top = this->iconSettings->GetFloat(_T("Y"), 2.0f);
    f.bottom = f.top + this->iconSettings->GetFloat(_T("Size"), 16.0f);
    f.left = this->iconSettings->GetFloat(_T("X"), 2.0f);
    f.right = f.left + this->iconSettings->GetFloat(_T("Size"), 16.0f);
    this->iconOverlay = mWindow->AddOverlay(f, icon);
}


bool PopupItem::CompareTo(PopupItem* b) {
    return this->itemType > b->itemType || this->itemType == b->itemType && _wcsicmp(mWindow->GetText(), b->mWindow->GetText()) < 0;
}


void PopupItem::SetIcon(IExtractIconW* extractIcon) {
    HICON icon = NULL;
    WCHAR iconFile[MAX_PATH];
    int iconIndex = 0;
    UINT flags;
    HRESULT hr;

    if (!((Popup*)mParent)->noIcons) {

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
