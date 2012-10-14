/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Popup.cpp
 *  The nModules Project
 *
 *  Represents a popup box.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nShared/Debugging.h"
#include "Popup.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/MonitorInfo.hpp"
#include "FolderItem.hpp"


extern LSModule* g_LSModule;


Popup::Popup(LPCSTR title, LPCSTR bang, LPCSTR prefix) : Drawable("nPopup") {
    if (bang != NULL) {
        this->bang = _strdup(bang);
    }
    else {
        this->bang = NULL;
    }
    this->openChild = NULL;
    this->owner = NULL;

    this->itemSpacing = settings->GetInt("ItemSpacing", 2);
    settings->GetOffsetRect("PaddingLeft", "PaddingTop", "PaddingRight", "PaddingBottom", &this->padding, 5, 5, 5, 5);

    DrawableSettings* defaultSettings = new DrawableSettings();
    defaultSettings->color = 0x440000FF;
    defaultSettings->textRotation = -45.0f;
    defaultSettings->fontSize = 32.0f;
    defaultSettings->alwaysOnTop = true;
    StringCchCopyW(defaultSettings->text, MAX_LINE_LENGTH, L"nDemo");
    MultiByteToWideChar(CP_ACP, 0, title, (int)strlen(title)+1, defaultSettings->text, sizeof(defaultSettings->text)/sizeof(defaultSettings->text[0]));
    StringCchCopy(defaultSettings->textAlign, sizeof(defaultSettings->textAlign), "Center");
    StringCchCopy(defaultSettings->textVerticalAlign, sizeof(defaultSettings->textVerticalAlign), "Middle");
    this->window->Initialize(defaultSettings);
    SetParent(this->window->GetWindow(), NULL);
    SetWindowLongPtr(this->window->GetWindow(), GWL_EXSTYLE, GetWindowLongPtr(this->window->GetWindow(), GWL_EXSTYLE) & ~WS_EX_NOACTIVATE);
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
    if (this->window->IsVisible()) {
        Size();
        this->window->Repaint();
    }
}


void Popup::RemoveItem(PopupItem* item) {
}


void Popup::CloseChild(bool closing) {
    if (this->openChild != NULL) {
        if (!closing) {
            SetFocus(this->window->GetWindow());
            SetActiveWindow(this->window->GetWindow());
        }

        this->openChild->owner = NULL;
        ((nPopup::FolderItem*)this->childItem)->ClosingPopup();
        this->openChild->Close();
        this->childItem = NULL;
        this->openChild = NULL;
    }
}


void Popup::OpenChild(Popup* child, int y, int x, PopupItem* childItem) {
    if (child != this->openChild) {
        CloseChild();
        //RECT r;
        //this->window->GetScreenRect(&r);
        this->openChild = child;
        this->childItem = childItem;
        //this->openChild->Show(r.right, y, this);
        this->openChild->Show(x+this->padding.right, y, this);
    }
}


LPCSTR Popup::GetBang() {
    return this->bang;
}


bool Popup::CheckFocus(HWND newActive, __int8 direction) {
    if (this->window->GetWindow() == newActive || this->mouseOver)
        return true;
    return direction & 1 && this->owner && this->owner->CheckFocus(newActive, 1)
        || direction & 2 && this->openChild && this->openChild->CheckFocus(newActive, 2);
}


void Popup::Close() {
    TRACEW(L"Closing %s", this->window->GetDrawingSettings()->text);
    this->window->Hide();
    CloseChild(true);
    TRACEW(L"PostClose %s", this->window->GetDrawingSettings()->text);
    PostClose();
    if (this->owner != NULL) {
        this->owner->openChild = NULL;
        ((nPopup::FolderItem*)this->owner->childItem)->ClosingPopup();
        this->owner->childItem = NULL;
        this->owner->Close();
        this->owner = NULL;
    }
    this->mouseOver = false;
}


void Popup::Show() {
    POINT pt;
    GetCursorPos(&pt);
    Show(pt.x, pt.y);
}


void Popup::Size() {
    int width = 200, height = this->padding.top;
    for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
        (*iter)->Position(this->padding.left, height);
        height += (*iter)->GetHeight() + this->itemSpacing;
    }
    height += this->padding.bottom - this->itemSpacing;
    MonitorInfo* monInfo = this->window->GetMonitorInformation();

    if (height > monInfo->m_virtualDesktop.height) {
        int columns = (height - this->padding.top - this->padding.bottom)/(monInfo->m_virtualDesktop.height - this->padding.top - this->padding.bottom) + 1;
        width = 200 * columns + this->itemSpacing*(columns - 1);
        height = this->padding.top;
        int column = 0;
        int rowHeight = 0;
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            (*iter)->Position(this->padding.left + (200 + this->itemSpacing) * column, height);
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
    this->window->Resize(width, height);
    this->sized = true;
}


void Popup::Show(int x, int y, Popup* owner) {
    this->owner = owner;
    SetParent(this->window->GetWindow(), NULL);
    PreShow();

    MonitorInfo* monInfo = this->window->GetMonitorInformation();

    if (!this->sized) {
        Size();
    }

    x = max(monInfo->m_virtualDesktop.rect.left, min(monInfo->m_virtualDesktop.rect.right - this->window->GetDrawingSettings()->width, x));
    y = max(monInfo->m_virtualDesktop.rect.top, min(monInfo->m_virtualDesktop.rect.bottom - this->window->GetDrawingSettings()->height, y));

    this->window->Move(x, y);

    this->window->Show();
    SetWindowPos(this->window->GetWindow(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetFocus(this->window->GetWindow());
    SetActiveWindow(this->window->GetWindow());
}


LRESULT Popup::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && this->window->IsVisible()) {
            if (!CheckFocus((HWND)lParam, 3)) {
                Close();
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
