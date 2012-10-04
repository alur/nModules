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


Popup::Popup(LPCSTR title, LPCSTR bang, LPCSTR prefix) {
    if (bang != NULL) {
        this->bang = _strdup(bang);
    }
    else {
        this->bang = NULL;
    }
    this->settings = new Settings("nPopup");
    DrawableSettings* defaultSettings = new DrawableSettings();
    defaultSettings->color = 0x440000FF;
    defaultSettings->textRotation = -45.0f;
    defaultSettings->fontSize = 32.0f;
    StringCchCopyW(defaultSettings->text, MAX_LINE_LENGTH, L"Demo");
    StringCchCopy(defaultSettings->textAlign, sizeof(defaultSettings->textAlign), "Center");
    StringCchCopy(defaultSettings->textVerticalAlign, sizeof(defaultSettings->textVerticalAlign), "Middle");
    this->window = new DrawableWindow(NULL, (LPCSTR)g_LSModule->GetWindowClass(1), g_LSModule->GetInstance(), this->settings, defaultSettings, this);
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
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
}


void Popup::AddItem(PopupItem* item) {
    this->items.push_back(item);
    this->sized = false;
}


LPCSTR Popup::GetBang() {
    return this->bang;
}


void Popup::Show() {
    POINT pt;
    GetCursorPos(&pt);
    Show(pt.x, pt.y);
}


void Popup::Show(int x, int y) {
     DrawableSettings* drawingSettings = this->window->GetSettings();
     drawingSettings->x = x;
     drawingSettings->y = y;

     if (!this->sized) {
         drawingSettings->width = 200;
         drawingSettings->height = 400;
         this->sized = true;
     }

     this->window->UpdatePosition();
     this->window->Show();
}


LRESULT Popup::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            this->window->Hide();
        }
        return 0;

    default:
        return this->window->HandleMessage(window, msg, wParam, lParam);
    }
}
