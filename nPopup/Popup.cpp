/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Popup.cpp
 *  The nModules Project
 *
 *  Represents a popup box.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "Popup.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/MonitorInfo.hpp"
#include "FolderItem.hpp"
#include "../Utilities/Math.h"


Popup::Popup(LPCTSTR title, LPCTSTR bang, LPCTSTR prefix) : Drawable(prefix) {
    if (bang != NULL) {
        this->bang = _tcsdup(bang);
    }
    else {
        this->bang = NULL;
    }
    this->openChild = NULL;
    this->owner = NULL;

    this->itemSpacing = mSettings->GetInt(_T("ItemSpacing"), 2);
    this->maxWidth = mSettings->GetInt(_T("MaxWidth"), 300);
    this->noIcons = mSettings->GetBool(_T("NoIcons"), false);
    this->expandLeft = mSettings->GetBool(_T("ExpandLeft"), false);
    this->confineToMonitor = mSettings->GetBool(_T("ConfineToMonitor"), false);
    this->confineToWorkArea = mSettings->GetBool(_T("ConfineToWorkArea"), false);
    mChildOffsetX = mSettings->GetInt(_T("ChildOffsetX"), 0);
    mChildOffsetY = mSettings->GetInt(_T("ChildOffsetY"), 0);
    this->padding = mSettings->GetOffsetRect(_T("Padding"), 5, 5, 5, 5);

    mPopupSettings.Load(mSettings);

    WindowSettings defaultSettings, windowSettings;
    defaultSettings.alwaysOnTop = true;
    defaultSettings.width = 200;
    windowSettings.Load(mSettings, &defaultSettings);

    StateRender<State>::InitData initData;
    initData[State::Base].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0x440000FF);
    initData[State::Base].defaults.fontSize = 32.0f;
    initData[State::Base].defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    initData[State::Base].defaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    mStateRender.Load(initData, mSettings);

    mWindow->Initialize(windowSettings, &mStateRender);
    mWindow->SetText(title);

    SetParent(mWindow->GetWindowHandle(), NULL);
    SetWindowLongPtr(mWindow->GetWindowHandle(), GWL_EXSTYLE, GetWindowLongPtr(mWindow->GetWindowHandle(), GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
    this->sized = false;
    this->mouseOver = false;
    this->childItem = NULL;
}


Popup::~Popup() {
    for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
        delete *iter;
    }
    this->items.clear();
    if (this->bang != NULL) {
        free((LPVOID)this->bang);
    }
}


void Popup::AddItem(PopupItem* item) {
    this->items.push_back(item);
    this->sized = false;
    if (mWindow->IsVisible()) {
        Size();
        mWindow->Repaint();
    }
}


void Popup::RemoveItem(PopupItem* /* item */) {
}


void Popup::CloseChild(bool closing) {
    if (this->openChild != NULL) {
        if (!closing) {
            SetFocus(mWindow->GetWindowHandle());
            SetActiveWindow(mWindow->GetWindowHandle());
        }

        this->openChild->owner = NULL;
        ((nPopup::FolderItem*)this->childItem)->ClosingPopup();
        this->openChild->Close();
        this->childItem = NULL;
        this->openChild = NULL;
    }
}


void Popup::OpenChild(Popup* child, LPRECT position, PopupItem* childItem) {
    if (child != this->openChild) {
        CloseChild();
        this->openChild = child;
        this->childItem = childItem;
        this->openChild->expandLeft = this->expandLeft;
        position->top -= this->padding.top + mChildOffsetY;
        position->bottom -= this->padding.top;
        position->left -= this->padding.left + mChildOffsetX;
        position->right += this->padding.right + mChildOffsetX;
        this->openChild->Show(position, this);
    }
}


LPCTSTR Popup::GetBang() {
    return this->bang;
}


bool Popup::CheckFocus(HWND newActive, __int8 direction) {
    if (mWindow->GetWindowHandle() == newActive || this->mouseOver)
        return true;
    return direction & 1 && this->owner && this->owner->CheckFocus(newActive, 1)
        || direction & 2 && this->openChild && this->openChild->CheckFocus(newActive, 2);
}


void Popup::Close()
{
    mWindow->Hide();
    this->expandLeft = mSettings->GetBool(L"ExpandLeft", false);
    CloseChild(true);
    //PostClose();
    this->mouseOver = false;
    if (this->owner != NULL)
    {
        this->owner->openChild = NULL;
        ((nPopup::FolderItem*)this->owner->childItem)->ClosingPopup();
        this->owner->childItem = NULL;
        Popup* owner = this->owner;
        this->owner = NULL;
        owner->Close();
    }
}


void Popup::Show() {
    POINT pt;
    GetCursorPos(&pt);
    Show(pt.x, pt.y);
}


void Popup::Show(int x, int y) {
    RECT r;
    r.left = x - 1; r.right = x + 1;
    r.top = y - 1; r.bottom = y + 1;
    Show(&r);
}


void Popup::Size() {
    // Work out the desired item 
    int itemWidth = mSettings->GetInt(_T("Width"), 200) - this->padding.left - this->padding.right;
    int maxItemWidth = this->maxWidth - this->padding.left - this->padding.right;
    int height = this->padding.top;
    int width;
    for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
        (*iter)->Position(this->padding.left, height);
        height += (*iter)->GetHeight() + this->itemSpacing;
        itemWidth = max(itemWidth, (*iter)->GetDesiredWidth(maxItemWidth));
    }
    width = itemWidth + this->padding.left + this->padding.right;
    height += this->padding.bottom - this->itemSpacing;
    MonitorInfo* monInfo = mWindow->GetMonitorInformation();

    // We've excceeded the max height, split the popup into columns.
    if (height > monInfo->m_virtualDesktop.height) {
        int columns = (height - this->padding.top - this->padding.bottom)/(monInfo->m_virtualDesktop.height - this->padding.top - this->padding.bottom) + 1;
        int columnWidth = width;
        width = columnWidth * columns + this->itemSpacing*(columns - 1);
        height = this->padding.top;
        int column = 0;
        int rowHeight = 0;
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            (*iter)->Position(this->padding.left + (columnWidth + this->itemSpacing) * column, height);
            rowHeight = max((*iter)->GetHeight() + this->itemSpacing, rowHeight);
            column++;
            if (column == columns) {
                height += rowHeight;
                rowHeight = 0;
                column = 0;
            }
        }
        if (column != 0) {
            height += rowHeight;
        }
        height += this->padding.bottom - this->itemSpacing;
    }

    // Size all items properly
    for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
        (*iter)->SetWidth(itemWidth);
    }

    // Size the main window
    mWindow->Resize(width, height);
    this->sized = true;
}


void Popup::Show(LPRECT position, Popup* owner) {
    this->owner = owner;
    SetParent(mWindow->GetWindowHandle(), NULL);
    PreShow();

    MonitorInfo* monInfo = mWindow->GetMonitorInformation();

    if (!this->sized) {
        Size();
    }

    int x, y;
    RECT limits = monInfo->m_virtualDesktop.rect;

    if (this->confineToMonitor && this->confineToWorkArea) {
        limits = monInfo->m_monitors.at(monInfo->MonitorFromRECT(position)).workArea;
    }
    else if (this->confineToMonitor) {
        limits = monInfo->m_monitors.at(monInfo->MonitorFromRECT(position)).rect;
    }
    else if (this->confineToWorkArea) {
        // TODO::Shouldn't confine to the monitor here.
        limits = monInfo->m_monitors.at(monInfo->MonitorFromRECT(position)).workArea;
    }


    if (this->expandLeft) {
        x = position->left - mWindow->GetDrawingSettings()->width;
        if (x < limits.left) {
            this->expandLeft = false;
            x = this->owner ? position->right : limits.left;
        }
    }
    else {
        x = position->right;
        if (x > limits.right - mWindow->GetDrawingSettings()->width) {
            this->expandLeft = true;
            x = (this->owner ? position->left : limits.right) - mWindow->GetDrawingSettings()->width;
        }
    }

    y = max(limits.top, min(limits.bottom - mWindow->GetDrawingSettings()->height, position->top));

    mWindow->Move(x, y);

    mWindow->Show();
    SetWindowPos(mWindow->GetWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetFocus(mWindow->GetWindowHandle());
    SetActiveWindow(mWindow->GetWindowHandle());
}


LRESULT Popup::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg)
    {
    case Window::WM_HIDDEN:
        {
            PostClose();
        }
        return 0;

    case WM_ACTIVATE:
        {
            if (LOWORD(wParam) == WA_INACTIVE && mWindow->IsVisible())
            {
                if (!CheckFocus((HWND)lParam, 3))
                {
                    Close();
                }
            }
        }
        return 0;

    case WM_MOUSEMOVE:
        this->mouseOver = true;
        return 0;

    case WM_MOUSELEAVE:
        this->mouseOver = false;
        return 0;

    default:
        return DefWindowProc(window, msg, wParam, lParam);
    }
}
