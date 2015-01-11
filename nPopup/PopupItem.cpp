/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.hpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Popup.hpp"
#include "../nShared/LSModule.hpp"
#include "IconCache.hpp"
#include <shellapi.h>


extern LSModule gLSModule;
static IconCache iconCache;


PopupItem::PopupItem(Drawable* parent, LPCTSTR prefix, Type type, bool independent)
    : Drawable(parent, prefix, independent)
    , mItemType(type)
    , mHasIcon(false)
{
    this->iconSettings = mSettings->CreateChild(L"Icon");
}


PopupItem::~PopupItem()
{
    SAFEDELETE(this->iconSettings);
    if (mHasIcon)
    {
        iconCache.ReleaseIcon(mIconHash);
    }
}


void PopupItem::Position(int x, int y)
{
    mWindow->Move((float)x, (float)y);
}


int PopupItem::GetHeight()
{
    return (int)(mWindow->GetSize().height + 0.5f);
}


void PopupItem::SetWidth(int width)
{
    mWindow->Resize((float)width, mWindow->GetSize().height);
}


bool PopupItem::CheckMerge(LPCWSTR name)
{
    return mItemType == Type::Folder && _wcsicmp(name, mWindow->GetText()) == 0;
}


bool PopupItem::ParseDotIcon(LPCTSTR dotIcon)
{
    if (dotIcon == NULL || ((Popup*)mParent)->noIcons)
    {
        return false;
    }

    // TODO::May cause problems with paths which include a comma.
    LPTSTR index = (LPTSTR)wcsrchr(dotIcon, L',');
    int nIndex;
    if (index != NULL) {
        index++[0] = NULL;
        nIndex = _wtoi(index);
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
    f.top = this->iconSettings->GetFloat(L"Y", 2.0f);
    f.bottom = f.top + this->iconSettings->GetFloat(L"Size", 16.0f);
    f.left = this->iconSettings->GetFloat(L"X", 2.0f);
    f.right = f.left + this->iconSettings->GetFloat(L"Size", 16.0f);
    this->iconOverlay = mWindow->AddOverlay(f, icon);
}


bool PopupItem::CompareTo(PopupItem* b) {
    return mItemType > b->mItemType || mItemType == b->mItemType && _wcsicmp(mWindow->GetText(), b->mWindow->GetText()) < 0;
}


void PopupItem::SetIcon(IExtractIconW* extractIcon) {
    HICON icon = NULL;
    WCHAR iconFile[MAX_PATH];
    int iconIndex = 0;
    UINT flags;
    HRESULT hr;

    if (!((Popup*)mParent)->noIcons)
    {
        // Get the location of the file containing the appropriate icon, and the index of the icon.
        hr = extractIcon->GetIconLocation(GIL_FORSHELL, iconFile, MAX_PATH, &iconIndex, &flags);

        //
        if (SUCCEEDED(hr))
        {
            mIconHash = iconCache.ComputeHash(iconFile, iconIndex);

            icon = iconCache.GetIcon(mIconHash);

            if (icon)
            {
                mHasIcon = true;
                AddIcon(icon);
            }
            else
            {
                // Extract the icon.
                if (SUCCEEDED(hr))
                {
                    hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(64, 0));
                }
    
                // If the extraction failed, fall back to a 32x32 icon.
                if (hr == S_FALSE)
                {
                    hr = extractIcon->Extract(iconFile, iconIndex, &icon, NULL, MAKELONG(32, 0));
                }

                // And then to a 16x16
                if (hr == S_FALSE)
                {
                    hr = extractIcon->Extract(iconFile, iconIndex, NULL, &icon, MAKELONG(0, 16));
                }

                if (hr == S_FALSE)
                {
                    icon = ExtractIconW(gLSModule.GetInstance(), iconFile, iconIndex);
                }

                if (SUCCEEDED(hr) && icon != NULL)
                {
                    AddIcon(icon);
                }
                else
                {
                    TRACEW(L"Failed to extract icon %s,%i", iconFile, iconIndex);

                    // Try to fall back to the default icon.
                    icon = ExtractIconW(gLSModule.GetInstance(), L"shell32.dll", 1);

                    if (icon != NULL)
                    {
                        AddIcon(icon);
                        hr = S_FALSE;
                    }
                }

                if (icon != nullptr)
                {
                    mHasIcon = true;
                    iconCache.StoreIcon(mIconHash, icon);
                }
            }
        }
    }

    // Let go of the interface.
    SAFERELEASE(extractIcon);
}
