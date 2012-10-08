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
    this->sized = false;
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


void Popup::CloseChild() {
    if (this->openChild != NULL) {
        this->openChild->Close(false);
        this->openChild = NULL;
    }
}


void Popup::OpenChild(Popup* child, int y) {
    if (child != this->openChild) {
        if (this->openChild != NULL) {
            this->openChild->Close(false);
        }
        RECT r;
        this->window->GetScreenRect(&r);
        this->openChild = child;
        this->openChild->Show(r.right, y, this);
    }
}


LPCSTR Popup::GetBang() {
    return this->bang;
}


void Popup::HandleInactivate(HWND window) {
    if (this->window->GetWindow() != window) {
        Close(false);
    
        if (this->owner) {
            this->owner->HandleInactivate(window);
        }
    }
}


void Popup::Close(bool closeAll) {
    this->window->Hide();
    if (this->openChild != NULL) {
        this->openChild->Close(false);
    }
    if (this->owner != NULL) {
        this->owner->ChildClosing(closeAll);
    }
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
    this->window->Move(x, y);
    this->owner = owner;

    if (!this->sized) {
        int width = 200, height = this->padding.top;
        for (vector<PopupItem*>::const_iterator iter = this->items.begin(); iter != this->items.end(); iter++) {
            (*iter)->Position(this->padding.left, height);
            height += (*iter)->GetHeight() + this->itemSpacing;
        }
        height += this->padding.bottom - this->itemSpacing;
        this->window->SetPosition(x, y, width, height);
        this->sized = true;
    }

    this->window->Show();
    SetWindowPos(this->window->GetWindow(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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

    default:
        return DefWindowProc(window, msg, wParam, lParam);
    }
}
