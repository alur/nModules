/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Popup.cpp
 *  The nModules Project
 *
 *  Represents a popup box.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nShared/Debugging.h"
#include "Popup.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/MonitorInfo.hpp"


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
}


void Popup::RemoveItem(PopupItem* item) {
}


void Popup::CloseChild() {
    if (this->openChild != NULL) {
        this->openChild->owner = NULL;
        this->openChild->Close(false);
        this->openChild = NULL;
    }
}


void Popup::OpenChild(Popup* child, int y, int x) {
    if (child != this->openChild) {
        if (this->openChild != NULL) {
            this->openChild->Close(false);
        }
        //RECT r;
        //this->window->GetScreenRect(&r);
        this->openChild = child;
        //this->openChild->Show(r.right, y, this);
        this->openChild->Show(x+this->padding.right, y, this);
    }
}


LPCSTR Popup::GetBang() {
    return this->bang;
}


void Popup::HandleInactivate(HWND window) {
    if (this->window->GetWindow() != window && !this->mouseOver) {
        Close(false);
    
        if (this->owner) {
            this->owner->HandleInactivate(window);
        }
    }
}


void Popup::Close(bool closeAll) {
    this->window->Hide();
    if (this->openChild != NULL) {
        this->openChild->owner = NULL;
        this->openChild->Close(true);
    }
    if (this->owner != NULL) {
        this->owner->ChildClosing(closeAll);
    }
    this->mouseOver = false;
    PostClose();
}


void Popup::ChildClosing(bool close) {
    this->openChild = NULL;
    if (close) {
        Close();
    }
}


void Popup::Show() {
    POINT pt;
    GetCursorPos(&pt);
    Show(pt.x, pt.y);
}


void Popup::Show(int x, int y, Popup* owner) {
    PreShow();
    this->owner = owner;

    MonitorInfo* monInfo = this->window->GetMonitorInformation();

    if (!this->sized) {
        int width = 200, height = this->padding.top;
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            (*iter)->Position(this->padding.left, height);
            height += (*iter)->GetHeight() + this->itemSpacing;
        }
        height += this->padding.bottom - this->itemSpacing;

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
        this->window->SetPosition(x, y, width, height);
        this->sized = true;
    }

    x = max(monInfo->m_virtualDesktop.rect.left, min(monInfo->m_virtualDesktop.rect.right - this->window->GetDrawingSettings()->width, x));
    y = max(monInfo->m_virtualDesktop.rect.top, min(monInfo->m_virtualDesktop.rect.bottom - this->window->GetDrawingSettings()->height, y));

    this->window->Move(x, y);

    this->window->Show();
    SetWindowPos(this->window->GetWindow(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    SetActiveWindow(this->window->GetWindow());
}


LRESULT Popup::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            if (this->openChild == NULL) {
                Close(false);
                if (this->owner) {
                    this->owner->HandleInactivate((HWND)lParam);
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
